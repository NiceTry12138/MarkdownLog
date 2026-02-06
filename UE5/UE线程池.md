# UE 的线程池

什么是线程池？

```
┌──────────────────────────────────────────────────────────────────────────┐
│                           线程池概念图                                    │
├──────────────────────────────────────────────────────────────────────────┤
│                                                                          │
│    ┌─────────────┐                        ┌─────────────────────────┐    │
│    │   Task 1    │ ─┐                     │     Worker Thread 1     │    │
│    └─────────────┘  │                     ├─────────────────────────┤    │
│    ┌─────────────┐  │    ┌──────────┐     │     Worker Thread 2     │    │
│    │   Task 2    │ ─┼──► │  队列    │ ──► ├─────────────────────────┤    │
│    └─────────────┘  │    │ (Queue)  │     │     Worker Thread 3     │    │
│    ┌─────────────┐  │    └──────────┘     ├─────────────────────────┤    │
│    │   Task 3    │ ─┘                     │         ...             │    │
│    └─────────────┘                        └─────────────────────────┘    │
│                                                                          │
└──────────────────────────────────────────────────────────────────────────┘
```

不使用线程池，会不停的创建线程，然后把任务交给线程，频繁创建/销毁线程 的代价十分高昂

线程池解决的核心问题

1. 线程创建开销大，创建线程需要分配栈控件、初始化 TLS 等
2. 上下文切换代价，过多线程会导致频繁的 CPU 上下文切换
3. 资源管理复杂，手动管理每个线程的声明周期容器出错

通过 线程池，预先创建一组线程，并且重复利用这些线程

UE 中提前定义了多个全局线程池

```cpp
/** The global thread pool */
FQueuedThreadPool* GThreadPool = nullptr;            // 通用线程池
FQueuedThreadPool* GIOThreadPool = nullptr;          // I/O 专用线程池
FQueuedThreadPool* GBackgroundPriorityThreadPool = nullptr;  // 后台低优先级线程池

#if WITH_EDITOR
FQueuedThreadPool* GLargeThreadPool = nullptr;       // 编辑器大型任务线程池
#endif
```

UE 中的分层架构

1. 用户层：提供 `AsyncTask`、`ParallelFor`、`UE::Tasks` 等接口，方便用户使用
2. 接口层：使用 `FQueuedThreadPool` 作为线程池接口
   1. 提供 `Create` 创建线程池
   2. 提供 `Destroy` 销毁线程池
   3. 提供 `AddQueuedWork` 添加任务
   4. 提供 `RestractQueuedWork` 撤回任务
3. 实现层：通过继承 `FQueuedThreadPoolBase` 来实现不同的功能
4. 具体工作线程层

UE 做了一些设计

| 设计 | 做了声明 | 为什么这么做 |
| --- | --- | --- |
| 多线程池分离 | 创建 GThreadPool、GIOThreadPool、GBackgroundPriorityThreadPool | I/O密集型和 CPU 密集型任务分开，避免互相阻塞 |
| 优先级队列 | 6 级优先级（Blocking -> Lowest） | 关键任务优先执行，低优先级任务不阻塞重要工作 |
| 从数组末尾取线程 | QueuedThreads[ThreadIndex] （末尾） | 利用 CPU 缓存局部性，最近用过的线程缓存更热 |
| Work Stealing | Worker 可以从其他 Worker 的队列偷取任务 | 负载军星，避免某些 Worker 空闲 |
| Lock-Free 睡眠栈 | TEventStack 使用 Treiber Stack | 减少锁竞争，提高唤醒/睡眠效率 |
| Busy Wait | 在等待时执行其他任务 | 避免线程公转，提高 CPU 利用率 |
| Fake Threa | 单线程环境下的模拟线程 | 支持不支持多线程的平台 |
| Forkable Threa | 支持 fork 后的多线程 | 支持服务器场景的进程复制 |

UE 线程池类层次结构

- 接口层
  - `FRunnable` 可运行对象接口，提供 `Init`、`Run`、`Stop`、`Exit`
  - `IQueuedWork` 排队工作项接口，提供 `DoThreadedWork`、`Abandaon`
- 线程层
  - `FRunnableThread` 抽象基类
    - `FRunnableThreadWin` Windows 实现
    - `FRunnablePThread` POSIX 实现
    - `FFakeThrea` 单线程模拟
    - `FForkableThread` 可分叉线程
  - `FQueuedThread` 线程池工作线程
    - 继承自 FRunnable，在 `Run` 中实现工作循环
- 线程池层
  - `FQueuedThreadPool` 抽象接口
    - `FQueuedThreadPoolBase` 具体实现
      - `FThreadPoolPriorityQueue` 优先级任务队列
      - `TArray<FQueuedThread*> QueuedThreads` 空闲线程
      - `TArray<FQueuedThread*> AllThreads` 所有线程
- 管理层
  - `FThreadManager` 线程管理器单例
    - `TMap<uint32, FRunnableTHread*> Threads`
    - `Addthread`/`RemoveThread`
    - `Tick` 单线程模式下 `tick` 所有 `fake` 线程
  - `FEvent` 事件同步原语
    - `Trigger` 触发事件
    - `Wait` 等待事件
    - `Reset` 重置事件

## 数据结构

### FRunnable

```cpp
class FRunnable
{
public:
    virtual bool Init() { return true; }   // 线程上下文中的初始化
    virtual uint32 Run() = 0;              // 纯虚函数：核心工作
    virtual void Stop() { }                // 外部请求停止
    virtual void Exit() { }                // 清理资源
    
    // 单线程模式下的 Tick 支持
    virtual class FSingleThreadRunnable* GetSingleThreadInterface() { return nullptr; }
};
```

- `FRunnable` 只关心业务逻辑，完全不知道自己在哪个线程上运行
- 无平台依赖性，不关心任何 `HANDLE`、`pthread_t`` 或者其他系统概念
- 可测试性，可以直接在主线程调用

### FRunnableThread 

```cpp
// RunnableThread.h
class FRunnableThread
{
protected:
    FString ThreadName;           // 线程名称（调试用）
    FRunnable* Runnable;          // 持有的业务对象
    FEvent* ThreadInitSyncEvent;  // 初始化同步事件
    uint64 ThreadAffinityMask;    // CPU 亲和性掩码
    EThreadPriority ThreadPriority; // 线程优先级
    uint32 ThreadID;              // 系统线程 ID

public:
    // 工厂方法 - 创建适合当前平台的线程
    static FRunnableThread* Create(FRunnable* InRunnable, const TCHAR* ThreadName, ...);
    
    virtual void SetThreadPriority(EThreadPriority NewPriority) = 0;
    virtual void Suspend(bool bShouldPause) = 0;
    virtual bool Kill(bool bShouldWait) = 0;
    virtual void WaitForCompletion() = 0;
};
```

```
┌────────────────────────────────────────────────────────────────────────────────┐
│                            架构示意图                                           │
├────────────────────────────────────────────────────────────────────────────────┤
│                                                                                │
│    ┌─────────────────────────────┐       ┌─────────────────────────────────┐   │
│    │       FRunnable             │       │      FRunnableThread            │   │
│    │  (抽象业务接口)              │       │   (抽象线程容器)                 │   │
│    ├─────────────────────────────┤       ├─────────────────────────────────┤   │
│    │  + Init()    → 初始化逻辑   │        │  + Create()    → 工厂方法        │   │
│    │  + Run()     → 核心工作     │ ◄───┐  │  + Kill()      → 终止线程        │   │
│    │  + Stop()    → 停止请求     │     │  │  + Suspend()   → 暂停/恢复       │   │
│    │  + Exit()    → 清理逻辑     │     │  │  + SetPriority() → 设置优先级    │   │
│    └─────────────────────────────┘     │ │  # Runnable*   → 持有业务对象    │   │
│                 ▲                      │ └────────────────┬────────────────┘   │
│                 │ 实现                 │ 调用              │ 派生               │
│    ┌────────────┴───────────┐          │      ┌───────────┴───────────┐        │
│    │     FQueuedThread      │          └──────┤  FRunnableThreadWin    │       │
│    │    (线程池工作者)       │                 │  FRunnableThreadPThread│       │
│    │    FMyCustomTask       │                 │  FFakeThread           │       │
│    └────────────────────────┘                 └───────────────────────┘        │
│                                                                                │
└────────────────────────────────────────────────────────────────────────────────┘
```

为什么需要 `FRunnableThread`？

1. 平台抽象

不同操作系统的线程 API 完全不同

| 操作系统 | 创建线程 | 等待线程 | 终止线程 |
| --- | --- | --- | --- |
| `Windows` | `CreateThread` | `WaitForSingleObject` | `CloseHandle` |
| `Linux/Mac` | `pthread_create` | `pthread_join` | `pthread_cancel` |

如果没有 `FRunnableThread`

- `FRunnable` 必须直接调用 `CreateThread` 或者 `pthread_create`
- 每个业务逻辑类都要写平台判断代码 `#iddef _WIN32`
- 代码无法跨平台服用

2. 初始化同步 (Init Synchronization)

3. 单线程模式支持

某些平台不支持多线程，UE 通过 `FFakeThread` 实现了 **假线程**

4. TLS(线程本地存储) 管理

```cpp
class FRunnableThread
{
	static CORE_API uint32 RunnableTlsSlot;
}
```

5. 异常处理和崩溃报告，保证每个工作线程都有崩溃保护，并且崩溃报告能够显示哪个线程出现了问题，同时让业务代码不需要关系这些

```cpp
uint32 FRunnableThreadWin::GuardedRun()
```

6. 职责分离，业务开发者只需要继承 `FRunnable` 实现具体的开发逻辑就行，不需要关心平台差异、异常处理、TLS等信息，UE 会提供跨平台线程创建、优先级设置、同步和崩溃报告

### FQueuedThread

```cpp
class FQueuedThread : public FRunnable
{
protected:
    /** 等待工作的事件信号 - 线程休眠/唤醒的关键 */
    FEvent* DoWorkEvent = nullptr;

    /** 线程退出标志 - 原子操作保证线程安全 */
    TAtomic<bool> TimeToDie { false };

    /** 当前正在执行的任务 - volatile 确保多线程可见性 */
    IQueuedWork* volatile QueuedWork = nullptr;

    /** 所属的线程池 - 用于任务完成后回调 */
    class FQueuedThreadPoolBase* OwningThreadPool = nullptr;

    /** 实际的系统线程包装器 */
    FRunnableThread* Thread = nullptr;

public:
    virtual void Create(...);
    void KillThread();
    void DoWork(...);
};
```

`FQueuedThread` 是 UE 线程池中的工作线程类，它是连接 **系统线程** 和 **任务队列** 的桥梁

也就是说 用户提交任务 之后，会给到 `FQueuedThreadPoolBase::QueuedWork` 任务队列，再从 `FQueuedThreadPoolBase::QueuedThreads` 工作线程池中选择数组末尾线程来执行

```cpp
uint32 FQueuedThread::Run()
{
	while (!TimeToDie.Load(EMemoryOrder::Relaxed))
    {
		SET_DWORD_STAT(STAT_ThreadPoolDummyCounter, 0);
		bool bContinueWaiting = true;
        
        // 阶段 1: 等待工作（休眠）
		if (bContinueWaiting)
		{
			DoWorkEvent->Wait();
		}

        // 阶段 2: 获取任务
		IQueuedWork* LocalQueuedWork = QueuedWork;
		QueuedWork = nullptr;
		FPlatformMisc::MemoryBarrier();
        
        // 阶段 3: 执行任务（可能连续执行多个）
		while (LocalQueuedWork)
		{
			LocalQueuedWork->DoThreadedWork();
			LocalQueuedWork = OwningThreadPool->ReturnToPoolOrGetNextJob(this);
		}
    }
}
```

当执行完任务之后，会通过 `ReturnToPoolOrGetNextJob` 来决定是尝试从任务队列中获取下一个任务，还是将自己放到空闲线程池中

使用 `DoWorkEvent->Wait()` 是 Event 等待，此时无任务时的 CPU 占用是 0%，响应时间也快

如果使用忙等待，则无任务时 CPU 占用是 100%

```cpp
// CPU 疯狂空转，即使没有任务也占用 100% CPU
while (!TimeToDie) {
    if (QueuedWork != nullptr) {
        QueuedWork->DoThreadedWork();
        QueuedWork = nullptr;
    }
}
```



# BS_thread_pool 源码解读

源码路径： https://github.com/bshoshany/thread-pool/blob/master/tests/BS_thread_pool_test.cpp

## 线程池

线程池（Thread Pool） 是一种并发编程的设计模式，用于管理和复用多个线程，以便高效地执行大量任务。线程池的核心思想是预先创建一组线程，并将任务分配给这些线程执行，而不是为每个任务都创建一个新的线程。通过这种方式，线程池可以**减少线程创建和销毁的开销**，提高系统的性能和资源利用率。

- 线程创建和销毁的开销：
  - 创建和销毁线程是一个相对昂贵的操作，涉及到操作系统资源的分配和回收。频繁地创建和销毁线程会导致系统性能下降。
  - 例如，假设你有一个任务需要执行 1000 次，如果每次任务都创建一个新线程，那么系统将创建 1000 个线程，这会导致大量的资源消耗和上下文切换开销。

- 资源管理问题：
  - 如果不加以控制，系统中可能会同时存在大量的线程，导致系统资源（如内存、CPU）被过度占用，甚至可能导致系统崩溃。
  - 例如，在一个 Web 服务器中，如果每个请求都创建一个新线程来处理，那么在并发请求量很大的情况下，系统可能会因为线程过多而耗尽内存。

- 任务调度的复杂性：
  - 手动管理多个线程的创建、销毁和任务分配会增加代码的复杂性，容易引入 bug。
  - 例如，如果你需要手动管理 100 个线程的任务分配和同步，代码会变得非常复杂且难以维护。

## 简易线程池

```cpp
#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

class ThreadPool {
public:
    ThreadPool(size_t numThreads);
    ~ThreadPool();

    void enqueue(const std::function<void()> &task);

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex queueMutex;
    std::condition_variable condition;
    std::atomic<bool> stop;

    void worker();
};

ThreadPool::ThreadPool(size_t numThreads) : stop(false) {
    for (size_t i = 0; i < numThreads; ++i) {
        workers.emplace_back([this] { worker(); });
    }
}

ThreadPool::~ThreadPool() {
    stop = true;
    condition.notify_all();
    for (std::thread &worker : workers) {
        worker.join();
    }
}

void ThreadPool::enqueue(const std::function<void()> &task) {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        tasks.push(task);
    }
    condition.notify_one();
}

void ThreadPool::worker() {
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            condition.wait(lock, [this] { return stop || !tasks.empty(); });
            if (stop && tasks.empty()) return;
            task = std::move(tasks.front());
            tasks.pop();
        }
        task();
    }
}

int main() {
    ThreadPool pool(4);

    for (int i = 0; i < 8; ++i) {
        pool.enqueue([i] {
            std::cout << "Processing task " << i << std::endl;
        });
    }

    std::this_thread::sleep_for(std::chrono::seconds(2));
    return 0;
}
```

- `workers`：一个 `std::vector`，存储线程对象
- `tasks`：一个 `std::queue`，存储待执行的任务
- `queueMutex`：互斥锁，用于保护对 `tasks` 队列的访问，避免多线程竞争
- `condition`：条件变量，用于通知线程有任务可以执行
- `stop`：一个 `std::atomic<bool>` 标志，用于标记线程池是否已经停止。线程池在被销毁时会将 `stop` 设置为 `true`

在 `ThreadPool` 创建的时候，就会创建指定数量的线程，并进入阻塞状态，在 `enqueue` 添加任务的时候调用 `condition.notify_one()` 唤醒一个等待中的线程，通知它有新任务可以处理

`condition.notify_one()` 随机唤醒其中一个线程，`condition.notify_all()` 会唤醒所有的线程

## 前置知识

### std::future 和 std:promise


关于 `std::future`

`std::future` 是 C++11 标准库中引入的一种用于表示异步操作结果的机制。用于在请求处理结果的线程和执行任务的线程之间进行同步。`std::future` 提供了一个简洁的方式来获取异步计算的结果

使用 `std::future::get()` 可以用于获取结果，如果结果尚未准备好，则将线程阻塞，直到结果可用，该函数**只能被调用一次**，后续调用将会导致一场

使用 `std::future::valid()` 用于检查 `std::future` 对象是否有与之关联的共享状态

使用 `std::future_status` 用于查询任务的状态，比如是否超时等

```cpp
#include <iostream>
#include <future>
#include <chrono>

int compute() {
    std::this_thread::sleep_for(std::chrono::seconds(3));
    return 42;
}

int main() {
    // 使用 std::async 启动一个异步任务，返回 std::future
    std::future<int> result = std::async(std::launch::async, compute);

    // 模拟其他工作
    std::cout << "Doing other work...\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // 获取异步任务的结果
    std::cout << "Waiting for result...\n";
    int value = result.get(); // 阻塞直到 compute 完成并返回结果
    std::cout << "Result: " << value << "\n";
    
    return 0;
}
```

上述代码使用 `std::async` 创建一个异步任务，用 `std::future<int>` 表示结果是一个 int 类型的值

关于 `std::promise`

`std::promise` 是 C++11 标准引入的另一种同步机制，用于在线程之间传递数据。它与 `std::future` 密切相关，提供了一种方式来设置数据的结果，`std::future` 则用于获取这一结果

`std::promise` 用于在一个线程中设置某个异步操作的结果。你可以把它看作是 **承诺** 去提供一个结果，它保证返回值是一个它定义的类型

每个 `std::promise` 对象可以通过 `get_future()` 方法生成一个对应的 `std::future` 对象。这个 `std::future` 可以在另一个线程中使用，以获取 `std::promise` 所设置的结果

```cpp
#include <iostream>
#include <thread>
#include <future>

// 线程函数，执行一些计算并将结果设置到 promise 中
void calculate(std::promise<int>& p, int value) {
    try {
        if (value < 0) {
            throw std::invalid_argument("Negative value not allowed");
        }
        
        // 这里模拟一些计算
        int result = value * 2;
        std::this_thread::sleep_for(std::chrono::seconds(2));
        
        // 设置计算结果
        p.set_value(result);
    } catch (...) {
        // 如果发生异常，设置异常状态
        p.set_exception(std::current_exception());
    }
}

int main() {
    // 创建一个 promise 对象
    std::promise<int> p;
    
    // 获取与 promise 相关联的 future 对象
    std::future<int> f = p.get_future();
    
    // 启动线程，并将 promise 传递给线程函数
    std::thread t(calculate, std::ref(p), 10);
    
    try {
        // 从 future 获取计算结果，阻塞直到结果可用
        int result = f.get();
        std::cout << "Result from the thread: " << result << std::endl;
    } catch (const std::exception& e) {
        // 处理线程中抛出的异常
        std::cout << "Exception: " << e.what() << std::endl;
    }
    
    // 等待线程完成
    t.join();
    
    return 0;
}
```

### std::common_type_t

`std::common_type_t` 是 C++ 标准库中提供的一个类型特性，用于确定一组类型的通用类型。它会尝试通过类型转换规则找到一个能够容纳所有给定类型值的类型

- 如果两个类型都是相同的类型，`std::common_type_t` 就是这个类型本身
- 如果一个类型是 `int`，另一个是 `double`，`std::common_type_t` 会选择 `double` 作为通用类型，因为 `double` 可以表示所有 `int` 值而不会丢失精度

`std::is_signed_v` 和 `std::is_unsigned_v` 用于判断类型是否 有符合 和 无符号 的，如果是 无符号类型，使用 `std::is_unsigned_v` 返回 `true`，使用 `std::is_signed_v` 返回 `false`，反之亦然

`std::enable_if_t` 是一种用于 `SFINAE`（`Substitution Failure Is Not An Error`）的工具。它允许在模板参数中进行条件编译，如果条件为 `true`，则 `std::enable_if_t` 定义一个类型；如果条件为 `false`，则不定义类型，使模板特化失效

下面是 `BS_thread_pool` 对上述模板的统一使用

```cpp
// 通用模板
template <typename T1, typename T2, typename Enable = void>
struct common_index_type
{
    using type = std::common_type_t<T1, T2>;
};

// 两个有符号整数
template <typename T1, typename T2>
struct common_index_type<T1, T2, std::enable_if_t<std::is_signed_v<T1> && std::is_signed_v<T2>>>
{
    using type = std::conditional_t<(sizeof(T1) >= sizeof(T2)), T1, T2>;
};

// 两个无符号整数
template <typename T1, typename T2>
struct common_index_type<T1, T2, std::enable_if_t<std::is_unsigned_v<T1> && std::is_unsigned_v<T2>>>
{
    using type = std::conditional_t<(sizeof(T1) >= sizeof(T2)), T1, T2>;
};

// 一个有符号 一个无符号整数
template <typename T1, typename T2>
struct common_index_type<T1, T2, std::enable_if_t<(std::is_signed_v<T1> && std::is_unsigned_v<T2>) || (std::is_unsigned_v<T1> && std::is_signed_v<T2>)>>
{
    using S = std::conditional_t<std::is_signed_v<T1>, T1, T2>;
    using U = std::conditional_t<std::is_unsigned_v<T1>, T1, T2>;
    static constexpr std::size_t larger_size = (sizeof(S) > sizeof(U)) ? sizeof(S) : sizeof(U);
    using type = std::conditional_t<larger_size <= 4,
        std::conditional_t<larger_size == 1 || (sizeof(S) == 2 && sizeof(U) == 1), std::int16_t, std::conditional_t<larger_size == 2 || (sizeof(S) == 4 && sizeof(U) < 4), std::int32_t, std::int64_t>>,
        std::conditional_t<sizeof(U) == 8, std::uint64_t, std::int64_t>>;
};
```

使用模板特化的功能，分别定义四种不同的情况下使用的 `common_index_type`

- 对于两个类型都是 **无符号整数** 或者 **有符号整数** 的情况来说，选择 `sizeof(T)` 更大，可以保证不会被截断，进而保证正确性
- 对于一个类型是 **无符号整数** 另一个却是 **有符号整数** 的情况来说，通过比较尺寸，选择一个合适的类型。对于 32 位及以下的整数，使用 `int16_t`、`int32_t `或 `int64_t`；对于更大的整数，选择 `uint64_t` 或 `int64_t`

### std::enable_if_t 和 std::is_invocable_v

```cpp
#define BS_THREAD_POOL_IF_PAUSE_ENABLED template <bool P = pause_enabled, typename = std::enable_if_t<P>>
#define BS_THREAD_POOL_INIT_FUNC_CONCEPT(F) typename F, typename = std::enable_if_t<std::is_invocable_v<F> || std::is_invocable_v<F, std::size_t>> 

class thread_pool
{
    // ...
    BS_THREAD_POOL_IF_PAUSE_ENABLED
    [[nodiscard]] bool is_paused() const
    {
        const std::scoped_lock tasks_lock(tasks_mutex);
        return paused;
    }
    // ...
};
```

以上面这段代码为例，结合 `BS_THREAD_POOL_IF_PAUSE_ENABLED` 宏的使用

如果线程池没有启用了暂停功能，则 `pause_enable` = false，那么 P = false，那么 `std::enable_if_t<false>` 会通过 SFINAE 机制，自动忽略使用了该宏的特定成员函数或模板，也就是不会生成 `pause()` 和 `unpause()` 代码，减少了无意义的接口支持

`std::enable_if_t<P>` 是一个条件模板工具
- 如果 P 为 `true`，`std::enable_if_t<P>` 将生成一个有效的类型（通常为 `void`）
- 如果 P 为 `false`，`std::enable_if_t<P>` 会导致模板实例化失败

`SFINAE` 全称是 `Substitution Failure Is Not An Error`，替换失败不是错误

那么 `std::enable_if_t` 模板实例化失败，并不会报错，只是不生成，会忽略这个模板实例化



## BS_thread_pool

在该项目中，线程池的定义为 `thread_pool` 

```cpp
template <opt_t OptFlags = tp::none>
class [[nodiscard]] thread_pool
{
    // ...........
};
```

这里 `OptFlags` 用于定义线程池的作用

```cpp
using opt_t = std::uint8_t;

enum tp : opt_t
{
    none = 0,
    priority = 1 << 0,
    pause = 1 << 2,
    wait_deadlock_checks = 1 << 3
};
```

这些分别对应四种不同功能的线程池

```cpp
using light_thread_pool = thread_pool<tp::none>;
using priority_thread_pool = thread_pool<tp::priority>;
using pause_thread_pool = thread_pool<tp::pause>;
using wdc_thread_pool = thread_pool<tp::wait_deadlock_checks>;
```

在 `thread_pool` 模板类中，根据传入模板值进行参数的设置

```cpp
static constexpr bool priority_enabled = (OptFlags & tp::priority) != 0;
static constexpr bool pause_enabled = (OptFlags & tp::pause) != 0;
static constexpr bool wait_deadlock_checks_enabled = (OptFlags & tp::wait_deadlock_checks) != 0;
```

在定义 `light_thread_pool`、`priority_thread_pool` 等的时候就已经将其属性设置好，在后续类的使用中直接可以通过属性判断进行功能判断

```cpp
std::conditional_t<priority_enabled, std::priority_queue<pr_task>, std::queue<task_t>> tasks;
std::conditional_t<pause_enabled, bool, std::monostate> paused = {};
```

比如上述代码定义 `tasks` 任务列表，根据 `priority_enabled` 是否根据优先级排序来选择定义的 `tasks` 的队列类型是 `priority_queue` 还是 `queue`，这个类型在编译期就确定了

### submit_task 函数

上面代码介绍过 `std::future` 和 `std::promise` 的作用，接下来就是 `submit_task` 函数的源代码了

```cpp
template <typename F, typename R = std::invoke_result_t<std::decay_t<F>>>
[[nodiscard]] std::future<R> submit_task(F&& task, const priority_t priority = 0)
{
#ifdef __cpp_lib_move_only_function
    std::promise<R> promise;
#define BS_THREAD_POOL_PROMISE_MEMBER_ACCESS promise.
#else
    const std::shared_ptr<std::promise<R>> promise = std::make_shared<std::promise<R>>();
#define BS_THREAD_POOL_PROMISE_MEMBER_ACCESS promise->
#endif
    std::future<R> future = BS_THREAD_POOL_PROMISE_MEMBER_ACCESS get_future();
    detach_task(
        [task = std::forward<F>(task), promise = std::move(promise)]() mutable
        {
#ifdef __cpp_exceptions
            try
            {
#endif
                if constexpr (std::is_void_v<R>)
                {
                    task();
                    BS_THREAD_POOL_PROMISE_MEMBER_ACCESS set_value();
                }
                else
                {
                    BS_THREAD_POOL_PROMISE_MEMBER_ACCESS set_value(task());
                }
#ifdef __cpp_exceptions
            }
            catch (...)
            {
                try
                {
                    BS_THREAD_POOL_PROMISE_MEMBER_ACCESS set_exception(std::current_exception());
                }
                catch (...)
                {
                }
            }
#endif
        },
        priority);
    return future;
}
```

这个函数是一个模板函数, 用于将任务提交到线程池的任务队列中, 并返回一个 `std::future` 对象以获取任务的结果。它异步地执行一个函数,并在稍后获取结果或等待任务完成

该方法封装了一个 lambda 表达式，将传入的 `task` 封装了一层，创建了自己的 `task`，再将自己的 `task` 和 `priority` 作为参数传递给 `detach_task`

```cpp
template <typename F>
void detach_task(F&& task, const priority_t priority = 0)
{
    {
        const std::scoped_lock tasks_lock(tasks_mutex);
        if constexpr (priority_enabled)
            tasks.emplace(std::forward<F>(task), priority);
        else
            tasks.emplace(std::forward<F>(task));
    }
    task_available_cv.notify_one();
}
```


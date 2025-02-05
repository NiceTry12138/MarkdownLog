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

这些分别对应三种不同功能的线程池

```cpp
using light_thread_pool = thread_pool<tp::none>;
using priority_thread_pool = thread_pool<tp::priority>;
using pause_thread_pool = thread_pool<tp::pause>;
using wdc_thread_pool = thread_pool<tp::wait_deadlock_checks>;
```

在 `thread_pool` 类中，根据模板进行值的设置

```cpp
static constexpr bool priority_enabled = (OptFlags & tp::priority) != 0;
static constexpr bool pause_enabled = (OptFlags & tp::pause) != 0;
static constexpr bool wait_deadlock_checks_enabled = (OptFlags & tp::wait_deadlock_checks) != 0;
```


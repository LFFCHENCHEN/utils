#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <type_traits>
#include <vector>

namespace Tpool {

class ThreadPool {
public:
  enum Priority { High, Normal, Low };

  explicit ThreadPool(size_t threads, bool enable_affinity = false);

  template <class F, class... Args>
  auto enqueue(Priority priority, F &&f, Args &&...args)
      -> std::future<typename std::result_of<F(Args...)>::type>;

  void resize(size_t new_size);
  void shutdown() noexcept;

  ~ThreadPool() noexcept;

  ThreadPool(const ThreadPool &) = delete;
  ThreadPool &operator=(const ThreadPool &) = delete;

private:
  struct Task {
    std::function<void()> func;
    unsigned priority;

    bool operator<(const Task &other) const {
      return priority < other.priority;
    }
  };

  std::vector<std::thread> workers;
  std::priority_queue<Task> tasks;

  std::mutex queue_mutex;
  std::condition_variable condition;
  std::atomic<bool> stop;
  bool affinity_enabled;

  void worker_loop();
  void set_thread_affinity(std::thread::native_handle_type handle, int core_id);
};

ThreadPool::ThreadPool(size_t threads, bool enable_affinity)
    : stop(false), affinity_enabled(enable_affinity) {
  try {
    for (size_t i = 0; i < threads; ++i) {
      workers.emplace_back([this] { worker_loop(); });
      if (affinity_enabled) {
        set_thread_affinity(
            workers.back().native_handle(),
            static_cast<int>(i % std::thread::hardware_concurrency()));
      }
    }
  } catch (...) {
    stop = true;
    throw;
  }
}

template <class F, class... Args>
auto ThreadPool::enqueue(Priority priority, F &&f, Args &&...args)
    -> std::future<typename std::result_of<F(Args...)>::type> {

  typedef typename std::result_of<F(Args...)>::type return_type;
  typedef std::packaged_task<return_type()> task_type;

  auto task = std::make_shared<task_type>(
      std::bind(std::forward<F>(f), std::forward<Args>(args)...));

  auto res = task->get_future();
  unsigned prio_value = static_cast<unsigned>(priority);

  {
    std::unique_lock<std::mutex> lock(queue_mutex);
    if (stop.load())
      throw std::runtime_error("enqueue on stopped ThreadPool");

    tasks.push(Task{[task]() { (*task)(); }, prio_value});
  }

  condition.notify_one();
  return res;
}

void ThreadPool::resize(size_t new_size) {
  if (stop)
    return;

  std::unique_lock<std::mutex> lock(queue_mutex);
  const size_t old_size = workers.size();

  if (new_size > old_size) {
    workers.reserve(new_size);
    for (size_t i = old_size; i < new_size; ++i) {
      try {
        workers.emplace_back([this] { worker_loop(); });
        if (affinity_enabled) {
          set_thread_affinity(
              workers.back().native_handle(),
              static_cast<int>(i % std::thread::hardware_concurrency()));
        }
      } catch (...) {
        if (workers.size() < old_size)
          throw;
        stop = true;
        condition.notify_all();
        throw;
      }
    }
  } else if (new_size < old_size) {
    for (size_t i = new_size; i < old_size; ++i) {
      workers[i].detach();
    }
    workers.resize(new_size);
  }
}

void ThreadPool::worker_loop() {
  while (!stop) {
    Task task;
    {
      std::unique_lock<std::mutex> lock(queue_mutex);
      condition.wait(lock, [this] { return stop || !tasks.empty(); });

      if (stop && tasks.empty())
        return;

      task = tasks.top();
      tasks.pop();
    }

    try {
      task.func();
    } catch (...) {
      // 异常处理逻辑
    }
  }
}

void ThreadPool::shutdown() noexcept {
  stop = true;
  condition.notify_all();
  for (auto &worker : workers) {
    if (worker.joinable())
      worker.join();
  }
}

ThreadPool::~ThreadPool() noexcept { shutdown(); }

#ifdef __linux__
#include <pthread.h>
void ThreadPool::set_thread_affinity(std::thread::native_handle_type handle,
                                     int core_id) {
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  CPU_SET(core_id % CPU_SETSIZE, &cpuset);
  pthread_setaffinity_np(handle, sizeof(cpu_set_t), &cpuset);
}
#else
void ThreadPool::set_thread_affinity(...) {}
#endif

} // namespace TPOOL
#endif // THREAD_POOL_HPP

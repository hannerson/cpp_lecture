#include <atomic>
#include <functional>
#include <future>
#include <iostream>
#include <queue>
#include <vector>

class ThreadPool {
 public:
  typedef std::function<void()> TaskAsync;
  ThreadPool(size_t size) : size_(size) {}
  ~ThreadPool() {
    std::cout << " ~ThreadPool" << std::endl;
    stop();
  }

  void start() {
    if (!is_stop_.load()) {
      return;
    }
    is_stop_.store(false);
    for (size_t i = 0; i < size_; i++) {
      task_threads_.emplace_back(
          std::make_shared<std::thread>(std::bind(&ThreadPool::worker, this)));
    }
  }

  void stop() {
    is_stop_.store(true);
    {
      std::unique_lock<std::mutex> lock(task_mutex_);
      task_cv_.notify_all();
    }
    for (auto& t : task_threads_) {
      if (t->joinable()) {
        t->join();
      }
    }
  }

  template <typename Func, typename... Args>
  auto add_task(Func&& f, Args&&... args) -> std::future<decltype(f(args...))> {
    using return_type = decltype(f(args...));
    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<Func>(f), std::forward<Args>(args)...));
    std::future<return_type> result = task->get_future();

    // auto task_func = [task]() { (*task)(); };

    std::unique_lock<std::mutex> lock(task_mutex_);
    task_queue_.emplace([task]() { (*task)(); });
    std::cout << "add task" << std::endl;
    task_cv_.notify_one();
    return result;
  }

 private:
  void worker() {
    while (!is_stop_.load()) {
      TaskAsync task = nullptr;
      std::cout << "---start worker---" << std::endl;
      {
        std::unique_lock<std::mutex> lock(task_mutex_);
        task_cv_.wait(
            lock, [this]() { return is_stop_.load() || !task_queue_.empty(); });
        if (is_stop_.load() && task_queue_.empty()) {
          continue;
        }
        task = std::move(task_queue_.front());
        task_queue_.pop();
      }
      // task();
      if (task != nullptr) {
        task();
        std::cout << "---task run---" << std::endl;
      }
      std::cout << "---task end---" << std::endl;
    }
  }
  size_t size_;
  std::queue<TaskAsync> task_queue_;
  std::atomic_bool is_stop_ = {true};
  std::mutex task_mutex_;
  std::condition_variable task_cv_;
  std::vector<std::shared_ptr<std::thread>> task_threads_;
};

int add_func(int a, int b) {
  std::cout << "add func" << std::endl;
  return a + b;
}

class Test {
 public:
  Test(int a, int b) : a_(a), b_(b) {}
  int add() { return a_ + b_; }

 private:
  int a_;
  int b_;
};

int main() {
  auto f1 = [](int a, int b) { return a + b; };
  Test t(2, 3);

  auto pool = std::make_shared<ThreadPool>(10);
  pool->start();
  std::vector<std::future<int>> results;
  for (int i = 0; i < 100; i++) {
    results.emplace_back(pool->add_task(f1, i, i + 1));
    results.emplace_back(pool->add_task(std::bind(&Test::add, t)));
  }

  std::invoke(&Test::add, t);

  for (auto&& r : results) {
    std::cout << r.get() << std::endl;
  }
}
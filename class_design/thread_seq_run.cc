#include <atomic>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

class ThreadSeqRun {
 public:
  ThreadSeqRun(size_t size) : size_(size) {}
  ~ThreadSeqRun() { stop(); }
  void start() {
    if (!is_stop_) {
      return;
    }
    is_stop_ = false;
    for (size_t i = 0; i < size_; i++) {
      workers_.emplace_back(std::make_shared<std::thread>(
          std::bind(&ThreadSeqRun::worker, this, i)));
    }
  }
  void stop() {
    is_stop_ = true;
    {
      std::unique_lock<std::mutex> lock(mutex_);
      cv_.notify_all();
    }
    for (auto& t : workers_) {
      if (t->joinable()) {
        t->join();
      }
    }
  }

  void invoke(int i) {
    std::unique_lock<std::mutex> lock(mutex_);
    currend_ = i % size_;
    cv_.notify_one();
  }

 private:
  void worker(int i) {
    std::cout << "start worker: " << i << std::endl;
    while (!is_stop_) {
      std::unique_lock<std::mutex> lock(mutex_);
      cv_.wait(lock, [i, this]() { return is_stop_.load() || i == currend_; });
      if (is_stop_.load()) {
        return;
      }
      std::cout << "worker: " << i << std::endl;
      currend_ = (currend_ + 1) % size_;
      std::cout << "current: " << currend_ << std::endl;
      cv_.notify_all();
    }
  }
  std::mutex mutex_;
  std::condition_variable cv_;
  int currend_ = 0;
  size_t size_;
  std::atomic_bool is_stop_ = {true};
  std::vector<std::shared_ptr<std::thread>> workers_;
};

int main() {
  auto t = std::make_shared<ThreadSeqRun>(10);
  t->start();

  std::this_thread::sleep_for(std::chrono::milliseconds(3000));
  //   for (int i = 0; i < 10; i++) {
  //     t->invoke(i);
  //   }
}

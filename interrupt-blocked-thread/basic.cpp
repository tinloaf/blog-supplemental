#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

class Thread {
public:
  void run() { m_thread = std::thread(&Thread::threadMain, this); }
  void stop() {
    m_stop = true;
    m_thread.join();
  }

private:
  void threadMain() {
    while (!m_stop) {
      std::this_thread::sleep_for(std::chrono::seconds{1});
      std::cout << "Thread idling…\n";
    }
    std::cout << "Thread stopped cleanly.\n";
  }
  std::thread m_thread;
  std::atomic<bool> m_stop = false;
};

int main(int argc, char **argv) {
  Thread t;
  t.run();
  std::this_thread::sleep_for(std::chrono::seconds{5});
  t.stop();
  return 0;
}

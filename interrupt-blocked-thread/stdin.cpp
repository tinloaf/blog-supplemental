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
      char buffer[10];
      read(0, buffer, 9); // fd 0 is stdin
      buffer[9] = '\0';
      std::cout << "Read from stdin: " << buffer << "\n";
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
  std::cout << "Requesting thread stop\n";
  t.stop();
  return 0;
}

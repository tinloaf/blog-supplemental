#include <chrono>
#include <iostream>
#include <thread>

class Thread {
public:
  void run() {
    m_thread = std::jthread(
        [this](std::stop_token token) { this->threadMain(token); });
  }

private:
  void threadMain(std::stop_token token) {
    while (!token.stop_requested()) {
      std::this_thread::sleep_for(std::chrono::seconds{1});
      std::cout << "Thread idling…\n";
    }
    std::cout << "Thread stopped cleanly.\n";
  }
  std::jthread m_thread;
};

int main(int argc, char **argv) {
  Thread t;
  t.run();
  std::this_thread::sleep_for(std::chrono::seconds{5});
  return 0;
}

#include <atomic>
#include <chrono>
#include <csignal>
#include <iostream>
#include <signal.h>
#include <thread>

void myHandler(int) {}

class Thread {
public:
  void run() { m_thread = std::thread(&Thread::threadMain, this); }
  void stop() {
    m_stop = true;
    pthread_kill(m_thread.native_handle(), SIGUSR1);
    m_thread.join();
  }

private:
  void threadMain() {
    while (!m_stop) {
      std::this_thread::sleep_for(std::chrono::seconds{1});
      char buffer[10];
      int result = read(0, buffer, 9); // fd 0 is stdin
      if (result >= 0) {
        buffer[9] = '\0';
        std::cout << "Read from stdin: " << buffer << "\n";
      } else {
        std::cout << "read() returned error " << errno << "\n";
      }
    }
    std::cout << "Thread stopped cleanly.\n";
  }
  std::thread m_thread;
  std::atomic<bool> m_stop = false;
};

int main(int argc, char **argv) {
  // Set up interrupting threads
  struct sigaction sigActionData;
  sigemptyset(&sigActionData.sa_mask);
  sigActionData.sa_flags = SA_INTERRUPT;
  sigActionData.sa_handler = &myHandler;
  sigaction(SIGUSR1, &sigActionData, nullptr);

  Thread t;
  t.run();
  std::this_thread::sleep_for(std::chrono::seconds{5});
  std::cout << "Requesting thread stop\n";
  t.stop();
  return 0;
}

#include <atomic>
#include <chrono>
#include <csignal>
#include <iostream>
#include <signal.h>
#include <thread>

void myHandler(int) {}

class Thread {
public:
  void run() {
    m_thread = std::jthread(
        [this](std::stop_token token) { this->threadMain(token); });
  }

private:
  void threadMain(std::stop_token token) {
    // Register a stop callback that will send us the SIGUSR1 signal
    std::stop_callback callback(token, [this] {
      pthread_kill(m_thread.native_handle(), SIGUSR1);
    });

    while (!token.stop_requested()) {
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
  std::jthread m_thread;
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
  std::cout << "Sleeping in main()\n";
  std::this_thread::sleep_for(std::chrono::seconds{5});
  std::cout << "Sleep in main() done, ending the program.\n";
  return 0;
}

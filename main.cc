#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include <iostream>

timeval start;
size_t x = 0;

void OnExit(int unused_signum) {
  timeval end;
  if (gettimeofday(&end, nullptr)) {
    perror("gettimeofday()");
    exit(EXIT_FAILURE);
  }
  size_t micros =
      1000000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec);

  std::cout << micros << " microseconds, " << x << " nanosleeps, "
            << (micros / x) << " microseconds per nanosleep\n";
  exit(EXIT_SUCCESS);
}

int main(int argc, char **argv) {
  timespec one_ns{0, 1};
  pid_t pid = fork();
  if (gettimeofday(&start, nullptr)) {
    perror("gettimeofday()");
    return 1;
  }
  if (pid == -1) {
    perror("fork()");
    return 1;
  } else if (pid == 0) {
    signal(SIGTERM, OnExit);
    for (;;) {
      nanosleep(&one_ns, nullptr);
      x++;
    }
  } else {
    sleep(5);
    kill(pid, SIGTERM);
  }
  return 0;
}

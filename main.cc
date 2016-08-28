#include <signal.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include <iostream>

timeval start;
size_t x = 0;
const char *vname = NULL;

void OnExit(int unused_signum) {
  timeval end;
  if (gettimeofday(&end, nullptr)) {
    perror("gettimeofday()");
    exit(EXIT_FAILURE);
  }
  size_t micros =
      1000000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec);

  std::cout << micros << " microseconds, " << x << " " << vname << "s, "
            << (micros / x) << " microseconds per " << vname << std::endl;
  exit(EXIT_SUCCESS);
}

#define NS_PER_USEC 1000
#define USEC_PER_MS 1000

int go(timespec *ts, const char *name) {
  vname = name;
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
      nanosleep(ts, nullptr);
      x++;
    }
  } else {
    sleep(5);
    kill(pid, SIGTERM);
    waitpid(pid, NULL, 0);
  }
  return 0;
}

int main(int argc, char **argv) {
  timespec one_ns = {0, 1};
  timespec one_ms = {0, 1 * NS_PER_USEC * USEC_PER_MS};
  timespec two_ms = {0, 2 * NS_PER_USEC * USEC_PER_MS};

  if (0 != go(&one_ns, "nanosleep")) {
    return -1;
  }
  if (0 != go(&one_ms, "millissleep")) {
    return -1;
  }
  if (0 != go(&two_ms, "two millissleep")) {
    return -1;
  }
  return 0;
}

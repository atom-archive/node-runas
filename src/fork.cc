#include "fork.h"

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

namespace runas {

namespace {

void child(int* pfds,
           const std::string& command,
           const std::vector<std::string>& args) {
  // Redirect stdin to the pipe.
  close(pfds[1]);
  dup2(pfds[0], 0);
  close(pfds[0]);

  std::vector<char*> argv(StringVectorToCharStarVector(args));
  argv.insert(argv.begin(), const_cast<char*>(command.c_str()));

  execvp(command.c_str(), &argv[0]);
  perror("execvp()");
  exit(127);
}

int parent(int pid, int* pfds, const std::string& std_input) {
  // Write string to child's stdin.
  int want = std_input.size();
  if (want > 0) {
    const char* p = std_input.data();
    close(pfds[0]);
    while (want > 0) {
      int r = write(pfds[1], p, want);
      if (r > 0) {
        want -= r;
        p += r;
      } else if (errno != EAGAIN && errno != EINTR) {
        break;
      }
    }
    close(pfds[1]);
  }

  // Wait for child.
  int r, status;
  do {
    r = waitpid(pid, &status, 0);
  } while (r == -1 && errno == EINTR);

  if (r == -1 || !WIFEXITED(status))
    return -1;

  return WEXITSTATUS(status);
}

}  // namespace


std::vector<char*> StringVectorToCharStarVector(
    const std::vector<std::string>& args) {
  std::vector<char*> argv(1 + args.size(), NULL);
  for (size_t i = 0; i < args.size(); ++i)
    argv[i] = const_cast<char*>(args[i].c_str());
  return argv;
}

bool Fork(const std::string& command,
          const std::vector<std::string>& args,
          const std::string& std_input,
          int options,
          int* exit_code) {
  int pfds[2];
  if (pipe(pfds) == -1)
    return false;

  // execvp
  int pid = fork();
  switch (pid) {
    case 0:  // child
      child(pfds, command, args);
      break;

    case -1:  // error
      return false;

    default:  // parent
      *exit_code = parent(pid, pfds, std_input);
      return true;
  };

  return false;
}

}  // namespace runas

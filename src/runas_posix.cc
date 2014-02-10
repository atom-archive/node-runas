#include "runas.h"

#include <errno.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

namespace runas {

namespace {

void child(const std::string& command, const std::vector<std::string>& args) {
  // Convert std::string array to char* array.
  std::vector<char*> argv(2 + args.size(), NULL);
  argv[0] = const_cast<char*>(command.c_str());
  for (size_t i = 0; i < args.size(); ++i)
    argv[i + 1] = const_cast<char*>(args[i].c_str());

  execvp(command.c_str(), &argv[0]);
  perror("execvp()");
  exit(127);
}

int parent(int pid) {
  int r, status;
  do {
    r = waitpid(pid, &status, WNOHANG);
  } while (r != -1);

  int exit_code = 0;
  if (WIFEXITED(status))
    exit_code = WEXITSTATUS(status);
  return exit_code;
}

}  // namespace

bool Runas(const std::string& command,
           std::vector<std::string>& args,
           int options,
           int* exit_code) {
  // execvp
  int pid = fork();
  switch (pid) {
    case 0:  // child
      child(command, args);
      break;

    case -1:  // error
      return false;

    default:  // parent
      *exit_code = parent(pid);
      return true;
  };

  return false;
}

}  // namespace runas

#include "spawn_as_admin.h"

#include <errno.h>
#include <fcntl.h>
#include <Security/Authorization.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>

namespace spawn_as_admin {

using std::vector;
using std::string;

namespace {

AuthorizationRef authoriziation_ref = nullptr;
ChildProcess FAILURE = {nullptr, -1, -1, -1};

}


ChildProcess StartChildProcessWithoutPrivileges(const string &command, vector<char *> &argv) {
  int stdin_fds[2], stdout_fds[2];
  if (pipe(stdin_fds) == -1) return FAILURE;
  if (pipe(stdout_fds) == -1) return FAILURE;

  int pid = fork();
  switch (pid) {
    case -1:
      return FAILURE;

    case 0:
      close(stdin_fds[1]);
      dup2(stdin_fds[0], 0);
      close(stdin_fds[0]);

      close(stdout_fds[0]);
      dup2(stdout_fds[1], 1);
      close(stdout_fds[1]);

      argv.insert(argv.begin(), const_cast<char*>(command.c_str()));

      execvp(command.c_str(), argv.data());
      perror("execvp()");
      exit(127);

    default:
      close(stdin_fds[0]);
      return {nullptr, pid, stdin_fds[1], stdout_fds[0]};
  }
}

ChildProcess StartChildProcess(const string& command, const vector<string>& args, bool test_mode) {
  vector<char *> argv;
  argv.reserve(args.size());
  for (const string &arg : args) {
    argv.push_back(const_cast<char *>(arg.c_str()));
  }
  argv.push_back(nullptr);

  if (test_mode) return StartChildProcessWithoutPrivileges(command, argv);

  if (!authoriziation_ref && AuthorizationCreate(NULL,
                                                 kAuthorizationEmptyEnvironment,
                                                 kAuthorizationFlagDefaults,
                                                 &authoriziation_ref) != errAuthorizationSuccess) {
    return FAILURE;
  }

  FILE *pipe;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
  OSStatus status = AuthorizationExecuteWithPrivileges(
    authoriziation_ref,
    command.c_str(),
    kAuthorizationFlagDefaults,
    &argv[0],
    &pipe
  );
#pragma clang diagnostic pop

  if (status != errAuthorizationSuccess) {
    return FAILURE;
  }

  int pipe_descriptor = fileno(pipe);
  int pid = fcntl(pipe_descriptor, F_GETOWN, 0);
  return {pipe, pid, pipe_descriptor, pipe_descriptor};
}

int WaitForChildProcessToExit(ChildProcess *child_process, bool test_mode) {
  int r, status;
  do {
    r = waitpid(child_process->pid, &status, 0);
  } while (r == -1 && errno == EINTR);

  if (test_mode) {
    close(child_process->stdin_file_descriptor);
    close(child_process->stdout_file_descriptor);
  } else {
    FILE *pipe = static_cast<FILE *>(child_process->payload);
    fclose(pipe);
  }

  if (r == -1 || !WIFEXITED(status))
    return false;

  return WEXITSTATUS(status);
}

}  // namespace spawn_as_admin

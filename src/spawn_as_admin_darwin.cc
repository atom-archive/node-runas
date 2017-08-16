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

AuthorizationRef g_auth = NULL;

OSStatus ExecuteWithPrivileges(AuthorizationRef authorization,
                               const char *command,
                               AuthorizationFlags options,
                               char* const* arguments,
                               FILE** pipe) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
  return AuthorizationExecuteWithPrivileges(authorization,
                                            command,
                                            options,
                                            arguments,
                                            pipe);
#pragma clang diagnostic pop
}

}  // namespace


static Session FAILURE = {nullptr, -1, -1, -1};

Session SpawnAsCurrentUser(const string &command, vector<char *> &argv) {
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
      argv.push_back(nullptr);

      execvp(command.c_str(), &argv[0]);
      perror("execvp()");
      exit(127);

    default:
      close(stdin_fds[0]);
      return {nullptr, pid, stdin_fds[1], stdout_fds[0]};
  }
}

Session StartSpawnAsAdmin(const string& command, const vector<string>& args, bool admin) {
  vector<char *> argv(args.size());
  for (const string &arg : args) {
    argv.push_back(const_cast<char *>(arg.c_str()));
  }

  if (!admin) return SpawnAsCurrentUser(command, argv);

  if (!g_auth && AuthorizationCreate(NULL,
                                     kAuthorizationEmptyEnvironment,
                                     kAuthorizationFlagDefaults,
                                     &g_auth) != errAuthorizationSuccess)
    return FAILURE;

  FILE *pipe;
  if (ExecuteWithPrivileges(g_auth,
                            command.c_str(),
                            kAuthorizationFlagDefaults,
                            &argv[0],
                            &pipe) != errAuthorizationSuccess)
    return FAILURE;

  int pipe_descriptor = fileno(pipe);
  int pid = fcntl(pipe_descriptor, F_GETOWN, 0);
  return {pipe, pid, pipe_descriptor, pipe_descriptor};
}

int FinishSpawnAsAdmin(Session *session) {
  int r, status;
  do {
    r = waitpid(session->pid, &status, 0);
  } while (r == -1 && errno == EINTR);

  FILE *pipe = static_cast<FILE *>(session->payload);
  fclose(pipe);

  if (r == -1 || !WIFEXITED(status))
    return false;

  return WEXITSTATUS(status);
}

}  // namespace runas

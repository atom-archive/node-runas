#include "runas.h"

#include <Security/Authorization.h>
#include <sys/wait.h>

namespace runas {

namespace {

AuthorizationRef g_auth = NULL;

OSStatus ExecuteWithPrivileges(AuthorizationRef authorization,
                               const std::string& command,
                               AuthorizationFlags options,
                               char* const* arguments,
                               FILE** pipe) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
  return AuthorizationExecuteWithPrivileges(authorization,
                                            command.c_str(),
                                            options,
                                            arguments,
                                            pipe);
#pragma clang diagnostic pop
}

}  // namespace

bool Runas(const std::string& command,
           const std::vector<std::string>& args,
           const std::string& std_input,
           int options,
           int* exit_code) {
  if (!g_auth && AuthorizationCreate(NULL,
                                     kAuthorizationEmptyEnvironment,
                                     kAuthorizationFlagDefaults,
                                     &g_auth) != errAuthorizationSuccess)
    return false;

  // Convert std::string array to char* array.
  std::vector<char*> argv(2 + args.size(), NULL);
  argv[0] = const_cast<char*>(command.c_str());
  for (size_t i = 0; i < args.size(); ++i)
    argv[i + 1] = const_cast<char*>(args[i].c_str());

  FILE* pipe;
  if (ExecuteWithPrivileges(g_auth,
                            command,
                            kAuthorizationFlagDefaults,
                            &argv[0],
                            &pipe) != errAuthorizationSuccess)
    return false;

  // Write to stdin.
  const char*p = &std_input[0];
  size_t want = std_input.size();
  while (true) {
    size_t r = fwrite(p, sizeof(*p), want, pipe);
    if (r == 0)
      break;
    want -= r;
    p += r;
  }
  fclose(pipe);

  int status;
  int pid = wait(&status);
  if (pid == -1 || !WIFEXITED(status))
    return false;

  *exit_code = WEXITSTATUS(status);
  return true;
}

}  // namespace runas

#include "runas.h"

#include <Security/Authorization.h>
#include <sys/wait.h>

#include "fork.h"

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
  // Use fork when "admin" is false.
  if (!(options & OPTION_ADMIN))
    return Fork(command, args, std_input, options, exit_code);

  if (!g_auth && AuthorizationCreate(NULL,
                                     kAuthorizationEmptyEnvironment,
                                     kAuthorizationFlagDefaults,
                                     &g_auth) != errAuthorizationSuccess)
    return false;


  FILE* pipe;
  std::vector<char*> argv(StringVectorToCharStarVector(args));
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

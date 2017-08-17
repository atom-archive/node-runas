#ifndef SRC_SPAWN_AS_ADMIN_H_
#define SRC_SPAWN_AS_ADMIN_H_

#include <string>
#include <vector>

namespace spawn_as_admin {

struct ChildProcess {
  void *payload;
  int pid;
  int stdin_file_descriptor;
  int stdout_file_descriptor;
};

ChildProcess StartChildProcess(const std::string &command, const std::vector<std::string> &args, bool test_mode);

int WaitForChildProcessToExit(ChildProcess *, bool test_mode);

}  // namespace spawn_as_admin

#endif  // SRC_SPAWN_AS_ADMIN_H_

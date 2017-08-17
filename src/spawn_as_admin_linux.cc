#include "spawn_as_admin.h"

namespace spawn_as_admin {

ChildProcess StartChildProcess(const std::string& command, const std::vector<std::string>& args, bool test_mode) {
  return {nullptr, -1, -1, -1};
}

int WaitForChildProcessToExit(ChildProcess *child_process, bool test_mode) {
  return -1;
}

}  // namespace spawn_as_admin

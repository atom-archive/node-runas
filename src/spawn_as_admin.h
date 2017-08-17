#ifndef SRC_RUNAS_H_
#define SRC_RUNAS_H_

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

}  // namespace runas

#endif  // SRC_RUNAS_H_

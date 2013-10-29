#ifndef SRC_RUNAS_H_
#define SRC_RUNAS_H_

#include <string>
#include <vector>

namespace runas {

enum Options {
  OPTION_NONE = 0,
  OPTION_HIDE = 1,  // Hide the command's window, which is shown by default.
};

bool Runas(const std::string& command,
           std::vector<std::string>& args,
           int options,
           int* exit_code);

}  // namespace runas

#endif  // SRC_RUNAS_H_

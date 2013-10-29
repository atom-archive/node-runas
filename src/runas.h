#ifndef SRC_RUNAS_H_
#define SRC_RUNAS_H_

#include <string>
#include <vector>

namespace runas {

bool Runas(const std::string& command,
           std::vector<std::string>& args,
           int* exit_code);

}  // namespace runas

#endif  // SRC_RUNAS_H_

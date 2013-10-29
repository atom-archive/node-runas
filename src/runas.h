#ifndef SRC_RUNAS_H_
#define SRC_RUNAS_H_

#include <string>
#include <vector>

namespace runas {

int Runas(const std::string& command, std::vector<std::string>& args);

}  // namespace runas

#endif  // SRC_RUNAS_H_

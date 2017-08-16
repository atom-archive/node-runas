#include "spawn_as_admin.h"

namespace spawn_as_admin {

Session StartSpawnAsAdmin(const std::string& command, const std::vector<std::string>& args) {
  return {nullptr, -1, -1, -1};
}

int FinishSpawnAsAdmin(Session *session) {
  return -1;
}

}  // namespace runas

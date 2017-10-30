#ifndef DOCKER_CLIENT_PP_EXECRET_H
#define DOCKER_CLIENT_PP_EXECRET_H

#include "defines.hpp"

namespace DockerClientpp {

struct ExecRet {
  int ret_code;
  string output;
};

}  // namespace  DockerClientpp

#endif /* DOCKER_CLIENT_PP_EXECRET_H */

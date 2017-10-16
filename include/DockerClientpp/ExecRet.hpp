#ifndef EXECRET_H
#define EXECRET_H

#include "defines.hpp"

namespace DockerClientpp {

struct ExecRet {
  int ret_code;
  string output;
};

}  // namespace  DockerClientpp

#endif /* EXECRET_H */

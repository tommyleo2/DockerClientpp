#ifndef DEFINES_H
#define DEFINES_H

#include "OptionSetter.hpp"

namespace DockerClientpp {
#define FWD(x) std::forward<decltype(x)>(x)
  namespace __private {
    class DockerClientImplBase {
    public:
      virtual void createContainer(OptionSetter &option) = 0;
    };
  }
}

#endif /* DEFINES_H */

#ifndef OPTIONSETTER_H
#define OPTIONSETTER_H

#include <string>

#include "json.hpp"
#include "Options.hpp"

namespace DockerClientpp {

  class OptionSetter {
  public:
    OptionSetter() { }

    template<typename T>
    void set(DataOption<T> &&new_option) {
      data[new_option.getName()] = new_option.data;
    }

    std::string dump() {
      return data.dump();
    }
    nlohmann::json data;
  };

}
#endif /* OPTIONSETTER_H */

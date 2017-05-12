#ifndef OPTIONSETTER_H
#define OPTIONSETTER_H

#include <string>

#include "json.hpp"
#include "Options.hpp"

namespace DockerClientpp {
  /**
   *  @brief Docker http request post data setter
   *
   *  This class is used to build json options that will be sent to docker daemon
   */
  class OptionSetter {
  public:
    OptionSetter() { }

    /**
     *  @brief Set a new option
     *
     *  The setter will not guarantee the option type is matched with allowed type.
     *  It is the caller that does this.
     *
     *  @sa DockerClient
     */
    template<typename T>
    void set(DataOption<T> &&new_option) {
      data[new_option.getName()] = new_option.data;
    }

    /**
     *  @brief Dump json to string
     *
     *  @return Dumped json string
     */
    std::string dump() {
      return data.dump();
    }
    nlohmann::json data;
  };

}
#endif /* OPTIONSETTER_H */

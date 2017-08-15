#ifndef OPTIONS_H
#define OPTIONS_H

#include <string>
#include <vector>

namespace DockerClientpp {
class Option {
 public:
  virtual std::string getName() = 0;
};

template <typename T>
class DataOption : virtual public Option {
 public:
  explicit DataOption(const T& data) {
    this->data = data;
  }
  T data;
};

typedef DataOption<int> IntOption;
typedef DataOption<bool> BoolOption;
typedef DataOption<std::string> StringOption;
typedef DataOption<std::vector<std::string>> StringArrayOption;

template <typename T>
using ArrayOption = DataOption<std::vector<DataOption<T>>>;
}

#endif /* OPTIONS_H */

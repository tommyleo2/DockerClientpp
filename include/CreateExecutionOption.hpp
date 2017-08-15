#ifndef CREATEEXECUTIONOPTION_H
#define CREATEEXECUTIONOPTION_H

#include "Options.hpp"

namespace DockerClientpp {
using std::string;
template <typename T>
class CreateExecutionOption : public DataOption<T> {
 public:
  CreateExecutionOption(const T &data) : DataOption<T>(data) {}
};

namespace CreateExecution {
class AttachStdout : public CreateExecutionOption<bool> {
 public:
  AttachStdout(bool data) : CreateExecutionOption(data) {}
  virtual string getName() {
    return "AttachStdout";
  }
};
class AttachStderr : public CreateExecutionOption<bool> {
 public:
  AttachStderr(bool data) : CreateExecutionOption(data) {}
  virtual string getName() {
    return "AttachStderr";
  }
};
class Tty : public CreateExecutionOption<bool> {
 public:
  Tty(bool data) : CreateExecutionOption(data) {}
  virtual string getName() {
    return "Tty";
  }
};
class Cmd : public CreateExecutionOption<std::vector<string>> {
 public:
  Cmd(const std::vector<string> &data) : CreateExecutionOption(data) {}
  virtual string getName() {
    return "Cmd";
  }
};
class Privilieged : public CreateExecutionOption<bool> {
 public:
  Privilieged(const bool data) : CreateExecutionOption(data) {}
  virtual string getName() {
    return "Privilieged";
  }
};
class User : public CreateExecutionOption<string> {
 public:
  User(const string &data) : CreateExecutionOption(data) {}
  virtual string getName() {
    return "User";
  }
};
}
}

#endif /* CREATEEXECUTIONOPTION_H */

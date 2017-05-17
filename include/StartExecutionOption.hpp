#ifndef STARTEXECUTIONOPTION_H
#define STARTEXECUTIONOPTION_H

#include "Options.hpp"

namespace DockerClientpp {
  using std::string;
  template<typename T>
  class StartExecutionOption : public DataOption<T> {
  public:
    StartExecutionOption(const T &data) : DataOption<T>(data) { }
  };

  namespace StartExecution {
    class Detach : public StartExecutionOption<bool> {
    public:
      Detach(bool data) : StartExecutionOption(data) { }
      virtual string getName() override {
        return "Detach";
      }
    };

    class Tty : public StartExecutionOption<bool> {
    public:
      Tty(bool data) : StartExecutionOption(data) { }
      virtual string getName() override {
        return "Tty";
      }
    };
  }
}

#endif /* STARTEXECUTIONOPTION_H */

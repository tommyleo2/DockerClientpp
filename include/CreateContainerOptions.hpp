#ifndef CREATECONTAINEROPTIONS_H
#define CREATECONTAINEROPTIONS_H

#include "Options.hpp"

namespace DockerClientpp {
  using std::string;
  template<typename T>
  class CreateContainerOption : public DataOption<T> {
  public:
    CreateContainerOption(const T &data) : DataOption<T>(data) { }
  };

  namespace CreateContainer {

    //  TODO:
    //class AttachStdin : public CreateContainerOption {};
    //class Volumes

    class AttachStdout : public CreateContainerOption<bool> {
    public:
      AttachStdout(bool data) : CreateContainerOption(data) { }
      virtual string getName() {
        return "AttachStdout";
      }
    };
    class AttachStderr : public CreateContainerOption<bool> {
    public:
      AttachStderr(bool data) : CreateContainerOption(data) { }
      virtual string getName() {
        return "AttachStderr";
      }
    };
    class Tty : public CreateContainerOption<bool> {
    public:
      Tty(bool data) : CreateContainerOption(data) { }
      virtual string getName() {
        return "Tty";
      }
    };
    class Cmd : public CreateContainerOption<std::vector<string>> {
    public:
      Cmd(const std::vector<string> &data) : CreateContainerOption(data) { }
      virtual string getName() {
        return "Cmd";
      }
    };
    class WorkingDir : public CreateContainerOption<string> {
    public:
      WorkingDir(const string &data) : CreateContainerOption(data) { }
      virtual string getName() {
        return "WorkingDir";
      }
    };
    class Image : virtual public CreateContainerOption<string> {
    public:
      Image(const string &data) : CreateContainerOption(data) { }
      virtual string getName() {
        return "Image";
      }
    };
    class StopSignal : virtual public CreateContainerOption<string> {
    public:
      StopSignal(const string &data) : CreateContainerOption(data) { }
      virtual string getName() {
        return "StopSignal";
      }
    };
    class Name : virtual public CreateContainerOption<string> {
    public:
      Name(const string &data) : CreateContainerOption(data) { }
      virtual string getName() {
        return "name";
      }
    };
  }
}

#endif /* CREATECONTAINEROPTIONS_H */

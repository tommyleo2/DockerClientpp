#ifndef CREATECONTAINEROPTIONS_H
#define CREATECONTAINEROPTIONS_H

#include "Options.hpp"

namespace DockerClientpp {
  using std::string;
  class CreateContainerOption : virtual Option { };

  namespace CreateContainer {

    //  TODO:
    //class AttachStdin : public BoolOption {};
    //class Volumes

    class AttachStdout : virtual CreateContainerOption, virtual public BoolOption {
    public:
      AttachStdout(bool data) : BoolOption(data) { }
      virtual string getName() {
        return "AttachStdout";
      }
    };
    class AttachStderr : virtual CreateContainerOption, virtual public BoolOption {
    public:
      AttachStderr(bool data) : BoolOption(data) { }
      virtual string getName() {
        return "AttachStderr";
      }
    };
    class Tty : virtual CreateContainerOption, public BoolOption {
    public:
      Tty(bool data) : BoolOption(data) { }
      virtual string getName() {
        return "Tty";
      }
    };
    class Cmd : virtual CreateContainerOption, public StringArrayOption {
    public:
      Cmd(const std::vector<string> &data) : StringArrayOption(data) { }
      virtual string getName() {
        return "Cmd";
      }
    };
    class WorkingDir : virtual CreateContainerOption, public StringOption {
    public:
      WorkingDir(const string &data) : StringOption(data) { }
      virtual string getName() {
        return "WorkingDir";
      }
    };
    class Image : virtual CreateContainerOption, public StringOption {
    public:
      Image(const string &data) : StringOption(data) { }
      virtual string getName() {
        return "Image";
      }
    };
    class Name : virtual CreateContainerOption, public StringOption {
    public:
      Name(const string &data) : StringOption(data) { }
      virtual string getName() {
        return "name";
      }
    };
  }
}

#endif /* CREATECONTAINEROPTIONS_H */

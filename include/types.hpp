#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <sstream>
#include <map>
#include <streambuf>

namespace DockerClientpp {
  /**
   *  @brief Type of socket that docker daemon use.
   */
  enum SOCK_TYPE {
    UNIX,
    TCP
  };

  namespace Http {
    typedef std::string Uri;
    typedef std::map<std::string, std::string> Header;
    typedef std::map<std::string, std::string> QueryParam;
    class Response {
    public:
      Uri uri;
      int status_code;
      Header header;
    private:
      std::istream data;
    };
  }
}

#endif /* TYPES_H */

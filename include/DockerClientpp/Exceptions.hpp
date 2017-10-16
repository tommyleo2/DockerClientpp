#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>
#include <string>

using std::string;

namespace DockerClientpp {
/**
 * @brief Base class for all DockerClientpp exceptions
 */
class Exception : public std::runtime_error {
 public:
  explicit Exception(const string &what = "") : runtime_error(what) {}
  Exception(const Exception &e) = default;
};

class DockerOperationError : public Exception {
 public:
  DockerOperationError(const string &uri, int status_code, const string &what)
      : Exception(what), uri(uri), status_code(status_code) {}
  string uri;
  int status_code;
};

class ServerError : public Exception {
 public:
  explicit ServerError(const string &what)
      : Exception(what), status_code(500) {}
  const int status_code;
};

class SocketError : public Exception {
 public:
  explicit SocketError(const string &what) : Exception(what) {}
};

class SocketEOFError : public SocketError {
 public:
  SocketEOFError(int read) : SocketError("EOF"), read(read) {}
  int readSize() {
    return read;
  }

 private:
  int read;
};

class NotImplementError : public Exception {
 public:
  explicit NotImplementError(const string &what) : Exception(what) {}
};

class ParseError : public Exception {
 public:
  explicit ParseError(const string &what) : Exception(what) {}
};
}

#endif /* EXCEPTIONS_H */

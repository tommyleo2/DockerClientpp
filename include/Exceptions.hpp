#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <string>

using std::string;

namespace DockerClientpp {
  /**
   *  @brief Base class for all DockerClientpp exceptions
   */
  class Exception : public std::exception {
  public:
    explicit Exception(const string &what) : m_what(what) { }
    explicit Exception(const char *what = "") : m_what(what) { }
    Exception(const Exception &e) : std::exception(e), m_what(e.m_what) { }
    virtual const char *what() const throw() override {
      return m_what.c_str();
    }
  protected:
    string m_what;
  };

  class ServerError : public Exception {
  public:
    explicit ServerError(const string &what) :
      Exception(what), status_code(500) { }
    const int status_code;
  };

  class SocketError : public Exception {
  public:
    explicit SocketError(const string &what) : Exception(what) { }
  };

  class SocketEOFError : public SocketError {
  public:
    SocketEOFError(int read): SocketError("EOF"), read(read) { }
    int readSize() { return read; }
  private:
    int read;
  };

  class NotImplementError : public Exception {
  public:
    explicit NotImplementError(const string &what) : Exception(what) { }
  };

  class ParseError : public Exception {
  public:
    explicit ParseError(const string &what) : Exception(what) { }
  };

}

#endif /* EXCEPTIONS_H */

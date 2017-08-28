#ifndef SOCKET_H
#define SOCKET_H

#include "Archive.hpp"
#include "Exceptions.hpp"
#include "defines.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stddef.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

namespace DockerClientpp {
class Socket {
 public:
  Socket(const SOCK_TYPE type, const string &path);
  ~Socket();
  void connect();
  void read(char *buffer, size_t size);
  string read(size_t size);
  size_t readLine(char *buffer);
  const string &readLine(string &buffer);

  void write(const char *content, size_t size);
  void write(const string &content);
  void write(Utility::Archive &archive);

 private:
  class Impl;
  unique_ptr<Impl> m_impl;
};
}

#endif /* SOCKET_H */

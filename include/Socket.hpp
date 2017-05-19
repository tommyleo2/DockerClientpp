#ifndef SOCKET_H
#define SOCKET_H

#include "types.hpp"
#include "Exceptions.hpp"

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stddef.h>

namespace DockerClientpp {
  class Socket {
  public:
    Socket(const SOCK_TYPE type, const std::string &path);
    ~Socket();
    void connect();
    void read(char *buffer, size_t size);
    std::string read(size_t size);
    size_t readLine(char *buffer);
    const std::string &readLine(std::string &buffer);
    void write(const char *content, size_t size);
    void write(const std::string &content);
  private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
  };

}

#endif /* SOCKET_H */

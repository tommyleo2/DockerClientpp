#ifndef DOCKER_CLIENT_PP_SOCKET_H
#define DOCKER_CLIENT_PP_SOCKET_H

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

  /**
   * @brief Connect the socket
   */
  void connect();

  /**
   * @brief Close the socket
   */
  void close();

  /**
   * @brief Read data from socket
   * @param buffer buffer the data to be written into
   * @param size size of data to be read
   */
  void read(char *buffer, size_t size);

  /**
   * @brief Read data from socket
   * @param size size of data to be read
   * @return data read from socket
   */
  string read(size_t size);

  /**
   * @brief Read a line from socket
   *
   * Designed for reading http header, the line will not contain '\\r' or '\\n'
   * when return
   *
   * @param buffer buffer the data to be written into
   * @return length of the line
   */
  size_t readLine(char *buffer);

  /**
   * @brief Read a line from socket
   *
   * Designed for reading http header, the line will not contain '\\r' or '\\n'
   * when return
   *
   * @param buffer buffer the data to be written into
   * @return the same buffer that is passed in
   */
  const string &readLine(string &buffer);

  /**
   * @brief Write data to socket
   * @param content content to be sent
   * @param size size of the data
   */
  void write(const char *content, size_t size);

  /**
   * @brief Write data to socket
   * @param content content to be sent
   */
  void write(const string &content);

  /**
   * @brief Write archive to socket
   * @param archive archive to be sent
   */
  void write(Utility::Archive &archive);

 private:
  class Impl;
  unique_ptr<Impl> m_impl;
};
}  // namespace DockerClientpp

#endif /* DOCKER_CLIENT_PP_SOCKET_H */

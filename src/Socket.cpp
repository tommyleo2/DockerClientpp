#include "Socket.hpp"

using std::string;

namespace DockerClientpp {
  class Socket::Impl {
  public:
    Impl(const SOCK_TYPE type, const string &path);
    ~Impl();
    void connect();
    void read(char *buffer, size_t size);
    size_t readLine(char *buffer);
    const std::string &readLine(std::string &buffer);

    void write(const char *buffer, size_t size);
    void write(Utility::Archive &archive);
  private:
    int fd;
    int addr_length;
    char addr[64];
  };
}

using namespace DockerClientpp;

Socket::Impl::Impl(const SOCK_TYPE type, const string &path) : fd(-1) {
  if (type == UNIX) {
    sockaddr_un server_socket_addr;
    memset(&server_socket_addr, 0, sizeof(sockaddr_un));
    server_socket_addr.sun_family = AF_UNIX;
    strcpy(server_socket_addr.sun_path, path.c_str());
    addr_length = offsetof(sockaddr_un, sun_path) +
      strlen(server_socket_addr.sun_path) + 1;
    memcpy(&addr, &server_socket_addr, addr_length);
  } else if (type == TCP) {
    //  TODO: URL support
    sockaddr_in server_socket_addr;

    auto seperate_position = path.find(':');
    server_socket_addr.sin_port = htons(std::stoi(path.substr(seperate_position + 1)));
    server_socket_addr.sin_addr.s_addr = inet_addr(path.substr(0, seperate_position).c_str());
    server_socket_addr.sin_family = AF_INET;
    addr_length = sizeof(server_socket_addr);
    memcpy(&addr, &server_socket_addr, addr_length);
  }

  // sockaddr *addr_ptr = reinterpret_cast<sockaddr *>(addr);
  // if ((fd = socket(addr_ptr->sa_family, SOCK_STREAM, 0)) < 0) {
  //   throw SocketError(strerror(errno));
  // }
  // if (::connect(fd, addr_ptr, addr_length) < 0) {
  //   close(fd);
  //   throw SocketError(strerror(errno));
  // }
}

Socket::Impl::~Impl() {
  close(fd);
}

void Socket::Impl::connect() {
  close(fd);
  sockaddr *addr_ptr = reinterpret_cast<sockaddr *>(addr);
  if ((fd = socket(addr_ptr->sa_family, SOCK_STREAM, 0)) < 0) {
    throw SocketError(strerror(errno));
  }
  if (::connect(fd, addr_ptr, addr_length) < 0) {
    close(fd);
    throw SocketError(strerror(errno));
  }
}

void Socket::Impl::read(char *buffer, size_t size) {
  ssize_t read_d = 0;
  size_t total = 0;
  while (total < size) {
    read_d = ::read(fd, buffer + total, size - total);
    if (read_d == 0) {
      throw SocketEOFError(total);
    }
    if (read_d < 0) {
      throw SocketError(strerror(errno));
    }
    total += read_d;
  }
}

size_t Socket::Impl::readLine(char *buffer) {
  int total = 0;
  while (true) {
    int read_d = ::read(fd, buffer + total, 1);
    if (read_d == 0) {
      throw SocketEOFError(total);
    }
    if (read_d < 0) {
      throw SocketError(strerror(errno));
    }
    if (buffer[total] == '\r') {
      total += read_d;
      int read_d = ::read(fd, buffer + total, 1);
      if (read_d == 0) {
        throw SocketEOFError(total);
      }
      if (read_d < 0) {
        throw SocketError(strerror(errno));
      }
      if (buffer[total] == '\n') {
        buffer[total - 1] = 0;
        return total - 1;
      }
    }
  }
}

const std::string &Socket::Impl::readLine(std::string &buffer) {
  char ch;
  while (true) {
    int read_d = ::read(fd, &ch, 1);
    if (read_d == 0) {
      throw SocketEOFError(buffer.size());
    }
    if (read_d < 0) {
      throw SocketError(strerror(errno));
    }
    if (ch == '\r') {
      char ch_2;
      int read_d = ::read(fd, &ch_2, 1);
      if (read_d == 0) {
        throw SocketEOFError(buffer.size());
      }
      if (read_d < 0) {
        throw SocketError(strerror(errno));
      }
      if (ch_2 == '\n') {
        return buffer;
      } else {
        buffer += ch;
        buffer += ch_2;
      }
    } else {
      buffer += ch;
    }
  }
}

void Socket::Impl::write(const char *buffer, size_t size) {
  int written = 0;
  size_t total_size = written;
  //  cout << req << endl;
  while (total_size < size) {
    written = ::write(fd, buffer + total_size, size - total_size);
    if (written == -1) {
      throw SocketError(strerror(errno));
    }
    total_size += written;
  }
}

void Socket::Impl::write(Utility::Archive &archive) {
  archive.writeToFd(fd);
}

//-------------------------Socket Implementation-------------------------//

Socket::Socket(const SOCK_TYPE type, const std::string &path) :
  m_impl(new Impl(type, path)) { }

Socket::~Socket() { }

void Socket::connect() {
  m_impl->connect();
}

void Socket::read(char *buffer, size_t size) {
  m_impl->read(buffer, size);
}

std::string Socket::read(size_t size) {
  std::string result;
  const int BUFFER_SIZE = 256;
  char buffer[BUFFER_SIZE];
  while (size > BUFFER_SIZE) {
    m_impl->read(buffer, BUFFER_SIZE);
    result.append(buffer, BUFFER_SIZE);
    size -= BUFFER_SIZE;
  }
  m_impl->read(buffer, size);
  result.append(buffer, size);
  return result;
}

size_t Socket::readLine(char *buffer) {
  return m_impl->readLine(buffer);
}

const std::string &Socket::readLine(std::string &buffer) {
  return m_impl->readLine(buffer);
}

void Socket::write(const char *content, size_t size) {
  m_impl->write(content, size);
}

void Socket::write(const std::string &content) {
  m_impl->write(content.c_str(), content.size());
}

void Socket::write(Utility::Archive &archive) {
  m_impl->write(archive);
}

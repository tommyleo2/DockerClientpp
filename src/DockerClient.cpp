#include "DockerClient.hpp"

using std::string;

namespace DockerClientpp {
  class DockerClientImpl : public DockerClientImplBase {
  public:
    DockerClientImpl(SOCK_TYPE type,
                     const string &path);
    virtual void createContainer(OptionSetter &session) override;
    ~DockerClientImpl();

  private:
    int socket_fd;  ///< socket file descriptor
  };
}

using namespace DockerClientpp;
using namespace Http;

DockerClientImpl::DockerClientImpl(SOCK_TYPE type, const string &path) {
  if (type == UNIX) {
    if ((socket_fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
      throw std::runtime_error(strerror(errno));
    }
    sockaddr_un server_socket_addr;
    memset(&server_socket_addr, 0, sizeof(sockaddr_un));
    server_socket_addr.sun_family = AF_UNIX;
    strcpy(server_socket_addr.sun_path, path.c_str());
    int sockaddr_length = offsetof(sockaddr_un, sun_path) +
      strlen(server_socket_addr.sun_path) + 1;
    if (connect(socket_fd,
                reinterpret_cast<sockaddr *>(&server_socket_addr),
                sockaddr_length) < 0) {
      close(socket_fd);
      throw std::runtime_error(strerror(errno));
    }
  } else if (type == TCP) {
    //  TODO: URL support
    sockaddr_in server_socket_addr;
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      throw std::runtime_error(strerror(errno));
    }
    auto seperate_position = path.find(':');
    server_socket_addr.sin_port = htons(std::stoi(path.substr(seperate_position + 1)));
    server_socket_addr.sin_addr.s_addr = inet_addr(path.substr(0, seperate_position).c_str());
    server_socket_addr.sin_family = AF_INET;

    if (connect(socket_fd, reinterpret_cast<sockaddr *>(&server_socket_addr),
                sizeof(server_socket_addr) ) < 0) {
      throw std::runtime_error(strerror(errno));
    }
  }
}

void DockerClientImpl::createContainer(DockerClientpp::OptionSetter &session) {
  QueryParam *query_param = nullptr;
  auto it = session.data.find("name");
  if (it != session.data.end()) {
    query_param = new QueryParam;
    query_param->operator[]("name") = *it;
  }
  Response res = Http::Post(socket_fd,
                            "/v1.24/containers/create",
                            // TODO
                            query_param,
                            session.dump());

}

DockerClientImpl::~DockerClientImpl() {
  close(socket_fd);
}

DockerClient::DockerClient(SOCK_TYPE type,
                           const string &path) :
  m_impl(new DockerClientpp::DockerClientImpl(type, path)) { }

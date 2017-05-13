#include "SimpleHttpClient.hpp"

using namespace DockerClientpp::Http;
// using namespace Http;
using std::string;

namespace DockerClientpp {
  namespace Http {
    class SimpleHttpClient::Impl {
    public:
      Impl(const SOCK_TYPE type, const std::string &path);
      ~Impl();
      Response Post(const Uri &uri,
                    const Header &header,
                    const QueryParam &query_param,
                    const std::string &data);
      Response Get(const Uri &uri,
                   const Header &header,
                   const QueryParam &query_param);

    private:
      string buildQuery(const Http::QueryParam &query_param);

      Response sendAndRecieve(const string &req);

      void sendRequest(const string &req);
      void getResponseHeader(Response &response);

      int getStatusCode(const string &line);

      string readLineFromSocket();
      string readFromSocket(int size);

    private:
      int fd;
    };
  }
}

using std::cout;
using std::endl;

//using namespace __private;

const int READ_BUFFER_SIZE = 256;

SimpleHttpClient::Impl::Impl(const SOCK_TYPE type, const std::string &path) {
  if (type == UNIX) {
    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
      throw SocketError(strerror(errno));
    }
    sockaddr_un server_socket_addr;
    memset(&server_socket_addr, 0, sizeof(sockaddr_un));
    server_socket_addr.sun_family = AF_UNIX;
    strcpy(server_socket_addr.sun_path, path.c_str());
    int sockaddr_length = offsetof(sockaddr_un, sun_path) +
      strlen(server_socket_addr.sun_path) + 1;
    if (connect(fd,
                reinterpret_cast<sockaddr *>(&server_socket_addr),
                sockaddr_length) < 0) {
      close(fd);
      throw SocketError(strerror(errno));
    }
  } else if (type == TCP) {
    //  TODO: URL support
    sockaddr_in server_socket_addr;
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      throw SocketError(strerror(errno));
    }
    auto seperate_position = path.find(':');
    server_socket_addr.sin_port = htons(std::stoi(path.substr(seperate_position + 1)));
    server_socket_addr.sin_addr.s_addr = inet_addr(path.substr(0, seperate_position).c_str());
    server_socket_addr.sin_family = AF_INET;

    if (connect(fd, reinterpret_cast<sockaddr *>(&server_socket_addr),
                sizeof(server_socket_addr) ) < 0) {
      throw SocketError(strerror(errno));
    }
  }
}

SimpleHttpClient::Impl::~Impl() {
  close(fd);
}

Response SimpleHttpClient::Impl::Post(const Uri &uri,
                                      const Header &header,
                                      const QueryParam &query_param,
                                      const string &data) {
  //  build request text
  string sent_data("POST ");
  string uri_with_query = uri + buildQuery(query_param);
  sent_data += uri_with_query;
  sent_data += " HTTP/1.1\r\n";
  sent_data += Utility::dumpHeader(header);
  sent_data += data;

  Response response = sendAndRecieve(sent_data);
  response.uri = uri_with_query;
  return response;
}

Response SimpleHttpClient::Impl::Get(const Uri &uri,
                                     const Header &header,
                                     const QueryParam &query_param) {
  //  build request text
  string sent_data("GET ");
  string uri_with_query = uri + buildQuery(query_param);
  sent_data += uri_with_query;
  sent_data += " HTTP/1.1\r\n";
  sent_data += Utility::dumpHeader(header);

  Response response = sendAndRecieve(sent_data);
  response.uri = uri_with_query;
  return response;
}

string SimpleHttpClient::Impl::buildQuery(const Http::QueryParam &query_param) {
  string result;
  if (!query_param.empty()) {
    // TODO: parse special characters
    result+= "?";
    for (auto it = query_param.begin(); it != query_param.end(); it++) {
      result += it->first + "=" + it->second + "&";
    }
    result.pop_back();
  }
  return result;
}

Response SimpleHttpClient::Impl::sendAndRecieve(const string &sent_data) {
  sendRequest(sent_data);

  Response response;
  getResponseHeader(response);

  //  TODO: A better solution for skipping entity
  if (response.status_code == 204)
    return response;

  auto length_it = response.header.find("Content-Length");
  if (length_it == response.header.end())
    throw NotImplementError("Cannot handle http response header without Content-Length");
  int length = std::stoi(length_it->get<std::string>());
  response.body = readFromSocket(length);
  return response;
}

void SimpleHttpClient::Impl::sendRequest(const string &req) {
  int written = 0;
  int total_size = written;
  while (total_size < req.size()) {
    written = write(fd, req.c_str() + total_size, req.size() - total_size);
    if (written == -1) {
      throw SocketError(strerror(errno));
    }
    total_size += written;
  }
}

void SimpleHttpClient::Impl::getResponseHeader(Response &response) {
  string response_header;
  //  Read http response header from socket
  string line = readLineFromSocket();
  response.status_code = getStatusCode(line);
  while (not (line = readLineFromSocket()).empty()) {
    response_header += line + '\n';
  }

  response.header = Utility::loadHeader(response_header);
}

int SimpleHttpClient::Impl::getStatusCode(const string &line) {
  auto pos = line.find(' ') + 1;
  return std::stoi(line.substr(pos, 3));
}

string SimpleHttpClient::Impl::readLineFromSocket() {
  string result;
  char buf;
  while (true) {
    if (read(fd, &buf, 1) < 1)
      throw SocketError(strerror(errno));
    if (buf != '\r') {
      result.push_back(buf);
    } else {
      if (read(fd, &buf, 1) < 1)
        throw SocketError(strerror(errno));
      if (buf == '\n')
        break;
      else
        result.push_back(buf);
    }
  }
  return result;
}

string SimpleHttpClient::Impl::readFromSocket(int size) {
  string result;
  const int BUFF_SIZE = 256;
  char buff[BUFF_SIZE];
  int read_d = 0;
  while (size > 0) {
    if (size < BUFF_SIZE) {
      read_d = read(fd, buff, size);
    } else {
      read_d = read(fd, buff, BUFF_SIZE - 1);
    }
    if (read_d < 0)
      throw SocketError(strerror(errno));
    buff[read_d] = 0;
    result.append(buff, read_d);
    size -= read_d;
  }
  return result;
}

//-------------------------SimpleHttpClient Implementation-------------------------//

SimpleHttpClient::SimpleHttpClient(const SOCK_TYPE type,
                                   const string &path) :
  m_impl(new Impl(type, path)) { }

SimpleHttpClient::~SimpleHttpClient() { }

Response SimpleHttpClient::Post(const Uri &uri,
                                const Header &header,
                                const QueryParam &query_param,
                                const std::string &data) {
  return m_impl->Post(uri, header, query_param, data);
}

Response SimpleHttpClient::Get(const Uri &uri,
                               const Header &header,
                               const QueryParam &query_param) {
  return m_impl->Get(uri, header, query_param);
}

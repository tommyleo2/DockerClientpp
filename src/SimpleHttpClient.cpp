#include "SimpleHttpClient.hpp"
#include "Socket.hpp"

using namespace DockerClientpp::Http;
// using namespace Http;
using std::string;
using std::shared_ptr;

namespace DockerClientpp {
  namespace Http {
    class SimpleHttpClient::Impl {
    public:
      Impl(const SOCK_TYPE type, const std::string &path);
      ~Impl();
      shared_ptr<Response> Post(const Uri &uri,
                                const Header &header,
                                const QueryParam &query_param,
                                const std::string &data);
      shared_ptr<Response> Get(const Uri &uri,
                               const Header &header,
                               const QueryParam &query_param);

    private:
      string buildQuery(const Http::QueryParam &query_param);

      std::shared_ptr<Response> sendAndRecieve(const string &req);

      void sendRequest(const string &req);
      void getResponseHeader(std::shared_ptr<Response> &response);

      int getStatusCode(const string &line);

    private:
      Socket socket;
    };
  }
}

using std::cout;
using std::endl;

const int READ_BUFFER_SIZE = 256;

SimpleHttpClient::Impl::Impl(const SOCK_TYPE type, const std::string &path) :
  socket(type, path) { }

SimpleHttpClient::Impl::~Impl() { }

shared_ptr<Response> SimpleHttpClient::Impl::Post(const Uri &uri,
                                                  const Header &header,
                                                  const QueryParam &query_param,
                                                  const string &data) {
  socket.connect();
  //  build request text
  string sent_data("POST ");
  string uri_with_query = uri + buildQuery(query_param);
  sent_data += uri_with_query;
  sent_data += " HTTP/1.1\r\n";
  sent_data += Utility::dumpHeader(header);
  sent_data += data;

  shared_ptr<Response> response = sendAndRecieve(sent_data);
  response->uri = uri_with_query;
  return response;
}

shared_ptr<Response> SimpleHttpClient::Impl::Get(const Uri &uri,
                                                 const Header &header,
                                                 const QueryParam &query_param) {
  socket.connect();
  //  build request text
  string sent_data("GET ");
  string uri_with_query = uri + buildQuery(query_param);
  sent_data += uri_with_query;
  sent_data += " HTTP/1.1\r\n";
  sent_data += Utility::dumpHeader(header);

  shared_ptr<Response> response = sendAndRecieve(sent_data);
  response->uri = uri_with_query;
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

shared_ptr<Response> SimpleHttpClient::Impl::sendAndRecieve(const string &sent_data) {
  sendRequest(sent_data);

  shared_ptr<Response> response = std::make_shared<Response>();
  getResponseHeader(response);

  //  TODO: A better solution for skipping entity
  if (response->status_code == 204)
    return response;

  auto length_it = response->header.find("Content-Length");
  auto end_it = response->header.end();
  if (length_it != end_it) {
    //  Read specific length of data
    int length = std::stoi(length_it->get<std::string>());
    response->body = socket.read(length);
  } else if ((length_it = response->header.find("Transfer-Encoding")) != end_it &&
             *length_it == "chunked") {
    //  Read according to chunked size
    int chunk_size = 0;
    do {
      string line;
      chunk_size = std::stoi(socket.readLine(line), nullptr, 16);
      response->body += socket.read(chunk_size);
      socket.readLine(line);
    } while (chunk_size);
  } else if ((length_it = response->header.find("Content-Type")) != end_it &&
             *length_it == "application/vnd.docker.raw-stream") {
    int chunk_size = 0;
    while (true) {
      //  Read stream according to docker stream protocol
      //  https://docs.docker.com/engine/api/v1.24/#attach-to-a-container
      //  response->chunk->push_back(Chunk{readFromSocket(1)[0], ""});
      string line;
      try {
        socket.read(4);
        chunk_size = __builtin_bswap32(*reinterpret_cast<const unsigned int *>(socket.read(4).c_str()));
      } catch (SocketError &e) {
        break;
      }
      response->body += (socket.read(chunk_size));
    }
  } else {
    throw NotImplementError("Cannot handle http response header without Content-Length"
                            " or Transfer-Encoding: chunked");
  }
  return response;
}

void SimpleHttpClient::Impl::sendRequest(const string &req) {
  socket.write(req);
}

void SimpleHttpClient::Impl::getResponseHeader(shared_ptr<Response> &response) {
  //  Read http response header from socket
  // if (flag) {
  //   string line = readFromSocket(1000);
  //   cout << line << endl;
  // }
  string line;
  socket.readLine(line);
  response->status_code = getStatusCode(line);
  line.clear();
  while (not socket.readLine(line).empty()) {
    auto pos = line.find(':');
    if (pos == string::npos)
      throw ParseError("Parse http header error, which is: " + line);
    response->header[line.substr(0, pos)] = line.substr(pos + 2);
    line.clear();
  }
}

int SimpleHttpClient::Impl::getStatusCode(const string &line) {
  auto pos = line.find(' ') + 1;
  return std::stoi(line.substr(pos, 3));
}


//-------------------------SimpleHttpClient Implementation-------------------------//

SimpleHttpClient::SimpleHttpClient(const SOCK_TYPE type,
                                   const string &path) :
  m_impl(new Impl(type, path)) { }

SimpleHttpClient::~SimpleHttpClient() { }

shared_ptr<Response> SimpleHttpClient::Post(const Uri &uri,
                                            const Header &header,
                                            const QueryParam &query_param,
                                            const std::string &data) {
  return m_impl->Post(uri, header, query_param, data);
}

shared_ptr<Response> SimpleHttpClient::Get(const Uri &uri,
                                           const Header &header,
                                           const QueryParam &query_param) {
  return m_impl->Get(uri, header, query_param);
}

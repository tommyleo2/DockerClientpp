#include "DockerClient.hpp"

using std::string;

namespace DockerClientpp {
  class DockerClientImpl : public DockerClientImplBase {
  public:
    DockerClientImpl(const SOCK_TYPE type,
                     const string &path);
    virtual void setAPIVersion(const string &api) override;
    virtual string createContainer(OptionSetter &session) override;

  private:
    Http::Header createCommonHeader(size_t content_length);

    Http::SimpleHttpClient http_client;
    string api_version;
  };
}

using namespace DockerClientpp;
using namespace Http;
using namespace Utility;
using nlohmann::json;

DockerClientImpl::DockerClientImpl(const SOCK_TYPE type, const string &path) :
  http_client(type, path),
  api_version("v1.24") {

}

void DockerClientImpl::setAPIVersion(const string &api) {
  api_version = api;
}

Http::Header DockerClientImpl::createCommonHeader(size_t content_length) {
  return {
    {"Content-Type", "application/json"},
    {"Content-Length", std::to_string(content_length)},
    {"Host", api_version},
    {"Accept", "*/*"},
  };
}

string DockerClientImpl::createContainer(DockerClientpp::OptionSetter &session) {
  QueryParam query_param {};
  auto it = session.data.find("name");
  if (it != session.data.end()) {
    query_param["name"] = *it;
    session.data.erase(it);
  }
  string raw_request = session.dump();
  Header header = createCommonHeader(raw_request.size());
  Response res = http_client.Post("/containers/create",
                                  header,
                                  query_param,
                                  raw_request);
  json body = json::parse(res.body);
  switch(res.status_code) {
  case 201:
    break;
  default:
    throw Exception(body["message"].get<string>());
  }
  return body["Id"];
}

DockerClient::DockerClient(const SOCK_TYPE type,
                           const string &path) :
  m_impl(new DockerClientpp::DockerClientImpl(type, path)) { }

void DockerClient::setAPIVersion(const string &api) {
  m_impl->setAPIVersion(api);
}

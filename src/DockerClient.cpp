#include "DockerClient.hpp"

#include <fstream>

using std::string;
using std::shared_ptr;
using std::list;

namespace DockerClientpp {
  class DockerClientImpl : public DockerClientImplBase {
  public:
    DockerClientImpl(const SOCK_TYPE type,
                     const string &path);
    virtual void setAPIVersion(const string &api) override;
    virtual string createContainer(OptionSetter &option) override;
    virtual void startContainer(const string &identifier) override;
    virtual void stopContainer(const std::string &identifier) override;
    virtual string createExecution(const string &identifier,
                                   const OptionSetter &option) override;
    virtual string startExecution(const string &id,
                                  const OptionSetter &option) override;
    virtual string listImages() override;
    virtual void putFiles(const string &identifier,
                          const vector<string> &files,
                          const string &path) override;
    virtual string inspectExecution(const string &id) override;

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

string DockerClientImpl::listImages() {
  Header header = createCommonHeader(0);
  shared_ptr<Response> res = http_client.Get("/images/json",
                                 header,
                                 {});
  switch (res->status_code) {
  case 200: {
    break;
  }
  default:
    json body = json::parse(res->body);
    throw Exception(body["message"].get<string>());
    break;
  }
  return res->body;
}

Http::Header DockerClientImpl::createCommonHeader(size_t content_length) {
  return {
    {"Content-Type", "application/json"},
    {"Content-Length", std::to_string(content_length)},
    {"Host", api_version},
    {"Accept", "*/*"},
  };
}

string DockerClientImpl::createContainer(DockerClientpp::OptionSetter &option) {
  QueryParam query_param {};
  auto it = option.data.find("name");
  if (it != option.data.end()) {
    query_param["name"] = *it;
    option.data.erase(it);
  }
  string post_data = option.dump();
  Header header = createCommonHeader(post_data.size());
  shared_ptr<Response> res = http_client.Post("/containers/create",
                                  header,
                                  query_param,
                                  post_data);
  json body = json::parse(res->body);
  switch(res->status_code) {
  case 201:
    break;
  default:
    throw Exception(body["message"].get<string>());
  }
  return body["Id"];
}

void DockerClientImpl::startContainer(const string &identifier) {
  Header header = createCommonHeader(0);
  shared_ptr<Response> res = http_client.Post("/containers/" + identifier + "/start",
                                  header, {}, "");
  switch(res->status_code) {
  case 204:
    break;
  default: {
    json body = json::parse(res->body);
    throw Exception(body["message"].get<string>());
  }
  }
}

void DockerClientImpl::stopContainer(const string &identifier) {
  Header header = createCommonHeader(0);
  shared_ptr<Response> res = http_client.Post("/containers/" + identifier + "/stop",
                                  header, {}, "");
  switch(res->status_code) {
  case 204:
    break;
  default: {
    json body = json::parse(res->body);
    throw Exception(body["message"].get<string>());
  }
  }
}

string DockerClientImpl::createExecution(const string &identifier,
                                         const OptionSetter &option) {
  string post_data = option.dump();
  Header header = createCommonHeader(post_data.size());
  shared_ptr<Response> res = http_client.Post("/containers/" + identifier + "/exec",
                                  header,
                                  { },
                                  post_data);
  json body = json::parse(res->body);
  switch(res->status_code) {
  case 201:
    break;
  default:
    throw Exception(body["message"].get<string>());
  }
  return body["Id"];
}

string DockerClientImpl::startExecution(const string &id,
                                        const OptionSetter &option) {
  string post_data = option.dump();
  Header header = createCommonHeader(post_data.size());
  shared_ptr<Response> res = http_client.Post("/exec/" + id + "/start",
                                  header,
                                  { },
                                  post_data);
  switch(res->status_code) {
  case 200:
    break;
  default:
    json body = json::parse(res->body);
    throw Exception(body["message"].get<string>());
  }
  return res->body;
}

void DockerClientImpl::putFiles(const string &identifier,
                                const vector<string> &files,
                                const string &path) {
  Utility::Archive ar(files);
  string put_data;
  ar.getTar(put_data);
  Header header = createCommonHeader(put_data.size());

  header["Content-Type"] = "application/x-tar";
  QueryParam query_param {
    {"path", path}
  };
  shared_ptr<Response> res = http_client.Put("/containers/" + identifier + "/archive",
                                             header,
                                             query_param,
                                             put_data);
  switch(res->status_code) {
  case 200:
    break;
  default:
    json body = json::parse(res->body);
    throw Exception(body["message"].get<string>());
  }
}

string DockerClientImpl::inspectExecution(const string &id) {
  Header header = createCommonHeader(0);
  shared_ptr<Response> res = http_client.Get("/exec/" + id + "/json",
                                             header,
                                             {});
  switch(res->status_code) {
  case 200:
    break;
  default:
    json body = json::parse(res->body);
    throw Exception(body["message"].get<string>());
  }
  return res->body;
}

//-------------------------DockerClient Implementation-------------------------

DockerClient::DockerClient(const SOCK_TYPE type,
                           const string &path) :
  m_impl(new DockerClientpp::DockerClientImpl(type, path)) { }

void DockerClient::setAPIVersion(const string &api) {
  m_impl->setAPIVersion(api);
}

void DockerClient::startContainer(const string &identifier) {
  m_impl->startContainer(identifier);
}

void DockerClient::stopContainer(const string &identifier) {
  m_impl->stopContainer(identifier);
}

void DockerClient::putFiles(const string &identifier,
                            const vector<string> &files, const string &path) {
  m_impl->putFiles(identifier, files, path);
}

string DockerClient::inspectExecution(const string &id) {
  return m_impl->inspectExecution(id);
}

#include "DockerClient.hpp"
#include "SimpleHttpClient.hpp"

#include <fstream>

namespace DockerClientpp {
class DockerClient::Impl {
 public:
  Impl(const SOCK_TYPE type, const string &path);
  ~Impl();
  void setAPIVersion(const string &api);
  string listImages();
  string createContainer(const json &config, const string &name = "");
  void startContainer(const string &identifier);
  void stopContainer(const std::string &identifier);
  string createExecution(const string &identifier, const json &config);
  string startExecution(const string &id, const json &config);
  string inspectExecution(const string &id);
  void putFiles(const string &identifier, const vector<string> &files,
                const string &path);
  void getFile(const string &identifier, const string &file,
               const string &path);

 private:
  Http::Header createCommonHeader(size_t content_length);

  Http::SimpleHttpClient http_client;
  string api_version;
};
}

using namespace DockerClientpp;
using namespace Http;
using namespace Utility;

DockerClient::Impl::Impl(const SOCK_TYPE type, const string &path)
    : http_client(type, path), api_version("v1.24") {}

DockerClient::Impl::~Impl() {}

void DockerClient::Impl::setAPIVersion(const string &api) {
  api_version = api;
}

string DockerClient::Impl::listImages() {
  Header header = createCommonHeader(0);
  shared_ptr<Response> res = http_client.Get("/images/json", header, {});
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

Http::Header DockerClient::Impl::createCommonHeader(size_t content_length) {
  return {
      {"Content-Type", "application/json"},
      {"Content-Length", std::to_string(content_length)},
      {"Host", api_version},
      {"Accept", "*/*"},
  };
}

string DockerClient::Impl::createContainer(const json &config,
                                           const string &name) {
  QueryParam query_param{};
  if (!name.empty()) {
    query_param["name"] = name;
  }

  string post_data = config.dump();
  Header header = createCommonHeader(post_data.size());
  shared_ptr<Response> res =
      http_client.Post("/containers/create", header, query_param, post_data);
  json body = json::parse(res->body);
  switch (res->status_code) {
    case 201:
      break;
    default:
      throw Exception(body["message"].get<string>());
  }
  return body["Id"];
}

void DockerClient::Impl::startContainer(const string &identifier) {
  Header header = createCommonHeader(0);
  shared_ptr<Response> res =
      http_client.Post("/containers/" + identifier + "/start", header, {}, "");
  switch (res->status_code) {
    case 204:
      break;
    default: {
      json body = json::parse(res->body);
      throw Exception(body["message"].get<string>());
    }
  }
}

void DockerClient::Impl::stopContainer(const string &identifier) {
  Header header = createCommonHeader(0);
  shared_ptr<Response> res =
      http_client.Post("/containers/" + identifier + "/stop", header, {}, "");
  switch (res->status_code) {
    case 204:
      break;
    default: {
      json body = json::parse(res->body);
      throw Exception(body["message"].get<string>());
    }
  }
}

string DockerClient::Impl::createExecution(const string &identifier,
                                           const json &config) {
  string post_data = config.dump();
  Header header = createCommonHeader(post_data.size());
  shared_ptr<Response> res = http_client.Post(
      "/containers/" + identifier + "/exec", header, {}, post_data);
  json body = json::parse(res->body);
  switch (res->status_code) {
    case 201:
      break;
    default:
      throw Exception(body["message"].get<string>());
  }
  return body["Id"];
}

string DockerClient::Impl::startExecution(const string &id,
                                          const json &config) {
  string post_data = config.dump();
  Header header = createCommonHeader(post_data.size());
  shared_ptr<Response> res =
      http_client.Post("/exec/" + id + "/start", header, {}, post_data);
  switch (res->status_code) {
    case 200:
      break;
    default:
      json body = json::parse(res->body);
      throw Exception(body["message"].get<string>());
  }
  return res->body;
}

string DockerClient::Impl::inspectExecution(const string &id) {
  Header header = createCommonHeader(0);
  shared_ptr<Response> res =
      http_client.Get("/exec/" + id + "/json", header, {});
  switch (res->status_code) {
    case 200:
      break;
    default:
      json body = json::parse(res->body);
      throw Exception(body["message"].get<string>());
  }
  return res->body;
}

void DockerClient::Impl::putFiles(const string &identifier,
                                  const vector<string> &files,
                                  const string &path) {
  Utility::Archive ar;
  ar.addFiles(files);
  string put_data = ar.getTar(false);
  Header header = createCommonHeader(put_data.size());

  header["Content-Type"] = "application/x-tar";
  QueryParam query_param{{"path", path}};
  shared_ptr<Response> res = http_client.Put(
      "/containers/" + identifier + "/archive", header, query_param, put_data);
  switch (res->status_code) {
    case 200:
      break;
    default:
      json body = json::parse(res->body);
      throw Exception(body["message"].get<string>());
  }
}

void DockerClient::Impl::getFile(const string &identifier, const string &file,
                                 const string &path) {
  Header header = createCommonHeader(0);
  shared_ptr<Response> res = http_client.Get(
      "/containers/" + identifier + "/archive", header, {{"path", file}});
  switch (res->status_code) {
    case 200:
      break;
    default:
      json body = json::parse(res->body);
      throw Exception(body["message"].get<string>());
  }

  Utility::Archive::extractTar(res->body, path);
}

//-------------------------DockerClient Implementation-------------------------

DockerClient::DockerClient(const SOCK_TYPE type, const string &path)
    : m_impl(new Impl(type, path)) {}

DockerClient::~DockerClient() {}

void DockerClient::setAPIVersion(const string &api) {
  m_impl->setAPIVersion(api);
}

string DockerClient::listImages() {
  return m_impl->listImages();
}

string DockerClient::createContainer(const json &config, const string &name) {
  return m_impl->createContainer(config, name);
}

void DockerClient::startContainer(const string &identifier) {
  m_impl->startContainer(identifier);
}

void DockerClient::stopContainer(const string &identifier) {
  m_impl->stopContainer(identifier);
}

string DockerClient::createExecution(const string &identifier,
                                     const json &config) {
  return m_impl->createExecution(identifier, config);
}

string DockerClient::startExecution(const string &id, const json &config) {
  return m_impl->startExecution(id, config);
}

string DockerClient::inspectExecution(const string &id) {
  return m_impl->inspectExecution(id);
}

void DockerClient::putFiles(const string &identifier,
                            const vector<string> &files, const string &path) {
  m_impl->putFiles(identifier, files, path);
}

void DockerClient::getFile(const string &identifier, const string &file,
                           const string &path) {
  m_impl->getFile(identifier, file, path);
}

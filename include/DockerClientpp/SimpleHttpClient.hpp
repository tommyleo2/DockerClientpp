#ifndef DOCKER_CLIENT_PP_SIMPLEHTTPCLIENT_H
#define DOCKER_CLIENT_PP_SIMPLEHTTPCLIENT_H

#include "Exceptions.hpp"
#include "Response.hpp"
#include "Utility.hpp"
#include "defines.hpp"

#include <algorithm>

namespace DockerClientpp {
namespace Http {
/**
 * @brief Simple http client
 *
 * Contains some basic http request methods, with limited implementation.
 * Adapted to docker http request
 *
 */
class SimpleHttpClient {
 public:
  SimpleHttpClient(const SOCK_TYPE type, const string &path);
  ~SimpleHttpClient();
  shared_ptr<Response> Post(const Uri &uri, const Header &header,
                            const QueryParam &query_param, const string &data);
  shared_ptr<Response> Put(const Uri &uri, const Header &header,
                           const QueryParam &query_param, const string &data);
  shared_ptr<Response> Get(const Uri &uri, const Header &header,
                           const QueryParam &query_param);
  shared_ptr<Response> Delete(const Uri &uri, const Header &header,
                              const QueryParam &query_param);

 private:
  class Impl;
  unique_ptr<Impl> m_impl;
};
}  // namespace Http
}  // namespace DockerClientpp

#endif /* DOCKER_CLIENT_PP_SIMPLEHTTPCLIENT_H */

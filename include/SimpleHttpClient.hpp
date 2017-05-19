#ifndef SIMPLEHTTPCLIENT_H
#define SIMPLEHTTPCLIENT_H

#include "types.hpp"
#include "Response.hpp"
#include "Utility.hpp"
#include "Exceptions.hpp"

#include <algorithm>

namespace DockerClientpp {
  namespace Http {
    /**
     *  @brief Simple http client
     *
     *  Contains some basic http request methods, with limited implementation.
     *  Adapted to docker http request
     *
     */
    class SimpleHttpClient {
    public:
      SimpleHttpClient(const SOCK_TYPE type, const std::string &path);
      ~SimpleHttpClient();
      std::shared_ptr<Response> Post(const Uri &uri,
                                     const Header &header,
                                     const QueryParam &query_param,
                                     const std::string &data);
      std::shared_ptr<Response> Get(const Uri &uri,
                                    const Header &header,
                                    const QueryParam &query_param);

    private:
      class Impl;
      std::unique_ptr<Impl> m_impl;
    };
  }
}

#endif /* SIMPLEHTTPCLIENT_H */

#ifndef SIMPLEHTTPCLIENT_H
#define SIMPLEHTTPCLIENT_H

#include "types.hpp"

namespace DockerClientpp {
  namespace Http {
    Response Post(const int fd,
                  const Uri &uri,
                  const Header &header,
                  const QueryParam *query_param,
                  const std::string &data);
    Response Get(const int fd,
                 const Uri &uri,
                 const Header &header,
                 const QueryParam *query_param);
  }
}

#endif /* SIMPLEHTTPCLIENT_H */

#ifndef DOCKER_CLIENT_PP_RESPONSE_H
#define DOCKER_CLIENT_PP_RESPONSE_H

#include "defines.hpp"

namespace DockerClientpp {
namespace Http {
/**
 * @brief Http response class
 */
struct Response {
  // Response(Response &&) = default;
  Uri uri;                ///<  Uri of the request
  int status_code = 400;  ///<  Status code of the response
  Header header;          ///<  Header of the the response
  string body;            ///<  Body of the response
};
}  // namespace Http
}  // namespace DockerClientpp

#endif /* DOCKER_CLIENT_PP_RESPONSE_H */

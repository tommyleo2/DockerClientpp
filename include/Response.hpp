#ifndef RESPONSE_H
#define RESPONSE_H

#include "types.hpp"
#include "Chunk.hpp"

namespace DockerClientpp {
  namespace Http {
    /**
     *  @brief Http response class
     */
    struct Response {
      //Response(Response &&) = default;
      Uri uri;                    ///<  Uri of the request
      int status_code = 400;      ///<  Status code of the response
      Header header;              ///<  Header of the the response
      std::string body;           ///<  Body of the response
    };
  }
}

#endif /* RESPONSE_H */

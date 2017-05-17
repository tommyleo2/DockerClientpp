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
      Uri uri;                    ///<  Uri of the request
      int status_code = 400;      ///<  Status code of the response
      Header header;              ///<  Header of the the response
      std::string body;         ///<  Body of the response
      std::list<Chunk> chunk;   ///<  Docker raw stream chunk
    };
  }
}

#endif /* RESPONSE_H */

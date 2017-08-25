#ifndef CHUNK_H
#define CHUNK_H

#include <list>
#include <string>

namespace DockerClientpp {
/**
 * @brief Docker row stream chunk
 */
struct Chunk {
  int type;          ///<  Type 0:stdin, 1:stdout, 2:stderr
  std::string body;  ///<  Content
};
}
#endif /* CHUNK_H */

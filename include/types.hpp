#ifndef TYPES_H
#define TYPES_H

#include <map>
#include <sstream>
#include <streambuf>
#include <string>

#include "Chunk.hpp"
#include "CreateContainerOptions.hpp"
#include "CreateExecutionOption.hpp"
#include "StartExecutionOption.hpp"
#include "json.hpp"

namespace DockerClientpp {
/**
 *  @brief Type of socket that docker daemon use.
 */
enum SOCK_TYPE { UNIX, TCP };

namespace Http {
typedef std::string Uri;
typedef nlohmann::json Header;
typedef std::map<std::string, std::string> QueryParam;
}
}

#endif /* TYPES_H */

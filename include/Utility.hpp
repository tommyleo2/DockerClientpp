#ifndef UTILITY_H
#define UTILITY_H

#include "Exceptions.hpp"
#include "Response.hpp"
#include "types.hpp"

#include <sstream>

namespace DockerClientpp {
namespace Utility {
using namespace Http;
using std::string;
/**
 *  @brief Load http header from string
 *
 *  Note: The header string must not contain '\r',
 *        and it is usually called by getResponseHeader()
 *
 *  @param header_str header string
 *
 *  @return parsed Header object
 *
 *  @sa getResponseHeader()
 */
Header loadHeader(const string &header_str);

/**
 *  @brief Dump Header object to string
 *
 *  Dump Header object to http request string header, formated as
 *  Key: Value
 *
 *  @param header header to be dumped
 *
 *  @return Dumped header string
 */
string dumpHeader(const Header &header);
}
}

#endif /* UTILITY_H */

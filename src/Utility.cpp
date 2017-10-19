#include "Utility.hpp"

using namespace DockerClientpp;
using std::string;

string Utility::dumpHeader(const Header &header) {
  string result;
  for (auto it = header.begin(); it != header.end(); it++) {
    result += it.key();
    result += ": ";
    result += it.value().get<string>();
    result += "\r\n";
  }
  result += "\r\n";
  return result;
}

Http::Header Utility::loadHeader(const string &header_str) {
  Http::Header header;
  std::stringstream ss(header_str);
  string line;
  while (ss) {
    std::getline(ss, line);
    if (line.empty()) break;
    auto pos = line.find(':');
    if (pos == string::npos)
      throw ParseError("Parse http header error, which is: " + line);
    header[line.substr(0, pos)] = line.substr(pos + 2);
  }
  return header;
}

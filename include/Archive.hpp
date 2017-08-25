#ifndef ARCHIVE_H
#define ARCHIVE_H

#include "defines.hpp"

namespace DockerClientpp {
namespace Utility {
class Archive {
 public:
  Archive(const vector<string> &files, bool reserve_path = false);
  ~Archive();
  void addFile(const string &file);
  void addFile(const vector<string> &files);
  void writeToFd(const int fd);
  void getTar(string &buffer);

 private:
  class Impl;
  unique_ptr<Impl> m_impl;
};
}
}

#endif /* ARCHIVE_H */

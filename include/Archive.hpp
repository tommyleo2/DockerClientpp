#ifndef ARCHIVE_H
#define ARCHIVE_H

#include "defines.hpp"

namespace DockerClientpp {
namespace Utility {
class Archive {
 public:
  Archive();
  ~Archive();
  void addFile(const string &file);
  void addFiles(const vector<string> &files);
  void writeToFd(const int fd, bool reserve_path = false);
  string getTar(bool reserve_path = false);
  static void extractTar(const string &tar_buffer, const string &path);

 private:
  class Impl;
  unique_ptr<Impl> m_impl;
};
}
}

#endif /* ARCHIVE_H */

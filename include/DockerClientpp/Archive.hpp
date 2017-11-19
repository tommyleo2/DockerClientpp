#ifndef DOCKER_CLIENT_PP_ARCHIVE_H
#define DOCKER_CLIENT_PP_ARCHIVE_H

#include "defines.hpp"

namespace DockerClientpp {
namespace Utility {
class Archive {
 public:
  Archive();
  ~Archive();
  /**
   * @brief Add a file to the archive
   * @param file file path to the added file
   */
  void addFile(const string &file);

  /**
   * @brief Add files to the archive
   * @param files files path to the added files
   */
  void addFiles(const vector<string> &files);

  /**
   * @brief Write the archive binary to a file
   * @param fd archive file's file descriptor
   * @param reserve_path reserve file's path in archive. If false, all files
   *        will be set to root directory in the archive
   */
  void writeToFd(const int fd);

  /**
   * @brief Get archive binary string
   * @return raw string of the archive
   */
  string getTar();
  static void extractTar(const string &tar_buffer, const string &path);

 private:
  class Impl;
  unique_ptr<Impl> m_impl;
};
}  // namespace Utility
}  // namespace DockerClientpp

#endif /* DOCKER_CLIENT_PP_ARCHIVE_H */

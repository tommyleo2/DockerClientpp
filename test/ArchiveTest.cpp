#include <fstream>
#include "gtest/gtest.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "Archive.hpp"

#include "Socket.hpp"

TEST(ArchiveTest, CompressTest) {
  std::fstream fs("1", std::fstream::out);
  fs << 1 << std::endl;
  fs.close();
  fs.open("2", std::fstream::out);
  fs << 2 << std::endl;
  fs.close();

  int fd = open("test.tar", O_TRUNC | O_WRONLY, 0644);

  DockerClientpp::Utility::Archive ac;
  ac.addFiles({"1", "2"});
  ac.writeToFd(fd);

  close(fd);

  std::remove("1");
  std::remove("2");

  // DockerClientpp::Socket s(DockerClientpp::UNIX, "/var/run/docker.sock");
}

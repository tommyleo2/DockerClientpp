#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fstream>
#include <streambuf>

#include "Archive.hpp"
#include "Socket.hpp"
#include "gtest/gtest.h"

TEST(ArchiveTest, GetTarTest) {
  std::fstream fs("1", std::fstream::out);
  fs << 1 << std::endl;
  fs.close();
  fs.open("2", std::fstream::out);
  fs << 2 << std::endl;
  fs.close();

  DockerClientpp::Utility::Archive ac;
  ac.addFiles({"1", "2"});
  std::fstream out_file("test.tar", std::fstream::out);
  std::string tar = ac.getTar();
  out_file << tar;

  std::remove("1");
  std::remove("2");

  std::system("tar axf test.tar");
  std::fstream test_1("./1");
  std::string content;
  test_1 >> content;
  EXPECT_EQ("1", content);
  std::fstream test_2("./2");
  test_2 >> content;
  EXPECT_EQ("2", content);

  std::remove("1");
  std::remove("2");
  std::remove("test.tar");
}

TEST(ArchiveTest, WriteToFdTest) {
  std::fstream fs("1", std::fstream::out);
  fs << 1 << std::endl;
  fs.close();
  fs.open("2", std::fstream::out);
  fs << 2 << std::endl;
  fs.close();

  DockerClientpp::Utility::Archive ac;
  ac.addFiles({"1", "2"});
  int fd = ::creat("test.tar", 0644);
  ac.writeToFd(fd);

  std::remove("1");
  std::remove("2");

  std::system("tar axf test.tar");
  std::fstream test_1("./1");
  std::string content;
  test_1 >> content;
  EXPECT_EQ("1", content);
  std::fstream test_2("./2");
  test_2 >> content;
  EXPECT_EQ("2", content);

  std::remove("1");
  std::remove("2");
  std::remove("test.tar");
}

TEST(ArchiveTest, ExtractTest) {
  std::system("mkdir test_archive");
  std::system("echo 1 >> test_archive/1");
  std::system("echo 2 >> test_archive/2");
  std::system("tar -acf test_archive.tar test_archive");
  std::system("rm -r test_archive");
  std::system("mkdir test");

  std::fstream fs("test_archive.tar", std::fstream::in);
  std::string buffer;
  fs.seekg(0, std::fstream::end);
  buffer.reserve(fs.tellg());
  fs.seekg(0, std::fstream::beg);
  buffer.assign((std::istreambuf_iterator<char>(fs)),
                std::istreambuf_iterator<char>());

  DockerClientpp::Utility::Archive::extractTar(buffer, "./test");
  std::fstream test_1("./test/test_archive/1");
  std::string content;
  test_1 >> content;
  EXPECT_EQ("1", content);
  std::fstream test_2("./test/test_archive/2");
  test_2 >> content;
  EXPECT_EQ("2", content);

  std::system("rm test test_archive.tar -r");
}

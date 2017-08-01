#include "gtest/gtest.h"

#include "DockerClient.hpp"
#include "types.hpp"

#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace nlohmann;
using namespace DockerClientpp;

class LaunchTest : public ::testing::Test {
protected:
  virtual void SetUp() override {
    std::system("docker rm -f test > /dev/null 2>&1");
  }
  virtual void TearDown() override {
    std::system("docker start test > /dev/null 2>&1");
  }
  void test(DockerClient &dc) {
    string id;
    id = dc.createContainer(CreateContainer::AttachStdout(true),
                            CreateContainer::AttachStderr(true),
                            CreateContainer::Tty(true),
                            CreateContainer::StopSignal("SIGKILL"),
                            CreateContainer::Image("busybox:1.26"),
                            CreateContainer::Name("test"));
    ASSERT_FALSE(id.empty()) << id;

    dc.startContainer("test");
    ASSERT_TRUE(std::system(("docker ps -q --no-trunc | grep " + id + " > /dev/null 2>&1").c_str()) == 0);

    dc.stopContainer("test");
    ASSERT_FALSE(std::system(("docker ps -q --no-trunc | grep " + id + " > /dev/null 2>&1").c_str()) == 0);
  }
};

TEST_F(LaunchTest, UnixSocketTest) {
  DockerClient dc(DockerClientpp::UNIX);
  test(dc);
}

// TEST_F(LaunchTest, TCPSocketTest) {
//   DockerClient dc(DockerClientpp::TCP, "127.0.0.1:8888");
//   test(dc);
// }

TEST(ExecTest, CreateExecTest) {
  DockerClient dc;//(TCP, "127.0.0.1:8888");
  string id;
  for (int i = 0; i < 5; i++) {
    id = dc.createExecution("test",
                            CreateExecution::AttachStdout(true),
                            CreateExecution::AttachStderr(true),
                            CreateExecution::Tty(false),
                            CreateExecution::Cmd({"echo", "1"}));
    ASSERT_FALSE(id.empty()) << id;
    auto output = dc.startExecution(id,
                                    StartExecution::Detach(false),
                                    StartExecution::Tty(false));
    EXPECT_EQ("1\n", output);
  }
  //std::system("docker ps -a");
}

TEST(ExecTest, InspectTest) {
  DockerClient dc;
  string id;
  id = dc.createExecution("test",
                          CreateExecution::AttachStdout(true),
                          CreateExecution::AttachStderr(true),
                          CreateExecution::Tty(false),
                          CreateExecution::Cmd({"echo", "1"}));
  ASSERT_FALSE(id.empty()) << id;
  dc.startExecution(id,
                    StartExecution::Detach(false),
                    StartExecution::Tty(false));
  json status = json::parse(dc.inspectExecution(id));
  EXPECT_EQ(0, status["ExitCode"].get<int>());
}

TEST(ExecTest, PutFileTest) {
  DockerClient dc;//(TCP, "127.0.0.1:8888");
  string id;
  std::fstream fs("1", std::fstream::out);
  fs << 1 << std::endl;
  fs.close();
  fs.open("2", std::fstream::out);
  fs << 2 << std::endl;
  fs.close();
  fs.open("3", std::fstream::out);
  fs << 3 << std::endl;
  fs.close();

  dc.putFiles("test", {"1", "2"}, "/tmp");

  id = dc.createExecution("test",
                          CreateExecution::AttachStdout(true),
                          CreateExecution::AttachStderr(true),
                          CreateExecution::Tty(false),
                          CreateExecution::Cmd({"ls", "/tmp"}));
  ASSERT_FALSE(id.empty()) << id;
  auto output = dc.startExecution(id,
                                  StartExecution::Detach(false),
                                  StartExecution::Tty(false));
  EXPECT_EQ("1\n2\n", output);

  dc.putFiles("test", {"1", "2", "3"}, "/tmp");

  id = dc.createExecution("test",
                          CreateExecution::AttachStdout(true),
                          CreateExecution::AttachStderr(true),
                          CreateExecution::Tty(false),
                          CreateExecution::Cmd({"ls", "/tmp"}));
  ASSERT_FALSE(id.empty()) << id;
  output = dc.startExecution(id,
                             StartExecution::Detach(false),
                             StartExecution::Tty(false));
  EXPECT_EQ("1\n2\n3\n", output);

  std::remove("1");
  std::remove("2");
  std::remove("3");
}

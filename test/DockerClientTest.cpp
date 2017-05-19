#include <gtest/gtest.h>

#include "DockerClient.hpp"
#include "types.hpp"

#include <iostream>
#include <cstdlib>

using namespace nlohmann;
using namespace DockerClientpp;

class LaunchTest : public ::testing::Test {
protected:
  virtual void TearDown() override {
    std::system("docker rm -f test > /dev/null 2>&1");
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

    dc.stopContainer("test");
  }
};

class ExecTest : public LaunchTest {
protected:
  virtual void SetUp() override {
    if (std::system("docker run -d -t --name test busybox:1.26 > /dev/null")) {
      throw std::runtime_error("Cannot run busybox:1.26");
    }
  }
};

TEST_F(LaunchTest, UnixSocketTest) {
  DockerClient dc(DockerClientpp::UNIX);
  test(dc);
}

TEST_F(LaunchTest, TCPSocketTest) {
  //DockerClient dc(DockerClientpp::TCP, "127.0.0.1:8888");
  DockerClient dc;
  test(dc);
}

TEST_F(ExecTest, CreateTest) {
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

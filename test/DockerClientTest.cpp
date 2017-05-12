#include <gtest/gtest.h>

#include "DockerClient.hpp"
#include "CreateContainerOptions.hpp"

#include <iostream>

using namespace nlohmann;
using namespace DockerClientpp;

TEST(DockerClient, creatContainerTest) {
  DockerClient dc(DockerClientpp::UNIX);
  string id = dc.createContainer(CreateContainer::AttachStdout(true),
                                 CreateContainer::AttachStderr(true),
                                 CreateContainer::Image("busybox"),
                                 CreateContainer::Name("test"));
  ASSERT_FALSE(id.empty()) << id;
}

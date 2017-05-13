#include <gtest/gtest.h>


class GlobalEnv: public ::testing::Environment {
public:
  virtual void SetUp() {
    // if (std::system("docker pull busybox:1.26")) {
    //   throw std::runtime_error("Cannot pull busybox");
    // }
  }
  virtual void TearDown() {
    std::system("docker rm -f test > /dev/null 2>&1");
  }
};

int main(int argc, char *argv[]) {
  AddGlobalTestEnvironment(new GlobalEnv);
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

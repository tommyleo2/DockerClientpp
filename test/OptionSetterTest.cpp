#include <gtest/gtest.h>

#include "OptionSetter.hpp"
#include "CreateContainerOptions.hpp"

#include <iostream>

using namespace nlohmann;

TEST(OptionSetter, setTest) {
  DockerClientpp::OptionSetter op;
  json expect {
    {"AttachStdout", true},
      {"AttachStderr", true},
        {"Tty", true},
          {"Cmd", {"ls", "-l"}},
            {"WorkingDir", "/tmp"},
              {"Image", "sandbox10:libsanbox"}
  };
  op.set(DockerClientpp::AttachStdout(true));
  op.set(DockerClientpp::AttachStderr(true));
  op.set(DockerClientpp::Tty(true));
  op.set(DockerClientpp::Cmd({"ls", "-l"}));
  op.set(DockerClientpp::WorkingDir("/tmp"));
  op.set(DockerClientpp::Image("sandbox10:libsanbox"));
  json test = json::parse(op.dump());

  ASSERT_EQ(expect.size(), test.size());
  for (auto it = expect.begin(); it != expect.end(); it++) {
    ASSERT_TRUE(test.find(it.key()) != test.end());
    if (it.key() == "Cmd") {
      ASSERT_EQ(expect["Cmd"].size(), test["Cmd"].size());
      for (auto &item: expect["Cmd"]) {
        ASSERT_TRUE(std::find(test["Cmd"].begin(), test["Cmd"].end(), item) != test["Cmd"].end());
      }
      continue;
    }
    ASSERT_EQ(it.value(), test[it.key()]);
  }
}

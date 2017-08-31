#include "SimpleHttpClient.hpp"
#include "gtest/gtest.h"

using namespace DockerClientpp::Http;

class IOTest : public ::testing::Test {
 protected:
  IOTest();
  SimpleHttpClient unix_client;
  SimpleHttpClient tcp_client;
  string uri;
  Header header;
  QueryParam query_param;

  void test(SimpleHttpClient &client) {
    for (int i = 0; i < 100; i++) {
      auto res = client.Get(uri, header, query_param);
      EXPECT_EQ(200, res->status_code);
    }
  }
};

IOTest::IOTest()
    : unix_client(DockerClientpp::UNIX, "/var/run/docker.sock"),
#ifdef CI_TEST
      tcp_client(DockerClientpp::UNIX, "/var/run/docker.sock"),
#else
      tcp_client(DockerClientpp::TCP, "127.0.0.1:8888"),
#endif
      uri("/images/json"),
      header{{"Content-Type", "application/json"},
             {"Host", "v1.24"},
             {"Accept", "*/*"}},
      query_param{{"all", "0"}} {
}

#ifndef CI_TEST

TEST_F(IOTest, TcpSocketTest) {
  test(tcp_client);
}

#endif

TEST_F(IOTest, UnixSocketTest) {
  test(unix_client);
}

#include <gtest/gtest.h>
#include "SimpleHttpClient.hpp"

using namespace DockerClientpp::Http;

class IOTest : public ::testing::Test {
protected:
  static SimpleHttpClient unix_client;
  static SimpleHttpClient tcp_client;
  static string uri;
  static Header header;
  static QueryParam query_param;

  void test(SimpleHttpClient &client) {
    for (int i = 0; i < 1000; i++) {
      auto res = client.Get(uri,
                            header,
                            query_param);
      EXPECT_EQ(200, res.status_code);
    }
  }
};

SimpleHttpClient IOTest::unix_client(DockerClientpp::UNIX, "/var/run/docker.sock");
SimpleHttpClient IOTest::tcp_client(DockerClientpp::TCP, "127.0.0.1:8888");
string IOTest::uri("/containers/json");
Header IOTest::header {
  {"Content-Type", "application/json"},
  {"Host", "v1.24"},
  {"Accept", "*/*"}
};
QueryParam IOTest::query_param {
  {"all", "1"}
};



TEST_F(IOTest, UnixSocketTest) {
  test(unix_client);
}

TEST_F(IOTest, TcpSocketTest) {
  test(tcp_client);
}

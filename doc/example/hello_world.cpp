#include "dockerclientpp/DockerClientpp.hpp"

#include <fstream>
#include <iostream>

using namespace DockerClientpp;
using std::cout;
using std::endl;

int main() {
  std::fstream fs("testfile", std::fstream::out);
  fs << "hello world\n";
  fs.close();

  // Use default socket, unix domain socket: /var/run/docker.sock
  DockerClient dc;
  // Use tcp socket
  // DockerClient dc(SOCK_TYPE::TCP, "ip:port");

  string id = dc.createContainer({{"AttachStdout", true},
                                  {"AttachStderr", true},
                                  {"Tty", true},
                                  {"StopSignal", "SIGKILL"},
                                  {"Image", "busybox:1.26"}},
                                 "test");

  dc.startContainer(id);

  dc.putFiles(id, {"testfile"}, "/tmp");

  string exec_id =
      dc.createExecution(id, {{"AttachStdout", true},
                              {"AttachStderr", true},
                              {"Tty", false},
                              {"Cmd", json::array({"cat", "/tmp/testfile"})}});

  string output = dc.startExecution(exec_id, {});
  cout << output;
  std::remove("testfile");

  dc.executeCommand(id, {"mv", "/tmp/testfile", "/tmp/test_file"});
  dc.getFile(id, "/tmp/test_file", ".");

  fs.open("test_file", std::fstream::in);
  string s;
  std::getline(fs, s);
  cout << s << endl;
  std::remove("test_file");

  dc.stopContainer(id);

  return 0;
}

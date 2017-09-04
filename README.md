# A Docker Client SDK for C++

DockerClientpp is a simple docker client sdk written in C++

**This library heavily relies on Linux platform**

## Quick Start

```c++

DockerClient dc  // Instantiate a client with default option: connect to /var/run/docker.sock

DockerClient dc(SOCK_TYPE::UNIX, "/path/to/docker/daemon/socket");

DockerClient dc(SOCK_TYPE::TCP, "IP:Port");

string container_id = dc.createContainer({
  {"AttachStdout", true},
  {"AttachStderr", true},
  {"Tty", true},
  {"StopSignal", "SIGKILL"},
  {"Image", "busybox:1.26"}
}, "test");

ExecRet result = dc.executeCommand(container_id, {"echo", "hello DockerClientpp"});

std::cout << result.ret_code << std::endl 
          << result.output << std::endl; 
```
Note: 
1. For more usage, see [hello_world.cpp](./doc/example/hello_world.cpp)
2. For complete API reference, see [header file](./include/DockerClient.hpp) or [documentation](https://codedocs.xyz/tommyleo2/DockerClientpp/classDockerClientpp_1_1DockerClient.html)
3. For configuration passed in docker client see [Docker Engine API](https://docs.docker.com/engine/api/v1.30/)

## Build & Install

1. `git clone --recursive https://github.com/tommyleo2/DockerClientpp.git`
2. `cd DockerClientpp`
3. `mkdir build`
4. `cd build`
5. `cmake ..`
6. `make install`(with root permission)

The library's name is `libDockerClientpp.a`
Use Headers `#include <dockerclientpp/DockerClientpp.hpp>` in your project

### doxygen support

After cmake configuration, execute `make doc`. The doc files will be put under `/<DockerClient root directory>/doc`

## Use in CMake Project

1. add subdirectory
2. link `DOCKER_CLIENT_PP_LIB` to your targets

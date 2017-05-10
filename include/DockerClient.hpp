#ifndef DOCKERCLIENT_H
#define DOCKERCLIENT_H

#include <iostream>
#include <vector>
#include <type_traits>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stddef.h>

#include "CreateContainerOptions.hpp"
#include "OptionSetter.hpp"
#include "types.hpp"
#include "defines.hpp"
#include "SimpleHttpClient.hpp"

namespace DockerClientpp {
  using std::string;
  using std::vector;
  using namespace __private;
  /**
   *  @brief Docker client class
   */
  class DockerClient {
    /**
     *  @brief Disallow copy
     */
    DockerClient(const DockerClient &) = delete;
    /**
     *  @brief Disallow copy
     */
    DockerClient &operator=(const DockerClient &) = delete;
    DockerClient(DockerClient &&) = default;
  public:
    /**
     *  @brief Constructor, create a socket file
     *  @param type socket type that docker daemon use
     *  @param path path to the docker daemon socket
     *         if type is TCP, path might be a IP to docker daemon server
     */
    DockerClient(SOCK_TYPE type = UNIX,
                 const string &path = "/var/run/docker.sock");

    /**
     *  @brief Create a new container based on existing image
     *
     *  This does not mean that you can pass multiple commands to Cmd.
     *  A valid command may look like this: `Cmd = {"ls", "-a", "-l"}`
     *  If you want to run multiple commands in one run, consider use shell script
     *  e.g. `Cmd = {"bash", "-c", "mkdir test && cd test"}`
     *
     *  @param ts All options derived from CreateContainerOption.
     *  @sa CreateContainerOption
     */
    template<typename... Ts>
    void createContainer(Ts &&...ts) {
      OptionSetter option;
      setOpt(option, FWD(ts)...);
      m_impl->createContainer(option);
    }

    /**
     *  @brief Stop a running container
     *  @param identifier Container's ID or name
     */
    // void stopContainer(const string &identifier);

    /**
     *  @brief Execute a command in a running container
     *
     *  For reference of parameters cmd see createContainer()
     *
     *  @param identifier Container's ID or name
     *  @param cmd Executing command with parameters in vector
     *  @return
     *  @sa createContainer()
     */
    // string executeCommand(const string &identifier,
    //                       const vector<string> cmd);

    // string createExecution();

    // string startExecution();

  private:
    template<typename T, typename... Ts>
    void setOpt(OptionSetter &option, T &&t, Ts &&...ts) {
      static_assert(std::is_base_of<CreateContainerOption, T>::value,
                    "Option is not CreateContainerOption");
      option.set(FWD(t));
      setOpt(option, FWD(ts)...);
    }

  private:
    class DockerClientImpl;
    std::unique_ptr<DockerClientImplBase> m_impl;
  };
}

#endif /* DOCKERCLIENT_H */

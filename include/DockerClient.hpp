#ifndef DOCKERCLIENT_H
#define DOCKERCLIENT_H

#include "SimpleHttpClient.hpp"

#include "Archive.hpp"
#include "Response.hpp"
#include "defines.hpp"

namespace DockerClientpp {

/**
 * @brief Docker client class
 */
class DockerClient {
  /**
   * @brief Disallow copy
   */
  DockerClient(const DockerClient &) = delete;
  /**
   * @brief Disallow copy
   */
  DockerClient &operator=(const DockerClient &) = delete;

 public:
  DockerClient(DockerClient &&) = default;

  ~DockerClient();

 public:
  /**
   * @brief Constructor, create a socket file
   * @param type socket type that docker daemon use
   * @param path path to the docker daemon socket
   *        if type is TCP, path might be a IP to docker daemon server
   */
  DockerClient(const SOCK_TYPE type = UNIX,
               const string &path = "/var/run/docker.sock");

  /**
   * @brief Set Docker daemon API version
   *
   * The default api version is v1.24
   *
   * @param api api version to be set. e.g. api = "v1.24"
   */
  void setAPIVersion(const string &api);

  /**
   * @brief List all images
   *
   * Test Only!
   *
   * @return Images list in json format
   */
  string listImages();

  /**
   * @brief Create a new container based on existing image
   *
   * This does not mean that you can pass multiple commands to Cmd.
   * A valid command may look like this: `Cmd = {"ls", "-a", "-l"}`
   * If you want to run multiple commands in one run, consider using shell
   * script
   * e.g. `Cmd = {"bash", "-c", "mkdir test && cd test"}`
   *
   * @param config configuration
   * @param name container's name
   * @return container's id
   * @sa CreateContainerOption
   */
  string createContainer(const json &config, const string &name = "");

  /**
   * @brief Start a stopped or created container
   * @param identifier Container's ID or name
   */
  void startContainer(const string &identifier);

  /**
   * @brief Stop a running container
   * @param identifier Container's ID or name
   */
  void stopContainer(const string &identifier);

  /**
   * @brief Set up an exec running instance in a running container
   *
   * The execution won't start until a start command is executed on it
   *
   * @param identifier Container's ID or name
   * @param config configuration
   * @return Execution ID, needed when start a execution
   */
  string createExecution(const string &identifier, const json &config);

  /**
   * @brief Start a execution instance that is set up previously
   *
   * The first byte of the return value indicates output type
   * 0: stdin
   * 1: stdout
   * 2: stderr
   *
   * @param id Execution instance ID
   * @param config configuration
   * @return if Detach is false, return output
   */
  string startExecution(const string &id, const json &config);

  /**
   * @brief Inspect a execution instance
   *
   * @param id Execution instance ID
   * @return Execution status
   */
  string inspectExecution(const string &id);

  /**
   * @brief Execute a command in a running container, like `docker exec`
   * command
   *
   * For reference of parameters cmd see createContainer()
   *
   * @param identifier Container's ID or name
   * @param cmd Executing command with parameters in vector
   * @return
   * @sa createContainer()
   */
  // template <typename... Ts>
  // string executeCommand(const string &identifier, Ts &&... ts) {}

  /**
   * @brief Put files to container
   *
   * @param identifier container's id or name
   * @param files files need to be put
   * @param path location in the container
   */
  void putFiles(const string &identifier, const vector<string> &files,
                const string &path);

 private:
  class Impl;
  unique_ptr<Impl> m_impl;
};
}

#endif /* DOCKERCLIENT_H */

#include "Archive.hpp"

#include "archive.h"
#include "archive_entry.h"

#include <fcntl.h>

namespace DockerClientpp {
  namespace Utility {
    class Archive::Impl {
    public:
      Impl(const vector<string> &files, bool reserve_path = false);
      ~Impl();
      void addFile(const string &file);
      void addFile(const vector<string> &files);
      void writeToFd(const int fd);
      void getTar(string &buffer);
    private:
      static la_ssize_t writeToBuffer(archive *a, void *client_data,
                                      const void *buff, size_t n);

      vector<string> m_files;
      bool reserve_path;
    };
  }
}

using namespace DockerClientpp::Utility;

Archive::Impl::Impl(const vector<string> &files, bool reserve_path) :
  m_files(files), reserve_path(reserve_path) { }

Archive::Impl::~Impl() { }

void Archive::Impl::addFile(const string &file) {
  m_files.push_back(file);
}
void Archive::Impl::addFile(const vector<string> &files) {
  m_files.insert(m_files.end(), files.begin(), files.end());
}

void Archive::Impl::writeToFd(const int fd) {
  archive *a;
  archive_entry *entry;
  struct stat st;
  char buff[8192];
  int len;

  a = archive_write_new();

  archive_write_set_format_pax_restricted(a);

  archive_write_open_fd(a, fd);
  for (const auto &file : m_files) {
    stat(file.c_str(), &st);
    entry = archive_entry_new();
    archive_entry_copy_stat(entry, &st);
    if (reserve_path) {
      archive_entry_set_pathname(entry, file.c_str());
    } else {
      auto pos = file.find_last_of('/');
      if (pos != std::string::npos) {
        string filename = file.substr(pos + 1);
        archive_entry_set_pathname(entry, filename.c_str());
      } else {
        archive_entry_set_pathname(entry, file.c_str());
      }
    }
    archive_write_header(a, entry);
    int file_fd;
    if ((file_fd = open(file.c_str(), O_RDONLY)) != -1) {
      len = read(file_fd, buff, sizeof(buff));
      while (len > 0) {
        archive_write_data(a, buff, len);
        len = read(file_fd, buff, sizeof(buff));
      }
      close(file_fd);
    }
    archive_entry_free(entry);
  }
  archive_write_free(a);
}

void Archive::Impl::getTar(string &buffer) {
  archive *a;
  archive_entry *entry;
  struct stat st;
  char buff[8192];
  int len;

  a = archive_write_new();

  archive_write_set_format_pax_restricted(a);

  archive_write_open(a, &buffer, nullptr, writeToBuffer, nullptr);
  for (const auto &file : m_files) {
    stat(file.c_str(), &st);
    entry = archive_entry_new();
    archive_entry_copy_stat(entry, &st);
    if (reserve_path) {
      archive_entry_set_pathname(entry, file.c_str());
    } else {
      auto pos = file.find_last_of('/');
      if (pos != std::string::npos) {
        string filename = file.substr(pos + 1);
        archive_entry_set_pathname(entry, filename.c_str());
      } else {
        archive_entry_set_pathname(entry, file.c_str());
      }
    }
    archive_write_header(a, entry);
    int file_fd;
    if ((file_fd = open(file.c_str(), O_RDONLY)) != -1) {
      len = read(file_fd, buff, sizeof(buff));
      while (len > 0) {
        archive_write_data(a, buff, len);
        len = read(file_fd, buff, sizeof(buff));
      }
      close(file_fd);
    }
    archive_entry_free(entry);
  }
  archive_write_free(a);
}

la_ssize_t Archive::Impl::writeToBuffer(archive *, void *client_data,
                                        const void *buff, size_t n) {
  string *to_buffer = reinterpret_cast<string *>(client_data);
  const char *from_buffer = reinterpret_cast<const char *>(buff);
  to_buffer->append(from_buffer, n);
  return n;
}


//-------------------------Archive Implementation-------------------------//

Archive::Archive(const vector<string> &files, bool reserve_path) :
  m_impl(new Impl(files, reserve_path)) { }

Archive::~Archive() { }

void Archive::writeToFd(const int fd) {
  m_impl->writeToFd(fd);
}

void Archive::getTar(string &buffer) {
  m_impl->getTar(buffer);
}

void Archive::addFile(const string &file) {
  m_impl->addFile(file);
}

void Archive::addFile(const vector<string> &files) {
  m_impl->addFile(files);
}

#include "Archive.hpp"

#include "archive.h"
#include "archive_entry.h"

#include <fcntl.h>

namespace DockerClientpp {
namespace Utility {
class Archive::Impl {
 public:
  Impl();
  ~Impl();
  void addFile(const string &file);
  void addFiles(const vector<string> &files);
  void writeToFd(const int fd, bool reserve_path);
  string getTar(bool reserve_path);
  static void extractTar(const string &tar_buffer, const string &path);

 private:
  static la_ssize_t writeToBuffer(archive *a, void *client_data,
                                  const void *buff, size_t n);
  static int writeContentToDisk(archive *a, archive *disk);

  vector<string> m_files;
};
}
}

using namespace DockerClientpp::Utility;

Archive::Impl::Impl() {}

Archive::Impl::~Impl() {}

void Archive::Impl::addFile(const string &file) {
  m_files.push_back(file);
}
void Archive::Impl::addFiles(const vector<string> &files) {
  m_files.insert(m_files.end(), files.begin(), files.end());
}

void Archive::Impl::writeToFd(const int fd, bool reserve_path) {
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

DockerClientpp::string Archive::Impl::getTar(bool reserve_path) {
  archive *a;
  archive_entry *entry;
  struct stat st;
  char buff[8192];
  int len;
  string buffer;

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
  return buffer;
}

la_ssize_t Archive::Impl::writeToBuffer(archive *, void *client_data,
                                        const void *buff, size_t n) {
  string *to_buffer = reinterpret_cast<string *>(client_data);
  const char *from_buffer = reinterpret_cast<const char *>(buff);
  to_buffer->append(from_buffer, n);
  return n;
}

void Archive::Impl::extractTar(const string &tar_buffer, const string &path) {
  archive *a;
  archive *ext;
  archive_entry *entry;
  int flags = ARCHIVE_EXTRACT_TIME | ARCHIVE_EXTRACT_PERM |
              ARCHIVE_EXTRACT_ACL | ARCHIVE_EXTRACT_FFLAGS;
  int ret;
  a = archive_read_new();
  archive_read_support_format_all(a);
  // archive_read_support_compression_all(a);
  ext = archive_write_disk_new();
  archive_write_disk_set_options(ext, flags);
  archive_write_disk_set_standard_lookup(ext);

  archive_read_open_memory(a, tar_buffer.c_str(), tar_buffer.size());
  for (;;) {
    ret = archive_read_next_header(a, &entry);
    if (ret == ARCHIVE_EOF) break;
    archive_entry_set_pathname(
        entry, (path + "/" + archive_entry_pathname(entry)).c_str());
    archive_write_header(ext, entry);
    if (archive_entry_size(entry) > 0) {
      writeContentToDisk(a, ext);
    }
    archive_write_finish_entry(ext);
  }
  archive_read_close(a);
  archive_read_free(a);
  archive_write_close(ext);
  archive_write_free(ext);
}

int Archive::Impl::writeContentToDisk(archive *a, archive *disk) {
  int ret;
  const void *buff;
  size_t size;
  la_int64_t offset;
  while (true) {
    ret = archive_read_data_block(a, &buff, &size, &offset);
    if (ret == ARCHIVE_EOF) {
      return ARCHIVE_OK;
    }
    archive_write_data_block(disk, buff, size, offset);
  }
}

//-------------------------Archive Implementation-------------------------//

Archive::Archive() : m_impl(new Impl()) {}

Archive::~Archive() {}

void Archive::writeToFd(const int fd, bool reserve_path) {
  m_impl->writeToFd(fd, reserve_path);
}

DockerClientpp::string Archive::getTar(bool reserve_path) {
  return m_impl->getTar(reserve_path);
}

void Archive::addFile(const string &file) {
  m_impl->addFile(file);
}

void Archive::addFiles(const vector<string> &files) {
  m_impl->addFiles(files);
}

void Archive::extractTar(const string &tar_buffer, const string &path) {
  Impl::extractTar(tar_buffer, path);
}

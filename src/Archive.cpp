#include "Archive.hpp"

#include "archive.h"
#include "archive_entry.h"

#include <dirent.h>
#include <fcntl.h>

namespace DockerClientpp {
namespace Utility {
class Archive::Impl {
 public:
  Impl();
  ~Impl();
  void addFile(const string &file);
  void addFiles(const vector<string> &files);
  void writeToFd(const int fd);
  string getTar();
  static void extractTar(const string &tar_buffer, const string &path);

 private:
  void writeEntry(archive *a, const string &file_name, const string &file_path);

  static la_ssize_t writeToBuffer(archive *a, void *client_data,
                                  const void *buff, size_t n);
  static int writeContentToDisk(archive *a, archive *disk);

  vector<string> m_files;
};
}  // namespace Utility
}  // namespace DockerClientpp

using namespace DockerClientpp::Utility;

Archive::Impl::Impl() {}

Archive::Impl::~Impl() {}

void Archive::Impl::addFile(const string &file) {
  m_files.push_back(file);
}
void Archive::Impl::addFiles(const vector<string> &files) {
  m_files.insert(m_files.end(), files.begin(), files.end());
}

void Archive::Impl::writeToFd(const int fd) {
  archive *a;

  a = archive_write_new();

  archive_write_set_format_pax_restricted(a);

  archive_write_open_fd(a, fd);
  for (const auto &file : m_files) {
    string file_name;
    auto pos = file.find_last_of('/');
    if (pos != std::string::npos) {
      file_name = file.substr(pos + 1);
    } else {
      file_name = file;
    }
    writeEntry(a, file_name, file);
  }
  archive_write_free(a);
}

DockerClientpp::string Archive::Impl::getTar() {
  archive *a;
  string buffer;

  a = archive_write_new();

  archive_write_set_format_pax_restricted(a);

  archive_write_open(a, &buffer, nullptr, writeToBuffer, nullptr);
  for (const auto &file : m_files) {
    string file_name;
    auto pos = file.find_last_of('/');
    if (pos != std::string::npos) {
      file_name = file.substr(pos + 1);
    } else {
      file_name = file;
    }
    writeEntry(a, file_name, file);
  }
  archive_write_free(a);
  return buffer;
}

void Archive::Impl::writeEntry(archive *a,
                               const DockerClientpp::string &file_name,
                               const DockerClientpp::string &file_path) {
  struct stat st;
  stat(file_path.c_str(), &st);

  struct archive_entry *entry;
  entry = archive_entry_new();
  archive_entry_copy_stat(entry, &st);

  archive_entry_set_pathname(entry, file_name.c_str());

  archive_write_header(a, entry);
  int file_fd;
  char buff[8192];
  if ((file_fd = open(file_path.c_str(), O_RDONLY)) != -1) {
    int len = 0;
    while ((len = read(file_fd, buff, sizeof(buff))) > 0) {
      archive_write_data(a, buff, len);
    }
    close(file_fd);
  }
  archive_entry_free(entry);

  if (S_ISDIR(st.st_mode)) {
    DIR *dir;
    dir = opendir(file_path.c_str());
    struct dirent *entry;
    while ((entry = readdir(dir))) {
      if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
        continue;
      }
      writeEntry(a, file_name + "/" + entry->d_name,
                 file_path + "/" + entry->d_name);
    }
  }
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

void Archive::writeToFd(const int fd) {
  m_impl->writeToFd(fd);
}

DockerClientpp::string Archive::getTar() {
  return m_impl->getTar();
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

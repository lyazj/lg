#pragma once

#include "Global.h"

class FileMap {
public:
  FileMap(const fs::path &path);
  ~FileMap();
  FileMap(const FileMap &) = delete;
  FileMap &operator=(const FileMap &) = delete;

  const char *data() const { return buffer; }
  size_t size() const { return bufferSize; }

private:
  char *buffer = nullptr;
  size_t bufferSize = 0;
#ifdef _WIN32
  void *fileHandle = nullptr;
  void *mappingHandle = nullptr;
#endif
};

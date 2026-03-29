#include "FileMap.h"

// The Windows version is written by ChatGPT.
#ifdef _WIN32
#include <windows.h>
#else
#include <err.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

FileMap::FileMap(const fs::path &path)
{
#ifdef _WIN32
  HANDLE f = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
  if(f == INVALID_HANDLE_VALUE) abort();

  LARGE_INTEGER s;
  if(!GetFileSizeEx(f, &s)) abort();
  bufferSize = (size_t)s.QuadPart;

  HANDLE m = CreateFileMappingA(f, 0, PAGE_READONLY, 0, 0, 0);
  if(!m) abort();
  buffer = (char *)MapViewOfFile(m, FILE_MAP_READ, 0, 0, 0);
  if(!buffer) abort();

  fileHandle = f;
  mappingHandle = m;
#else
  int fd = open(path.c_str(), O_RDONLY);
  if(fd < 0) err(EXIT_FAILURE, "open");

  struct stat st;
  if(fstat(fd, &st) < 0) err(EXIT_FAILURE, "fstat");
  bufferSize = st.st_size;

  buffer = (char *)mmap(0, bufferSize, PROT_READ, MAP_SHARED, fd, 0);
  if(buffer == MAP_FAILED) err(EXIT_FAILURE, "mmap");

  close(fd);
#endif
}

FileMap::~FileMap()
{
#ifdef _WIN32
  if(buffer) UnmapViewOfFile(buffer);
  if(mappingHandle) CloseHandle(mappingHandle);
  if(fileHandle) CloseHandle(fileHandle);
#else
  munmap(buffer, bufferSize);
#endif
}

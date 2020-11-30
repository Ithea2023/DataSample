#include "logging.h"

#include <chrono>
#include <ostream>
#include <iomanip>
#include <iostream>
#include <Windows.h>

const  bool Win_Thread = true;
const  bool Time_Enable = true;
typedef DWORD PlatformThreadId;

const char* FilenameFromPath(const char* file) {
  const char* end1 = ::strrchr(file, '/');
  const char* end2 = ::strrchr(file, '\\');
  if (!end1 && !end2)
    return file;
  else
    return (end1 > end2) ? end1 + 1 : end2 + 1;
}

LogStream::LogStream(const char * file_name, const int line):file_name_(file_name),line_(line)
{
  if(Win_Thread) {
    PlatformThreadId id = GetCurrentThreadId();
    print_stream_ << "" << std::dec << id << ": ";
  }
}

LogStream::~LogStream()
{
  print_stream_ << " (" << FilenameFromPath(file_name_) << ":" << line_ << ") ";
  std::cout << "\n" << print_stream_.str();
}

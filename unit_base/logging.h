#ifndef LOGGING_H_
#define LOGGING_H_
#include <sstream>
const char* FilenameFromPath(const char* file);

class LogStream {
public:
  LogStream() = default;
  LogStream(const char* file_name, const int line);
  ~LogStream();
  std::ostream& ostream() {
    return print_stream_;
  }


private:
  std::ostringstream print_stream_;
  const int line_;
  const char* file_name_;
  int thread_num_;
};

#define LOG \
  LogStream(__FILE__, __LINE__).ostream()


#endif

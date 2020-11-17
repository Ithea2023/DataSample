#include <iostream>
#include "sstream"
#include "../unit_base/logging.h"

#define DEFINE_FILE_NAME(x) namespace  {static const char* __FILENAME__ = x;}
void filname_test() {
  std::stringstream ss;
  ss << " __FILE__ " << FilenameFromPath(__FILE__) << " __LINE__ " << __LINE__;
  std::cout << " ss: " << ss.str();
}

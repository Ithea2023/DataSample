#include "common_code.h"
#include <sstream>
#include <unordered_map>
#include <array>
// void check_bps() {
//   std::unorder_map<uint32_t, >
// }
//
//
//

class HungerSingleClass {
public:
  static HungerSingleClass& getInstance(void) {
    return instance_;
  }

private:
  HungerSingleClass(){}
  static HungerSingleClass instance_;
};

void common_main() {
  HungerSingleClass& a1 = HungerSingleClass::getInstance();
}


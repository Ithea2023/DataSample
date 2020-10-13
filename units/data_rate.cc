#include "data_rate.h"
#include <string>

#include <sstream>

namespace datasample{
std::string datasample::ToString(DataRate value) {
  std::stringstream ss;
  if (value.IsPlusInfinity()) {
    ss << "+inf bps";
  }
  else if (value.IsMinusInfinity()) {
    ss << "-inf bps";
  }
  else {
    if (value.bps() == 0 || value.bps() % 1000 != 0) {
      ss << value.bps() << " bps";
    }
    else {
      ss << value.kbps() << " kbps";
    }
  }
  return ss.str();
}
}
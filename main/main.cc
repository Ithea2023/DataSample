#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#include "data_rate.h"
#include "common_code.h"
#include "logging.h"
#include <limits>

#define TODEFINE(x) x << ", "
#define VALUE_NAME(x) #x

template<class T>
class Vector {
public:
  Vector() = default;

  ~Vector();

  void push(T const&);

  void pop();

  std::vector<T>* get_ptr();

private:
  std::vector<T> vector_;
};

template<class T>
Vector<T>::~Vector() = default;

template<class T>
void Vector<T>::push(T const& element) {
  vector_.push_back(element);
}

template<class T>
void Vector<T>::pop() {
  vector_.pop_back();
}

template<class T>
std::vector<T>* Vector<T>::get_ptr() {
  return &vector_;
}

class Counter {
public:
  std::vector<int> count_;

  void
  Reset() {
    *this = {};
  }
};

void counter_func(Counter* counter) {
  // std::unique_ptr<Counter> counter(new Counter);

  for (int i = 0; i < 3000; i++) {
    counter->count_.push_back(i);

  }
  LOG << "Ming: counter.size: " << counter->count_.size();
  counter->Reset();
  LOG << "Ming: counter.size: " << counter->count_.size();
}

void size_check(std::vector<int>* ptr) {
  LOG << " prt_size: " << ptr->size() << " ptr[0] " << (*ptr)[0];
}

struct ssrcPair {
  ssrcPair(uint32_t ssrc, int mbps) {
    this->ssrc = ssrc;
    this->mbps = mbps;
  }

  ssrcPair() = default;

  uint32_t ssrc = 0;
  int mbps = 0;
};

void map() {
  std::map<int, int> map_var;
  map_var[123] = 123;
  map_var.emplace(123, 223);
  std::stringstream ss;
  for (auto it : map_var) {
    ss << " [" << it.first << ", " << it.second << "],";
  }
  LOG << "Ming: map res: " << ss.str() << map_var.at(123);
}

int main() {
  datasample::DataRate data = datasample::DataRate::KilobitsPerSec(1000);
  LOG << "Ming: data: " << std::sqrt(data.kbps()) * std::sqrt(data.kbps()) << " " << TODEFINE(data.bps()) << TODEFINE(data.bps_or(0))
    << TODEFINE(data.bytes_per_sec()) << TODEFINE(data.kbps_or(0)) << TODEFINE(data.IsPlusInfinity());

  std::vector<int> vec(3, 3);
  LOG << " vec: " << vec[0] << vec[2] << " " << vec.size();
  size_check(&vec);

  std::vector<ssrcPair> video_stream(3);
  LOG << " video_stream size: " << video_stream.size();
  video_stream[0] = {123, 180};
  video_stream[1] = {345, 360};
  video_stream[2] = {567, 720};
  std::stringstream ss;
  for (auto it : video_stream) {
    ss << "ssrc: " << it.ssrc << ", mbps: " << it.mbps << ", ";
  }
  LOG << ss.str();
  std::set<int> set_data;
  set_data.insert(1);
  set_data.insert(12);
  set_data.insert(13);

  auto it = set_data.rbegin();
  LOG << "log1: " << *it << " size: " << std::dec << (std::numeric_limits<int>::max());
  LOG << "Ming";

}

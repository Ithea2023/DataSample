#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#include "data_rate.h"
#include "common_code.h"
// #include "strings"

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
  std::cout << "Ming: counter.size: " << counter->count_.size() << std::endl;
  counter->Reset();
  std::cout << "Ming: counter.size: " << counter->count_.size() << std::endl;
}

void size_check(std::vector<int>* ptr) {
  std::cout << " prt_size: " << ptr->size() << " ptr[0] " << (*ptr)[0] << std::endl;
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
  std::stringstream ss;
  for (auto it : map_var) {
    ss << " [" << it.first << ", " << it.second << "],";
  }
  std::cout << ss.str() << map_var.at(123) << std::endl;
}

int main() {
  datasample::DataRate data = datasample::DataRate::KilobitsPerSec(1000);
  std::cout << "Ming: data: " << std::sqrt(data.kbps()) * std::sqrt(data.kbps()) << " " << TODEFINE(data.bps()) << TODEFINE(data.bps_or(0))
    << TODEFINE(data.bytes_per_sec()) << TODEFINE(data.kbps_or(0)) << TODEFINE(data.IsPlusInfinity()) << std::endl;


  std::vector<int> vec(3,3);
  std::cout << " vec: " << vec[0] << vec[2] << " " << vec.size();
  size_check(&vec);

  std::vector<ssrcPair> video_stream(3);
  std::cout << " video_stream size: " << video_stream.size() << std::endl;
  video_stream[0] = { 123, 180 };
  video_stream[1] = { 345, 360 };
  video_stream[2] = { 567, 720 };
  std::stringstream ss;
  for (auto it : video_stream) {
    ss << "ssrc: " << it.ssrc << ", mbps: " << it.mbps << ", ";
  }
  std::cout << ss.str() << std::endl;
  std::set<int> set_data;
  set_data.insert(1);
  set_data.insert(12);
  set_data.insert(13);

  auto it = set_data.rbegin();
  std::cout << " afgeg: " << *it << " size: " << static_cast<size_t>(std::numeric_limits<int>::max())/1000/1000 << std::endl;


  map();

  filname_test();
}

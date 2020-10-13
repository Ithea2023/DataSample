#include <iostream>
#include <set>
#include <string>
#include <vector>
#include "data_rate.h"

#define TODEFINE(x) x << ", "

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




int main() {
  datasample::DataRate data = datasample::DataRate::KilobitsPerSec(1000);
  std::cout << "Ming: data: " << std::sqrt(data.kbps()) * std::sqrt(data.kbps()) << " " << TODEFINE(data.bps()) << TODEFINE(data.bps_or(0))
    << TODEFINE(data.bytes_per_sec()) << TODEFINE(data.kbps_or(0)) << TODEFINE(data.IsPlusInfinity()) << std::endl;

}

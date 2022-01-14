#include <iostream>
#include "unit_base.h"
#include "main.h"

#include <algorithm>
#include <vector>
#include <set>
#include <unordered_map>
#include <functional>

using namespace std;

#define NAME_MS(var) #var << "=" << var << "ms"

template <typename T>
auto loopTestReturnCostTime = [](T count, function<void()> func)
{
  std::unique_ptr<Timer> timer;
  timer.reset(new Timer());
  timer->reset();
  for (T index = 0; index < count; ++index) {
    func();
  }
  return timer->elapsed_ms();
};

template <typename T>
auto vectorInsertCountTest(T count, bool initMemory = true)
{
  // vector insert test
  vector<T> vec;
  if (initMemory) {
    vec.reserve(count);
  }
  return loopTestReturnCostTime<T>(count, [&]()
  {
    vec.push_back(count);
  });
}

template <typename T>
auto ListInsertCheckSizeTest(T count, bool get_size = false)
{
  list<T> llist;
  volatile T size_count = 0;
  return loopTestReturnCostTime<T>(count, [&]()
  {
    llist.push_back(count);
    if (get_size) {
      size_count = llist.size();
    }
  });
}

int functionCall(int val)
{
  return val*2;
}

int functionOnTail(int val)
{
  return functionCall(val);
}

int functionNotOnTail(int val)
{
  auto it = functionCall(val);
  return it;
}

template <typename T>
auto FunctionEnableTailCallsTest(T count)
{
  double value = 0;
  return loopTestReturnCostTime<T>(count, [&]()
  {
      auto res = functionOnTail(count);
  });
}

template <typename T>
auto FunctionDisableTailCallsTest(T count)
{
  long long value = 0;
  auto time_res = loopTestReturnCostTime<T>(count, [&]()
  {
      auto res = functionNotOnTail(count);
  });
  return time_res;
}

void STLPerformanceTesting()
{
  const int LOOP_COUNT = 100 * 1000;
  const int REPEAT_TIME = 10;
  uint32_t performance_base_ms = 0;
  Timer timer;
  {
    LOG_MING << NAME_MS(vectorInsertCountTest(LOOP_COUNT*REPEAT_TIME, false));
    LOG_MING << NAME_MS(vectorInsertCountTest(LOOP_COUNT*REPEAT_TIME, true));
    LOG_MING << NAME_MS(ListInsertCheckSizeTest(LOOP_COUNT * REPEAT_TIME, true));
    LOG_MING << NAME_MS(ListInsertCheckSizeTest(LOOP_COUNT * REPEAT_TIME, false));
    LOG_MING << NAME_MS(FunctionEnableTailCallsTest(LOOP_COUNT*REPEAT_TIME));
    LOG_MING << NAME_MS(FunctionDisableTailCallsTest(LOOP_COUNT*REPEAT_TIME));
  }
}

int main()
{
  STLPerformanceTesting();

  return 0;
}

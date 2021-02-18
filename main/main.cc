#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#include "data_rate.h"
#include "common_code.h"
#include <algorithm>
#include "logging.h"
#include <limits>
#include <unordered_map>
#include <random>

void timer_test()
{
    Timer timer_test;
    timer_test.reset();
    uint32_t count = 10000000;

    std::vector<float> float_group;
    float_group.reserve(count);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(1, count);

    for (auto it : float_group)
    {
        it = distrib(gen);
    }

    LOG_MING << "time cost:" << timer_test.elapsed_ms() << "ms";
}




int main()
{
    LOG_MING << "main func test";
    timer_test();
    return 0;
}
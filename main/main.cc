#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#include "data_rate.h"
#include "common_code.h"
#include <algorithm>
#include <bitset>

#include "logging.h"
#include <limits>
#include <queue>
#include <unordered_map>
#include <random>
#include <unordered_set>
#include "fec_test.h"

class FriendTest;

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

void queue_test()
{
    std::queue<int> queque_group;
    queque_group.push(1);
    queque_group.push(2);
    queque_group.push(3);

    while (!queque_group.empty())
    {
        auto it = queque_group.front();
        queque_group.pop();
        LOG_MING << "queue: " << it;
    }

    if (queque_group.empty())
    {
        LOG_MING << "queue is empty";
    }
}

class SDivationCounter
{
public:
    void
    AddCounter(double val)
    {
        number_ += 1;
        double delta = val - mean_;
        mean_ += delta / number_;
        sd_ += (number_ - 1) / static_cast<double>(number_) * delta * delta;
    }

    double
    getSDivation() const
    {
        return sd_;
    }

    double
    getMean() const
    {
        return mean_;
    }

    uint32_t
    getNumer() const
    {
        return number_;
    }

private:

    double mean_ = 0;
    double sd_ = 0;
    uint32_t number_ = 0;
};

std::list<int64_t> sample_;
int64_t size_ = 0;
int64_t countLimit_ = 5;
int64_t mean_ = 0;
int64_t sd_ = 0;

void AddCounter(int64_t sample) {}

class basic
{
    friend FriendTest;
private:
    int i = 0;
};

class FriendTest
{
public:
    FriendTest() = delete;

    ~FriendTest() = default;

    FriendTest(int name)
    {
        name_ = name;
    }

    friend int
    printName(FriendTest test)
    {
        return test.name_;
    }

    int
    basic_name() const
    {
        return basic_.i;
    }

private:
    int name_;
    basic basic_;
};

int main()
{
    FriendTest test(300);
    std::cout << printName(test) << std::endl;
}

// int main()
// {
//     // ExampleFileUsage(10, 3);
//
//     muli_thread_test();
//
//     return 0;
// }

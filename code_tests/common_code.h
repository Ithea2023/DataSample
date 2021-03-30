#ifndef COMMON_CODE_H_
#define COMMON_CODE_H_

#include <chrono>
#include <deque>
#include <functional>
#include <queue>
#include <unordered_map>

class Timer
{
public:
    Timer()
        : beg_(clock_::now()) { }

    void
    reset()
    {
        beg_ = clock_::now();
    }

    double
    elapsed_ms() const
    {
        return std::chrono::duration_cast<second_>(clock_::now() - beg_).count() * 1000;
    }

    double
    elapsed_ns() const
    {
        return std::chrono::duration_cast<second_>(clock_::now() - beg_).count() * 10000000;
    }

private:
    typedef std::chrono::high_resolution_clock clock_;
    typedef std::chrono::duration<double, std::ratio<1>> second_;
    std::chrono::time_point<clock_> beg_;
};

class MaxQueue
{
    std::queue<int> q_history;
    std::deque<int> d;
public:
    MaxQueue() { }

    int
    max_value()
    {
        if (d.empty())
            return -1;
        return d.front();
    }

    void
    push_back(int value)
    {
        while (!d.empty() && d.back() < value)
        {
            d.pop_back();
        }
        d.push_back(value);
        q_history.push(value);
    }

    int
    pop_front()
    {
        if (q_history.empty())
            return -1;
        int ans = q_history.front();
        if (ans == d.front())
        {
            d.pop_front();
        }
        q_history.pop();
        return ans;
    }
};

constexpr int sample_fibonacci(const int n);

int func_main();

int lambda_main();

void future_thread_main();

void producer_consumer_problem();

void muli_thread_test();

void internal_test();

class HighFibonacci
{
public:
    typedef std::unordered_map<uint64_t, uint64_t>::iterator iter_;

    HighFibonacci()
    {
        history[1] = 1;
        history[2] = 1;
    }

    ~HighFibonacci() = default;

    const uint64_t
    default_run(const uint64_t n)
    {
        return n == 1 || n == 2 ? 1 : default_run(n - 1) + default_run(n - 2);
    }

    uint64_t
    run(const uint64_t n)
    {
        if (n == 1 || n == 2)
            return 1;

        uint64_t res_1 = 0;
        for (int i = 1; i < 3; ++i)
        {
            const iter_ it = history.find(n - i);
            if (
                it != history.end())
            {
                res_1 += it->second;
            }
            else
            {
                const uint64_t res = run(n - i);
                res_1 += res;
                history[n - i] = res;
            }
        }
        return res_1;
    }

    uint64_t
    run2(const uint64_t n)
    {
        if (n == 1 || n == 2)
            return 1;

        uint64_t res = 0;
        for (int i = 3; i < n + 1; ++i)
        {
            res = history.at(i - 1) + history.at(i - 2);
            history[i] = res;
        }
        return res;
    }

private:
    std::unordered_map<uint64_t, uint64_t> history;
};

class InternalStruct
{
    struct my_struct
    {
        int i = 0;
    };
    InternalStruct() = default;
    ~InternalStruct() = default;
    my_struct& struct_test();
};


#endif

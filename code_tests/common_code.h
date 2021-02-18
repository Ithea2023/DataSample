#ifndef COMMON_CODE_H_
#define COMMON_CODE_H_

#include <chrono>

class Timer
{
public:
    Timer() : beg_(clock_::now())
    {
    }
    void reset()
    {
        beg_ = clock_::now();
    }
    double elapsed_ms() const
    {
        return std::chrono::duration_cast<second_>(clock_::now() - beg_).count() * 1000;
    }
    double elapsed_ns() const
    {
        return std::chrono::duration_cast<second_>(clock_::now() - beg_).count() * 10000000;
    }

private:
    typedef std::chrono::high_resolution_clock clock_;
    typedef std::chrono::duration<double, std::ratio<1>> second_;
    std::chrono::time_point<clock_> beg_;
};



#endif
#pragma once
#include <sstream>
const char *FilenameFromPath(const char *file);

class Timer
{
public:
    Timer()
        : beg_(clock_::now()) {}

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

class LogStream
{
public:
    LogStream() = delete;
    LogStream(const char *file_name, const int line);
    LogStream(const char *file_name, const int line, const char *mark);
    ~LogStream();
    std::ostream &ostream()
    {
        return print_stream_;
    }

private:
    std::ostringstream print_stream_;
    const int line_;
    const char *file_name_;
    int thread_num_;
};

#define LOG \
    LogStream(__FILE__, __LINE__).ostream()

#define LOG_MING \
    LogStream(__FILE__, __LINE__, "Ming:").ostream()

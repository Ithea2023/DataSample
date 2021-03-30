#include "common_code.h"
#include <sstream>
#include <algorithm>
#include <functional>
#include <future>
#include <iostream>
#include <iterator>
#include <ppltasks.h>
#include <thread>
#include <condition_variable>

#include "../unit_base/logging.h"

constexpr int sample_fibonacci(const int n)
{
    return n == 1 || n == 2 ? 1 : sample_fibonacci(n - 1) + sample_fibonacci(n - 2);
}

int func_main()
{
    const int number = 30;
    uint64_t class_res = 0;
    Timer timer;
    HighFibonacci hisFibonacci;
    timer.reset();
    class_res = hisFibonacci.default_run(number);
    LOG_MING << "res:" << class_res << " class_finonacci: " << timer.elapsed_ms() << "ms";
    timer.reset();
    class_res = hisFibonacci.run(number);
    LOG_MING << "res:" << class_res << " class_finonacci: " << timer.elapsed_ms() << "ms";
    timer.reset();
    class_res = hisFibonacci.run2(number);
    LOG_MING << "res:" << class_res << " class_finonacci: " << timer.elapsed_ms() << "ms";

    return 0;
}

using foo = void(int);

void functional(foo f)
{
    f(1);
}

void reference(int& v)
{
    LOG_MING << "left";
}

void reference(int&& v)
{
    LOG_MING << "right";
}

template<typename T>
void pass(T&& v)
{
    LOG_MING << " common";
    reference(v);
}

int lambda_main()
{
    pass(1);

    int one = 1;
    pass(1);

    auto share_pointer = std::make_shared<int>(10);
    auto pointer2 = share_pointer;
    auto pointer3 = share_pointer;

    int* p = share_pointer.get();
    LOG_MING << "p1:" << share_pointer.use_count() << ", p2:" << pointer2.use_count() << ", p3:" << pointer3.use_count();

    pointer2.reset();
    LOG_MING << "p1:" << share_pointer.use_count() << ", p2:" << pointer2.use_count() << ", p3:" << pointer3.use_count();

    pointer2 = share_pointer;
    pointer3.reset();
    LOG_MING << "p1:" << share_pointer.use_count() << ", p2:" << pointer2.use_count() << ", p3:" << pointer3.use_count();

    auto unique_1 = std::make_unique<int>(10);
    auto unique_2 = new int(12);
    LOG_MING << "unique: " << *unique_1 << ", " << *unique_2;
    return 0;
}

void future_thread_main()
{
    std::packaged_task<int()> task([]
    {
        return 7;
    });
    std::future<int> result = task.get_future();
    std::thread(std::move(task)).detach();
    LOG_MING << "waiting...";
    result.wait();
    LOG_MING << "done! future result is:" << result.get();

    std::atomic<int64_t> a;
    LOG_MING << "is lock_free:" << a.is_lock_free();
}

void producer_consumer_problem()
{
    std::queue<int> produced_nums;
    std::mutex mutex;
    std::condition_variable condition_variable;
    bool notified = false;

    auto producer = [&]
    {
        for (int i = 0;; ++i)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(900));
            std::unique_lock<std::mutex> lock(mutex);
            LOG_MING << "producing " << i;
            produced_nums.push(i);
            notified = true;
            condition_variable.notify_all();
        }
    };

    auto consumer = [&]
    {
        while (true)
        {
            std::unique_lock<std::mutex> lock(mutex);
            while (!notified)
            {
                condition_variable.wait(lock);
            }
            lock.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            lock.lock();
            while (!produced_nums.empty())
            {
                LOG_MING << "consuming " << produced_nums.front();
                produced_nums.pop();
            }
            notified = false;
        }
    };

    std::thread producer_thread(producer);
    std::thread cs[2];
    for (int i = 0; i < 2; ++i)
    {
        cs[i] = std::thread(consumer);
    }
    producer_thread.join();
    for (int i = 0; i < 2; ++i)
    {
        cs[i].join();
    }
}



void muli_thread_test()
{
    std::queue<int> produced_nums;
    std::mutex mtx;
    std::condition_variable cv;
    bool notified = false;

    auto producer = [&]()
    {
        for(int i = 0; ;++i)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(900));
            std::unique_lock<std::mutex> lock(mtx);
            LOG_MING << "producing: " << i << ", size:" << produced_nums.size();
            produced_nums.push(i);
            notified = true;
            cv.notify_all();
        }
    };

    auto consumer = [&]()
    {
        while (true)
        {
            std::unique_lock<std::mutex> lock(mtx);
            while (!notified)
            {
                cv.wait(lock);
            }
            lock.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            lock.lock();
            while(!produced_nums.empty())
            {
                LOG_MING << "consumer: " << produced_nums.front();
                produced_nums.pop();
            }
            notified = false;
        }  
    };

    const int consumer_thread_num = 2;
    std::thread thread_p(producer);
    std::thread thread_co[consumer_thread_num];
    for(int i = 0; i< consumer_thread_num; ++i)
    {
        thread_co[i] = std::thread(consumer);
    }
    thread_p.join();
    for(int i = 0; i< consumer_thread_num; ++i)
    {
        thread_co[i].join();
    }
}

void internal_test()
{
    
}

InternalStruct::my_struct& InternalStruct::struct_test()
{
    my_struct myStruct;
    myStruct.i = 3000;
    return  myStruct;
}

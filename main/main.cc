#include <iostream>
#include "unit_base.h"
#include "main.h"
#include <vector>
#include <set>
#include <unordered_map>

using namespace std;

class Stack
{
    void Print(string const &)
    {
        std::unordered_map<int, bool> mm;
    }
    void Print2(string const &str)
    {
    }
};

class Solution
{
public:
    void revertVec(vector<int> &number, int startIndex, int endIndex)
    {
        if (startIndex > endIndex)
        {
            return;
        }
        const auto medium = (endIndex - startIndex) / 2;
        for (int index = 0; index <= medium; ++index)
        {
            const auto tmp = number[startIndex + index];
            number[startIndex + index] = number[endIndex - index];
            number[endIndex - index] = tmp;
        }
    }
    void rotate(vector<int> &nums, int k)
    {
        if (k == 0)
        {
            return;
        }
        revertVec(nums, 0, nums.size() - k - 1);
        revertVec(nums, nums.size() - k, nums.size() - 1);
        revertVec(nums, 0, nums.size() - 1);
    }
};

void leetcodeTest()
{
    Solution run;
    auto res = vector<int>{1, 2};
    run.rotate(res, 2);
    std::stringstream ss;
    std::for_each(res.begin(), res.end(), [&ss](int number) { ss << number << ", "; });
    LOG_MING << "leetcode: " << ss.str();
    std::set<int> _set;
    _set.insert(1);
}

int main()
{
    auto it3 = ::max3(23, -1.4);
    Timer timer;
    LOG_MING << "ns:" << timer.elapsed_ns();
    timer.reset();
    auto it = IsPrime<3>::value;
    LOG_MING << "ans:" << it << ", time_ms:" << timer.elapsed_ns();

    return 0;
}

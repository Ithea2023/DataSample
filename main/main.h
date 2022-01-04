#pragma once
#include <sstream>

template <typename T>
T max(T a, T b)
{
    return a > b ? a : b;
}

template <typename RT, typename T>

RT max2(T a, T b)
{
    return a > b ? a : b;
}

template <typename T, typename R>
auto max3(T a, R b) -> decltype(a > b ? a : b)
{
    return a > b ? a : b;
}

    template<unsigned p, unsigned d>
    struct DoIsPrime
    {
        static constexpr bool value = (p%d != 0) && DoIsPrime<p, d- 1>::value;
    };
    template<unsigned p>
    struct DoIsPrime<p,2>
    {
        static constexpr bool value = (p%2 != 0);
    };

    template<unsigned p>
    struct IsPrime
    {
        static constexpr bool value = DoIsPrime<p, p/2>::value;
    };
    template<>
    struct IsPrime<0>
    {
        static constexpr bool value = false;        
    };
    template<>
    struct IsPrime<1>
    {
        static constexpr bool value = false;
    };
    template<>
    struct IsPrime<2>
    {
        static constexpr bool value = false;
    };
    template<>
    struct IsPrime<3>
    {
        static constexpr bool value = false;
    };
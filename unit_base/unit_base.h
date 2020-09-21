#ifndef RTC_BASE_UNITS_UINT_BASE_H_
#define RTC_BASE_UNITS_UINT_BASE_H_

#include <stdint.h>
#include <algorithm>
#include <cmath>
#include <checks.h>

#include <type_traits>


namespace rtc_units_impl
{
  template <class Unit_T>
  class UnitBase
  {
  public:
    UnitBase() = delete;
    static constexpr Unit_T Zero() { return Unit_T(0); }
    static constexpr Unit_T PlusInfinity() { return Unit_T(PlusInfinityVal()); }
    static constexpr Unit_T MinusInfinity() { return Unit_T(MinusInfinityVal()); }

    constexpr bool IsZero() const { return value_ == 0; }
    constexpr bool IsFinite() const { return !IsInFinite(); }

    constexpr bool IsInFinite() const {
      return value_ == PlusInfinityVal() || value_ == MinusInfinityVal();
    }

    constexpr bool IsPlusInfinity() const { return value_ == PlusInfinityVal(); }
    //TODO need more code

  protected:
    // is int type
    template<typename T, typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
    static constexpr Unit_T FromValue(T value)
    {
      // if (Unit_T::one_sided)
      //   RTC_DCHECK_GE(value, 0);
      // RTC_DCHECK_GT(value, MinusInfinityVal());
      return Unit_T(static_cast<int64_t>(value));
    }
    // is float type, prevent float overflow the int64 type.
    template<typename T, typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr>
    static constexpr Unit_T FromValue(T value)
    {
      if(value == std::numeric_limits<T>::infinity())
      {
        return PlusInfinity();
      }else if(value == -std::numeric_limits<T>::infinity())
      {
        return MinusInfinity();
      }else
      {
        return FromValue(static_cast<int64_t>(value));
      }
    }

    // FromFraction
    template<typename T, typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
    static constexpr Unit_T FromFraction(int64_t denominator, T value)
    {
      return Unit_T(static_cast<int64_t>(denominator * value));
    }

    template<typename T, typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr>
    static constexpr Unit_T FromFraction(int64_t denominator, T value)
    {
      return FromValue(value * denominator);
    }

    // ToValue
    template<typename T = int64_t>
    constexpr typename std::enable_if<std::is_integral<T>::value>::type ToValue() const
    {
      return static_cast<T>(value_);
    }

    template<typename T>
    constexpr typename std::enable_if<std::is_floating_point<T>::value>::type ToValue() const
    {
      return IsPlusInfinity() ? std::numeric_limits<T>::infinity() :
      IsMinusInfinity() ? -std::numeric_limits<T>::infinity() : value_;
    }

    //ToValueOr
    template <typename T>
    constexpr T ToValueOr(T fallback_value) const
    {
      return IsFinite() ? value_ : fallback_value;
    }

    //ToFraction
    template <int64_t Denominator, typename T = int64_t>
    constexpr typename std::enable_if<std::is_integral<T>::value, T>::type ToFraction() const
    {
      if(Unit_T::one_sided)
      {
        return static_cast<T>(DivRoundPositionToNearest(value_, Denominator));
      }else
      {
        return static_cast<T>(DivRoundToNearset(value_, Denominator));
      }
    }
    template <int64_t Denominator, typename T>
    constexpr typename std::enable_if<std::is_floating_point<T>::value, T>::type ToFraction() const
    {
      return ToValue<T>() * (1 / static_cast<T>(Denominator));
    }

    //ToFractionOr
    template <int64_t >

  private:
    template <class RelativeUnit_T>
    friend class RelativeUnit;

    static constexpr int64_t PlusInfinityVal() {
      return std::numeric_limits<int64_t>::max();
    }

    static constexpr int64_t MinusInfinityVal() {
      return std::numeric_limits<int64_t>::min();
    }

    constexpr Unit_T& AsSubClassRef() {
      return static_cast<Unit_T&>(*this);
    }

    constexpr const Unit_T& AsSubClassRef() const {
      return static_cast<const Unit_T&>(*this);
    }

    static constexpr int64_t DivRoundPositionToNearest(int64_t n, int64_t d) {
      return (n + d / 2) / d;
    }

    static constexpr int64_t DivRoundToNearset(int64_t n, int64_t d) {
      return (n + (n > 0 ? d / 2 : -d / 2)) / d;
    }

    int64_t value_;
  };
}


#endif

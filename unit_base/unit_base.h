#ifndef RTC_BASE_UNITS_UINT_BASE_H_
#define RTC_BASE_UNITS_UINT_BASE_H_

#include <stdint.h>
#include <algorithm>
#include <cmath>

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

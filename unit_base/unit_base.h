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
    constexpr typename std::enable_if<std::is_integral<T>::value, T>::type ToValue() const
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
    template <int64_t Denominator>
    constexpr int64_t ToFractionOr(int64_t fallback_value) const {
        return IsFinite() ? Unit_T::one_sided ? DivRoundPositionToNearest(value_, Denominator) :
            DivRoundToNearset(value_, Denominator) : fallback_value;
    }

    template <int64_t Factor, typename T = int64_t>
    constexpr typename std::enable_if<std::is_integral<T>::value, T>::type
    ToMultiple() const {
        return static_cast<T>(ToValue() * Factor);
    }
    template <int64_t Factor, typename T>
    constexpr typename std::enable_if<std::is_floating_point<T>::value, T>::type
    ToMultiple() const {
        return ToValue<T>() * Factor;
    }

    explicit constexpr UnitBase(int64_t value) : value_(value){}

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

  template <class Unit_T>
  class RelativeUnit : public UnitBase<Unit_T>
  {
  public:
      constexpr Unit_T Clamped(Unit_T min_value, Unit_T max_value)const {
          return std::max(min_value, 
              std::min(UnitBase<Unit_T>::AsSubClassRef(), max_value));
      }
      constexpr void Clamp(Unit_T min_value, Unit_T max_value) {
          *this = Clamped(min_value, max_value);
      }
      constexpr Unit_T operator+(const Unit_T other) const {
          if(this->IsPlusInfinity() || other.IsPlusInfinity()) {
              return this->PlusInfinity();
          }else if(this->IsMinusInfinity() || other.IsMinusInfinity()) {
              return this->MinusInfinity();
          }
          return UnitBase<Unit_T>::FromValue(this->ToValue() + other.ToValue());
      }
      constexpr Unit_T operator-(const Unit_T other) const {
          if (this->IsPlusInfinity() || other.IsMinusInfinity()) {
              return this->PlusInfinity();
          }
          else if (this->IsMinusInfinity() || other.IsPlusInfinity()) {
              return this->MinusInfinity();
          }
          return UnitBase<Unit_T>::FromValue(this->ToValue() - other.ToValue());
      }
      constexpr Unit_T& operator+=(const Unit_T other) {
          *this = *this + other;
          return this->AsSubClassRef();
      }
      constexpr Unit_T& operator-=(const Unit_T other) {
          *this = *this - other;
          return this->AsSubClassRef();
      }
      constexpr double operator/(const Unit_T other) const {
          return UnitBase<Unit_T>::template ToValue<double>() / other.template ToValue<double>();
      }
      template <typename T>
      constexpr typename std::enable_if<std::is_arithmetic<T>::value, Unit_T>::type
      operator/(const T& scalar) const {
          return UnitBase<Unit_T>::FromValue(
              std::round(UnitBase<Unit_T>::template ToValue<int64_t>() / scalar));
      }
      constexpr Unit_T operator*(double scalar)const {
          return UnitBase<Unit_T>::FromValue(std::round(this->ToValue() * scalar));
      }
      constexpr Unit_T operator*(int64_t scalar)const {
          return UnitBase<Unit_T>::FromValue(this->ToValue() * scalar);
      }
      constexpr Unit_T operator*(int32_t scalar)const {
          return UnitBase<Unit_T>::FromValue(this->ToValue() * scalar);
      }
  protected:
      using UnitBase<Unit_T>::UnitBase;
  };

  template<class Unit_T>
  inline constexpr Unit_T operator*(double scalar, RelativeUnit<Unit_T> other) {
        return other * scalar;
  }
  template<class Unit_T>
  inline constexpr Unit_T operator*(int64_t scalar, RelativeUnit<Unit_T> other) {
      return other * scalar;
  }
  template<class Unit_T>
  inline constexpr Unit_T operator*(int32_t scalar, RelativeUnit<Unit_T> other) {
      return other * scalar;
  }
}

#endif
#pragma once
#include <iostream>
namespace DistMat
{
using Index = size_t;

template <typename Derived>
class MatrixBase {
private:
  Derived& derived() { return *static_cast<Derived*>(this); }
  const Derived& derived() const { return *static_cast<const Derived*>(this); }

  const Derived& const_derived() { return const_cast<const Derived&>(derived()); }
  const Derived& const_derived() const { return *static_cast<const Derived*>(this); }
public:

#define	EnsureOverwritten(F)	static_assert(!std::is_same<decltype(&Derived::F),\
        decltype(&MatrixBase<Derived>::F)>::value,\
        "static polymorphic function " #F " was not overwritten.");

  // Derived only need to define const version of accessWithoutBoundsChecking()
  auto& operator()(Index row, Index col)
  {
    return const_cast<typename Derived::value_type&>(
      const_derived().accessWithoutBoundsChecking(row, col)
    );
  }
  const auto& operator()(Index row, Index col) const
  {
    return const_derived().accessWithoutBoundsChecking(row, col);
  }
  auto& at(Index row, Index col)
  {
    return const_cast<typename Derived::value_type&>(
      const_derived().accessWithBoundsChecking(row, col)
    );
  }
  const auto& at(Index row, Index col) const
  {
    return const_derived().accessWithBoundsChecking(row, col);
  }

  template <typename Dest>
  void evalTo(Dest& dst) const { derived().evalTo_impl(dst); }

  template <typename Dest>
  void addTo(Dest& dst) const { derived().addTo_impl(dst); }

  template <typename Dest>
  void subTo(Dest& dst) const { derived().subTo_impl(dst); }

  template <typename Dest>
  void mulLeftTo(Dest& dst) const { derived().mulLeftTo_impl(dst); }

  template <typename Dest>
  void mulRightTo(Dest& dst) const { derived().mulRightTo_impl(dst); }

  // When a Base::f calls the Derived::f, but Derived::f is not defined,
  // compiler will not error, add EnsureOverwritten to avoid this.
  Index rows() const { EnsureOverwritten(rows); return derived().rows(); }
  Index cols() const { EnsureOverwritten(cols); return derived().cols(); }
  Index size() const { return rows() * cols(); }

  template <typename OtherDerived>
  MatrixBase<Derived>& operator=(const MatrixBase<OtherDerived>& other)
  {
    other.evalTo(derived());
  }

  template <typename OtherDerived>
  MatrixBase<Derived>& operator+=(const MatrixBase<OtherDerived>& other)
  {
    other.addTo(derived());
  }

  template <typename OtherDerived>
  MatrixBase<Derived>& operator-=(const MatrixBase<OtherDerived>& other)
  {
    other.subTo(derived());
  }
};

template <typename Dest, typename Derived>
Dest operator+(const Dest& lhs, const MatrixBase<Derived>& rhs)
{
  Dest tmp = lhs;
  rhs.addTo(tmp);
  return tmp;
}

template <typename Dest, typename Derived>
Dest operator-(const Dest& lhs, const MatrixBase<Derived>& rhs)
{
  Dest tmp = lhs;
  rhs.subTo(tmp);
  return tmp;
}

template <typename Dest, typename Derived>
Dest operator*(const Dest& lhs, const MatrixBase<Derived>& rhs)
{
  Dest tmp = lhs;
  rhs.mulRightTo(tmp);
  return tmp;
}

} // end of namespace DistMat

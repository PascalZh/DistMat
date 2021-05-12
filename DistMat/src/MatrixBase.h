#pragma once

namespace DistMat
{
using Index = size_t;

template <typename Derived>
class MatrixBase {
private:
  Derived& derived() { return *static_cast<Derived*>(this); }
  const Derived& derived() const { return *static_cast<const Derived*>(this); }

public:

#define	EnsureOverwritten(F)	static_assert(!std::is_same<decltype(&Derived::F),\
        decltype(&MatrixBase<Derived>::F)>::value,\
        "static polymorphic function " #F " was not overwritten.");

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
  // compiler will not error, so add EnsureOverwritten to avoid this.
  Index rows() const { EnsureOverwritten(rows); return derived().rows(); }
  Index cols() const { EnsureOverwritten(cols); return derived().cols(); }
  Index size() const { return rows() * derived().cols(); }
};

} // end of namespace DistMat
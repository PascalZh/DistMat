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
  template <typename Dest>
  void evalTo(Dest& dst) const { derived().evalTo(dst); }

  template <typename Dest>
  void addTo(Dest& dst) const { derived().addTo(dst); }

  template <typename Dest>
  void subTo(Dest& dst) const { derived().subTo(dst); }

  template <typename Dest>
  void mulLeftTo(Dest& dst) const { derived().mulLeftTo(dst); }

  template <typename Dest>
  void mulRightTo(Dest& dst) const { derived().mulRightTo(dst); }

  Index rows() const { return derived().rows(); }
  Index cols() const { return derived().cols(); }
  Index size() const { return rows() * derived().cols(); }
};

} // end of namespace DistMat
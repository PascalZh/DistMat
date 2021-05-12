#pragma once

namespace DistMat
{
template <typename Derived>
class MatrixBase {
public:
  using Derived::rows();
  using Derived::cols();
  using Derived::size();
  
  using Derived::lazy();
  using Derived::eval();
  Derived& derived() { return *static_cast<Derived*>(this); }

  template <typename Dest>
  void evalTo(Dest& dst) { derived().evalTo(dst); }

  template <typename Dest>
  void addTo(Dest& dst) { derived().addTo(dst); }

  template <typename Dest>
  void subTo(Dest& dst) { derived().subTo(dst); }

  template <typename Dest>
  void mulLeftTo(Dest& dst) { derived().mulLeftTo(dst); }

  template <typename Dest>
  void mulRightTo(Dest& dst) { derived().mulRightTo(dst); }
};

} // end of namespace DistMat
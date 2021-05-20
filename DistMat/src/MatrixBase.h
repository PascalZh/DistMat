#pragma once
#include <concepts>
#include "Error.h"
#include "Type.h"

namespace DistMat
{

// Concepts
// TODO ensure Derived implement standard(non-template) operator=

template<typename T>
concept IsScalar = std::regular<T>;

template<typename T>
concept Is_rows_Implemented = !same_as<decltype(&T::rows), decltype(&T::Base::rows)>;
template<typename T>
concept Is_cols_Implemented = !same_as<decltype(&T::cols), decltype(&T::Base::cols)>;
template<typename T>
concept Is_size_Implemented = !same_as<decltype(&T::size), decltype(&T::Base::size)>;

template<typename T>
concept IsAnyTwoOf_rows_cols_size_Implemented =
Is_rows_Implemented<T> && Is_cols_Implemented<T> ||
Is_rows_Implemented<T> && Is_size_Implemented<T> ||
Is_cols_Implemented<T> && Is_size_Implemented<T>;

template<typename Derived, typename Scalar>
concept IsMatrixBaseImplemented = requires (
  Derived mat, const Derived cmat, Index row, Index col, Index i, Scalar scalar 
  ) {
  { cmat(row, col) }    -> same_as<const Scalar&>;
  { cmat.at(row, col) } -> same_as<const Scalar&>;
  { cmat[i] }           -> same_as<const Scalar&>;
  { mat.mulByScalar(scalar) } -> same_as<void>;
  // test func with Derived, ensure func is implemented as a template
  { cmat.template evalTo<Derived>(mat) }     -> same_as<void>;
  { cmat.template addTo<Derived>(mat) }      -> same_as<void>;
  { cmat.template subTo<Derived>(mat) }      -> same_as<void>;
  { cmat.template mulLeftTo<Derived>(mat) }  -> same_as<void>;
  { cmat.template mulRightTo<Derived>(mat) } -> same_as<void>;
} && IsAnyTwoOf_rows_cols_size_Implemented<Derived> && IsScalar<Scalar>;

template<typename Derived, typename Scalar>
class MatrixBase {
public:

  Derived&       derived() { return *static_cast<Derived*>(this); }
  const Derived& derived() const { return *static_cast<const Derived*>(this); }

  const Derived& const_derived(){ return const_cast<const Derived&>(derived()); }
  const Derived& const_derived() const { return derived(); }

  Scalar& operator()(Index row, Index col)
  { return const_cast<Scalar&>(const_derived()(row, col)); }
  Scalar& at(Index row, Index col)
  { return const_cast<Scalar&>(const_derived().at(row, col)); }
  Scalar& operator[](Index i)
  { return const_cast<Scalar&>(const_derived()[i]); }

  ///> Define one of three for Derived if only other two are defined.
  Index rows() const { return derived().size() / derived().cols(); }
  Index cols() const { return derived().size() / derived().rows(); }
  Index size() const { return derived().rows() * derived().cols(); }

  template<typename Dest>
  void evalTo(Dest& other) const requires derived_from<Dest, MatrixBase<Dest, Scalar>>
  {
    CHECK_DIM(other, derived());
    ranges::for_each(views::iota(Index(0), derived().size()), [this, &other](Index i)
    {
      other[i] = derived()[i];
    });
  }

  template<typename Dest>
  void addTo(Dest& other) const requires derived_from<Dest, MatrixBase<Dest, Scalar>>
  {
    CHECK_DIM(other, derived());
    ranges::for_each(views::iota(Index(0), derived().size()), [this, &other](Index i)
    {
      other[i] += derived()[i];
    });
  }
  
  template<typename Dest>
  void subTo(Dest& other) const requires derived_from<Dest, MatrixBase<Dest, Scalar>>
  {
    CHECK_DIM(other, derived());
    ranges::for_each(views::iota(Index(0), derived().size()), [this, &other](Index i)
    {
      other[i] -= derived()[i];
    });
  }

public:
  template<typename OtherDerived>
    requires derived_from<OtherDerived, MatrixBase<OtherDerived, Scalar>>
  Derived& operator=(const MatrixBase<OtherDerived, Scalar>& other)
  {
    other.derived().evalTo(derived());
    return derived();
  }

  template<typename OtherDerived>
    requires derived_from<OtherDerived, MatrixBase<OtherDerived, Scalar>>
  Derived& operator+=(const MatrixBase<OtherDerived, Scalar>& other)
  {
    other.derived().addTo(derived());
    return derived();
  }

  template<typename OtherDerived>
    requires derived_from<OtherDerived, MatrixBase<OtherDerived, Scalar>>
  Derived& operator-=(const MatrixBase<OtherDerived, Scalar>& other)
  {
    other.derived().subTo(derived());
    return derived();
  }

};

template<typename Dest, typename Derived, typename Scalar>
  requires derived_from<Dest, MatrixBase<Dest, Scalar>>
Dest operator+(const Dest& lhs, const MatrixBase<Derived, Scalar>& rhs)
{
  Dest tmp = lhs;
  rhs.derived().addTo(tmp);
  return tmp;
}

template<typename Dest, typename Derived, typename Scalar>
  requires derived_from<Dest, MatrixBase<Dest, Scalar>>
Dest operator-(const Dest& lhs, const MatrixBase<Derived, Scalar>& rhs)
{
  Dest tmp = lhs;
  rhs.derived().subTo(tmp);
  return tmp;
}

template<typename Dest, typename Derived, typename Scalar>
  requires derived_from<Dest, MatrixBase<Dest, Scalar>>
Dest operator*(const Dest& lhs, const MatrixBase<Derived, Scalar>& rhs)
{
  Dest tmp = lhs;
  rhs.derived().mulRightTo(tmp);
  return tmp;
}

template<typename Derived, typename Scalar>
  requires derived_from<Derived, MatrixBase<Derived, Scalar>>
Derived operator*(const Scalar& lhs, const MatrixBase<Derived, Scalar>& rhs)
{
  Derived tmp = rhs.derived();
  tmp.mulByScalar(lhs);
  return tmp;
}

template<typename Derived, typename Scalar>
  requires derived_from<Derived, MatrixBase<Derived, Scalar>>
Derived operator*(const MatrixBase<Derived, Scalar>& lhs, const Scalar& rhs)
{
  return rhs * lhs;
}

} // end of namespace DistMat

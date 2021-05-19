#pragma once
#include <concepts>
#include "Error.h"
#include "Type.h"

namespace DistMat
{

// MatrixBase_traits should be defined to get Scalar of Derived
template<typename Derived>
struct MatrixBase_traits;
template<typename Derived>
class MatrixBase;

// Concepts
// TODO ensure Derived implement standard(non-template) operator=
template<typename _T, typename _Matrix>
concept IsScalarOf = same_as<_T, typename MatrixBase_traits<_Matrix>::scalar_type>;

// if MatrixBase has interface func and impl func the same name, it must be
// treated like this to avoid recurring.
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

template<typename Derived>
concept IsMatrixBaseImplemented = requires (
  Derived mat, const Derived cmat, Index row, Index col, Index i,
  typename Derived::Scalar scalar
  ) {
  { cmat(row, col) }    -> same_as<const typename Derived::Scalar&>;
  { cmat.at(row, col) } -> same_as<const typename Derived::Scalar&>;
  { cmat[i] }         -> same_as<const typename Derived::Scalar&>;
  { mat.mulByScalar(scalar) } -> same_as<void>;
  // test func with Derived, ensure func is implemented as a template
  { cmat.template evalTo<Derived>(mat) }     -> same_as<void>;
  { cmat.template addTo<Derived>(mat) }      -> same_as<void>;
  { cmat.template subTo<Derived>(mat) }      -> same_as<void>;
  { cmat.template mulLeftTo<Derived>(mat) }  -> same_as<void>;
  { cmat.template mulRightTo<Derived>(mat) } -> same_as<void>;
} && IsAnyTwoOf_rows_cols_size_Implemented<Derived>;

template<typename Derived>
class MatrixBase {
public:
  // postpone the concept here, because during the construction of the
  // MatrixBase, it doesn't know anything about Derived.
  ~MatrixBase() requires IsMatrixBaseImplemented<Derived> {}
  Derived& derived()
  { return *static_cast<Derived*>(this); }
  const Derived& derived() const
  { return *static_cast<const Derived*>(this); }

  const Derived& const_derived() { return const_cast<const Derived&>(derived()); }
  const Derived& const_derived() const { return derived(); }

  using Scalar = typename MatrixBase_traits<Derived>::scalar_type;

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
  void evalTo(Dest& other) const
  {
    CHECK_DIM(other, derived());
    std::ranges::for_each(std::views::iota(Index(0), derived().size()),
    [this, &other](Index i)
    {
      other[i] = derived()[i];
    });
  }

public:
  template<typename OtherDerived>
  Derived& operator=(const MatrixBase<OtherDerived>& other)
  {
    other.derived().evalTo(derived());
    return derived();
  }

  template<typename OtherDerived>
  Derived& operator+=(const MatrixBase<OtherDerived>& other)
  {
    other.derived().addTo(derived());
    return derived();
  }
  
  template<typename OtherDerived>
  Derived& operator-=(const MatrixBase<OtherDerived>& other)
  {
    other.derived().subTo(derived());
    return derived();
  }

};

template<typename Dest, typename Derived>
Dest operator+(const Dest& lhs, const MatrixBase<Derived>& rhs)
{
  Dest tmp = lhs;
  rhs.derived().addTo(tmp);
  return tmp;
}

template<typename Dest, typename Derived>
Dest operator-(const Dest& lhs, const MatrixBase<Derived>& rhs)
{
  Dest tmp = lhs;
  rhs.derived().subTo(tmp);
  return tmp;
}

template<typename Dest, typename Derived>
requires (!IsScalarOf<Dest, Derived>)
Dest operator*(const Dest& lhs, const MatrixBase<Derived>& rhs)
{
  Dest tmp = lhs;
  rhs.derived().mulRightTo(tmp);
  return tmp;
}

template<typename Scalar, typename Derived>
requires IsScalarOf<Scalar, Derived>
Derived operator*(const Scalar& lhs, const MatrixBase<Derived>& rhs)
{
  Derived tmp = rhs.derived();
  tmp.mulByScalar(lhs);
  return tmp;
}

template<typename Scalar, typename Derived>
requires IsScalarOf<Scalar, Derived>
Derived operator*(const MatrixBase<Derived>& lhs, const Scalar& rhs)
{
  return rhs * lhs;
}

} // end of namespace DistMat

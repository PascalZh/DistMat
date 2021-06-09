#pragma once
#include "Error.hpp"
#include "Type.hpp"
#include <concepts>
#include <ranges>
#include <algorithm>
#include <iostream>

namespace DistMat
{

/// \brief Define template function.
/// Note some arguments must be of the type `MatrixBase<Derived, Scalar>` to
/// deduce template parameters.
#define DISTMAT_TFUNCTION \
template<typename Derived, typename Scalar>\
  requires derived_from<Derived, MatrixBase<Derived, Scalar>>

#define DISTMAT_MEM_TFUNCTION \
template<typename OtherDerived>\
  requires derived_from<OtherDerived, MatrixBase<OtherDerived, Scalar>>

// Concepts
// ensure Derived implement standard(non-template) operator=

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
(Is_rows_Implemented<T> && Is_cols_Implemented<T>) ||
(Is_rows_Implemented<T> && Is_size_Implemented<T>) ||
(Is_cols_Implemented<T> && Is_size_Implemented<T>);

template<typename Derived, typename Scalar>
concept IsMatrixBaseImplemented = requires (
  Derived mat, const Derived cMat, Index row, Index col, Index i, Scalar scalar 
  ) {
  { cMat(row, col) }    -> same_as<const Scalar&>;
  { cMat.at(row, col) } -> same_as<const Scalar&>;
  { cMat[i] }           -> same_as<const Scalar&>;
} && IsAnyTwoOf_rows_cols_size_Implemented<Derived> && IsScalar<Scalar> &&
std::equality_comparable<Derived>;

template<typename Derived, typename Scalar>
class MatrixBase {
public:

  Derived&       derived()       { return *static_cast<Derived*>(this); }
  const Derived& derived() const { return *static_cast<const Derived*>(this); }

  const Derived& const_derived()       { return const_cast<const Derived&>(derived()); }
  const Derived& const_derived() const { return derived(); }

  Scalar& operator()(Index row, Index col)
  {
    return const_cast<Scalar&>(const_derived()(row, col));
  }
  Scalar& at(Index row, Index col)
  {
    return const_cast<Scalar&>(const_derived().at(row, col));
  }

  //> Access coefficients with just one index just like the matrix is spanned
  // into a vector with row major.
  Scalar& operator[](Index i)
  {
    return const_cast<Scalar&>(const_derived()[i]);
  }

  ///> Define one of three for Derived if only other two are defined.
  Index rows() const { return derived().size() / derived().cols(); }
  Index cols() const { return derived().size() / derived().rows(); }
  Index size() const { return derived().rows() * derived().cols(); }

public:
  void mulByScalar(const Scalar& scalar)
  {
    ranges::for_each(views::iota(Index(0), derived().size()),
      [this, scalar](Index i) { derived()[i] *= scalar; });
  }
  Derived transpose() const
  {
    Derived tmp(derived().cols(), derived().rows());
    for (Index col = 0; col < derived().cols(); ++col) {
      for (Index row = 0; row < derived().rows(); ++row) {
        tmp(col, row) = derived()(row, col);
      }
    }
    return tmp;
  }
#define DEFINE_FUNC_EVAL_ADD_SUB_TO(func, op) \
  DISTMAT_MEM_TFUNCTION\
  void func(OtherDerived& other) const\
  {\
    CHECK_DIM(other, derived());\
    ranges::for_each(views::iota(Index(0), other.size()), [this, &other](Index i)\
    {\
      other[i] op derived()[i];\
    });\
  }
  DEFINE_FUNC_EVAL_ADD_SUB_TO(evalTo, =)
  DEFINE_FUNC_EVAL_ADD_SUB_TO(addTo, +=)
  DEFINE_FUNC_EVAL_ADD_SUB_TO(subTo, -=)
#undef DEFINE_FUNC_EVAL_ADD_SUB_TO

#define DEFINE_ASSIGN_OPERATOR(op, func) \
  DISTMAT_MEM_TFUNCTION\
  Derived& operator op(const OtherDerived& other)\
  {\
    other.func(derived());\
    return derived();\
  }
  // NOLINTNEXTLINE(cppcoreguidelines-c-copy-assignment-signature)
  DEFINE_ASSIGN_OPERATOR(=, evalTo)
  DEFINE_ASSIGN_OPERATOR(+=, addTo)
  DEFINE_ASSIGN_OPERATOR(-=, subTo)
#undef DEFINE_ASSIGN_OPERATOR

  Derived& operator/=(const Scalar& scalar)
  {
    derived().mulByScalar(1.0 / scalar);
    return derived();
  }

  DISTMAT_MEM_TFUNCTION
  bool operator==(const OtherDerived& other) const
  {
    CHECK_DIM(derived(), other);
    bool isEqual = true;
    for (Index i = 0; i < other.size(); ++i) {
      if (derived()[i] != other[i]) {
        isEqual = false;
        break;
      }
    }
    return isEqual;
  }
};

#define DEFINE_BINARY_OPERATOR(op, func) \
template<typename Dest, typename Derived, typename Scalar>\
  requires derived_from<Derived, MatrixBase<Derived, Scalar>>\
Dest operator op(const Dest& lhs, const MatrixBase<Derived, Scalar>& rhs)\
{\
  Dest tmp = lhs;\
  rhs.derived().func(tmp);\
  return tmp;\
}
DEFINE_BINARY_OPERATOR(+, addTo)
DEFINE_BINARY_OPERATOR(-, subTo)
DEFINE_BINARY_OPERATOR(*, mulRightTo)
#undef DEFINE_BINARY_OPERATOR

DISTMAT_TFUNCTION
Derived operator*(const Scalar& lhs, const MatrixBase<Derived, Scalar>& rhs)
{
  Derived tmp = rhs.derived();
  tmp.mulByScalar(lhs);
  return tmp;
}

DISTMAT_TFUNCTION
Derived operator*(const MatrixBase<Derived, Scalar>& lhs, const Scalar& rhs)
{
  return rhs * lhs;
}

DISTMAT_TFUNCTION
Derived operator/(const MatrixBase<Derived, Scalar>& lhs, const Scalar& rhs)
{
  Derived tmp = lhs.derived();
  tmp.mulByScalar(1.0 / rhs);
  return tmp;
}

/// \brief Unary operator - as in -A
DISTMAT_TFUNCTION
Derived operator-(const MatrixBase<Derived, Scalar>& mat)
{
  Derived tmp = mat.derived();
  ranges::for_each(views::iota(Index(0), tmp.size()), [&tmp](Index i)
  {
    tmp[i] = -tmp[i];
  });
  return tmp;
}

DISTMAT_TFUNCTION
std::ostream& operator<<(std::ostream& out, const MatrixBase<Derived, Scalar>& mat)
{
  for (Index col = 0; col < mat.derived().cols(); ++col) {
    for (Index row = 0; row < mat.derived().rows(); ++row) {
      out << mat.derived()(row, col) << " ";
    }
    out << endl;
  }
  return out;
}

} // namespace DistMat

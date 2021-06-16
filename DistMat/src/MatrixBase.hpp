#pragma once
#include "Multiplication.hpp"

#include "Error.hpp"
#include "Type.hpp"
#include "Traits.hpp"

#include <concepts>
#include <ranges>
#include <algorithm>
#include <iostream>

namespace distmat {

/// \brief Define template function.
/// Inject template parameters `_Derived` and `_Scalar`.
/// Note some arguments must be of the type `MatrixBase<_Derived, _Scalar>` to
/// deduce template parameters.
#define DISTMAT_TFUNC \
template<typename _Derived, typename _Scalar>\
  requires derived_from<_Derived, MatrixBase<_Derived, _Scalar>>

#define DISTMAT_BINARY_TFUNC \
template<typename _LDerived, typename _RDerived, typename _Scalar>\
  requires derived_from<_LDerived, MatrixBase<_LDerived, _Scalar>> &&\
    derived_from<_RDerived, MatrixBase<_RDerived, _Scalar>>

#define DISTMAT_MEM_TFUNC \
template<typename OtherDerived>\
  requires derived_from<OtherDerived, MatrixBase<OtherDerived, Scalar>>

template<typename Derived, typename Scalar>
class MatrixBase {
public:
  constexpr Derived&       derived()       { return *static_cast<Derived*>(this); }
  constexpr const Derived& derived() const { return *static_cast<const Derived*>(this); }

  constexpr const Derived& const_derived()       { return const_cast<const Derived&>(derived()); }
  constexpr const Derived& const_derived() const { return derived(); }

  constexpr Scalar& operator()(Index row, Index col)
  {
    return const_cast<Scalar&>(const_derived()(row, col));
  }
  constexpr Scalar& at(Index row, Index col)
  {
    return const_cast<Scalar&>(const_derived().at(row, col));
  }

  //> Access coefficients with just one index just like the matrix is spanned
  // into a vector with row major.
  constexpr Scalar& operator[](Index i)
  {
    return const_cast<Scalar&>(const_derived()[i]);
  }

  ///> Define one of three for Derived if only other two are defined.
  constexpr Index rows() const { return derived().size() / derived().cols(); }
  constexpr Index cols() const { return derived().size() / derived().rows(); }
  constexpr Index size() const { return derived().rows() * derived().cols(); }

  constexpr bool isSquare() const { return derived().rows() == derived().cols(); }

  Derived transpose() const;

  static Derived eye(Index row, Index col);
  static Derived zeros(Index row, Index col) { return fill(row, col, traits::scalar_traits<Scalar>::zero); }
  static Derived ones(Index row, Index col) { return fill(row, col, traits::scalar_traits<Scalar>::one); }
  static Derived fill(Index row, Index col, Scalar fillValue);

// ********************** implimentations of arithematics **************************
  void mulByScalar(const Scalar& scalar);

  /// Multiply two square matrices and assign to the `dst`.
  /// `dst = (*this) * dst`
  DISTMAT_MEM_TFUNC
  void MulLeftTo(OtherDerived& dst) const
  {
    CHECK_DIM(dst, derived());
    assert(isSquare());
    Derived tmp(dst.rows(), 1);
    mul::multiplyMatrixLeftToInplace<Index>(dst, derived(), tmp);
  }

  /// Multiply two square matrices and assign to the `dst`.
  /// `dst = dst * (*this)`
  DISTMAT_MEM_TFUNC
  void MulRightTo(OtherDerived& dst) const
  {
    CHECK_DIM(dst, derived());
    assert(isSquare());
    Derived tmp(dst.rows(), 1);
    mul::multiplyMatrixRightToInplace<Index>(dst, derived(), tmp);
  }

#define DEFINE_FUNC_EVAL_ADD_SUB_TO(func, op) \
  DISTMAT_MEM_TFUNC\
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

// *********************** Operators ***********************

#define DEFINE_ASSIGN_OPERATOR(op, func) \
  DISTMAT_MEM_TFUNC\
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
    derived().mulByScalar(traits::scalar_traits<Scalar>::one / scalar);
    return derived();
  }

  bool operator==(const MatrixBase<Derived, Scalar>& other) const
  {
    CHECK_DIM(derived(), other.derived());
    bool isEqual = true;
    for (Index i = 0; i < other.derived().size(); ++i) {
      if (derived()[i] != other.derived()[i]) {
        isEqual = false;
        break;
      }
    }
    return isEqual;
  }

}; // class MatrixBase

template<typename Derived, typename Scalar>
  Derived MatrixBase<Derived, Scalar>::transpose() const
  {
    Derived tmp(derived().cols(), derived().rows());
    for (Index col = 0; col < derived().cols(); ++col) {
      for (Index row = 0; row < derived().rows(); ++row) {
        tmp(col, row) = derived()(row, col);
      }
    }
    return tmp;
  }

template<typename Derived, typename Scalar>
  void MatrixBase<Derived, Scalar>::mulByScalar(const Scalar& scalar)
  {
    ranges::for_each(views::iota(Index(0), derived().size()),
      [this, scalar](Index i) { derived()[i] *= scalar; });
  }

template<typename Derived, typename Scalar>
  Derived MatrixBase<Derived, Scalar>::eye(Index row, Index col)
  {
    Derived ret = Derived::zeros(row, col);
    for (Index i = 0; i < std::min(row, col); ++i) {
      ret(i, i) = traits::scalar_traits<Scalar>::one;
    }
    return ret;
  }

template<typename Derived, typename Scalar>
  Derived MatrixBase<Derived, Scalar>::fill(Index row, Index col, Scalar fillValue)
  {
    Derived ret(row, col);
    for (Index i = 0; i < ret.size(); ++i) {
      ret[i] = fillValue;
    }
    return ret;
  }

/// ************************* Operators ****************************

/// default binary operators
DISTMAT_BINARY_TFUNC
_LDerived operator+(const _LDerived& lhs, const MatrixBase<_RDerived, _Scalar>& rhs)\
{
  _LDerived tmp = lhs;
  rhs.derived().addTo(tmp);
  return tmp;
}

DISTMAT_BINARY_TFUNC
_LDerived operator-(const _LDerived& lhs, const MatrixBase<_RDerived, _Scalar>& rhs)\
{
  _LDerived tmp = lhs;
  rhs.derived().subTo(tmp);
  return tmp;
}

DISTMAT_BINARY_TFUNC
_LDerived operator*(const _LDerived& lhs, const MatrixBase<_RDerived, _Scalar>& rhs)
{
  CHECK_MUL_DIM(lhs, rhs.derived());
  _LDerived tmp = _LDerived::zeros(lhs.rows(), rhs.derived().rows());
  mul::multiplyMatrix<Index>(lhs, rhs.derived(), tmp);
  return tmp;
}

DISTMAT_TFUNC
_Derived operator*(const _Scalar& lhs, const MatrixBase<_Derived, _Scalar>& rhs)
{
  _Derived tmp = rhs.derived();
  tmp.mulByScalar(lhs);
  return tmp;
}

DISTMAT_TFUNC
_Derived operator*(const MatrixBase<_Derived, _Scalar>& lhs, const _Scalar& rhs)
{
  return rhs * lhs;
}

DISTMAT_TFUNC
_Derived operator/(const MatrixBase<_Derived, _Scalar>& lhs, const _Scalar& rhs)
{
  _Derived tmp = lhs.derived();
  return tmp * (traits::scalar_traits<_Scalar>::one / rhs);
}

/// \brief Unary operator - as in -A
DISTMAT_TFUNC
_Derived operator-(const MatrixBase<_Derived, _Scalar>& mat)
{
  _Derived tmp = mat.derived();
  ranges::for_each(views::iota(Index(0), tmp.size()), [&tmp](Index i)
  {
    tmp[i] = -tmp[i];
  });
  return tmp;
}

DISTMAT_TFUNC
std::ostream& operator<<(std::ostream& out, const MatrixBase<_Derived, _Scalar>& mat)
{
  auto& der = mat.derived();
  for (Index row = 0; row < der.rows(); ++row) {
    for (Index col = 0; col < der.cols(); ++col) {
      out << der(row, col) << " ";
    }
    out << endl;
  }
  return out;
}

// Concepts

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
  (Is_rows_Implemented<T> && Is_cols_Implemented<T>)
  || (Is_rows_Implemented<T> && Is_size_Implemented<T>)
  || (Is_cols_Implemented<T> && Is_size_Implemented<T>);

template<typename Derived, typename Scalar>
  concept IsMatrixBaseImplemented = derived_from<Derived, MatrixBase<Derived, Scalar>>
  && requires (Derived mat, const Derived cMat,
    Index row, Index col, Index i, Scalar scalar) {
    { cMat(row, col) }    -> same_as<const Scalar&>;
    { cMat.at(row, col) } -> same_as<const Scalar&>;
    { cMat[i] }           -> same_as<const Scalar&>;
  } && IsAnyTwoOf_rows_cols_size_Implemented<Derived>
  && IsScalar<Scalar> && std::regular<Derived>;

}  // namespace distmat
#pragma once
#include <concepts>
#include <ranges>
#include <algorithm>
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
  Derived mat, const Derived cMat, Index row, Index col, Index i, Scalar scalar 
  ) {
  { cMat(row, col) }    -> same_as<const Scalar&>;
  { cMat.at(row, col) } -> same_as<const Scalar&>;
  { cMat[i] }           -> same_as<const Scalar&>;
  { mat.mulByScalar(scalar) } -> same_as<void>;
  // test func with Derived, ensure func is implemented as a template
  { cMat.template evalTo<Derived>(mat) }     -> same_as<void>;
  { cMat.template addTo<Derived>(mat) }      -> same_as<void>;
  { cMat.template subTo<Derived>(mat) }      -> same_as<void>;
  { cMat.template mulLeftTo<Derived>(mat) }  -> same_as<void>;
  { cMat.template mulRightTo<Derived>(mat) } -> same_as<void>;
} && IsAnyTwoOf_rows_cols_size_Implemented<Derived> && IsScalar<Scalar> && std::equality_comparable<Derived>;

template<typename Derived, typename Scalar>
class MatrixBase {
public:

  Derived&       derived() { return *static_cast<Derived*>(this); }
  const Derived& derived() const { return *static_cast<const Derived*>(this); }

  const Derived& const_derived(){ return const_cast<const Derived&>(derived()); }
  const Derived& const_derived() const { return derived(); }

  Scalar& operator()(Index row, Index col)
  {
    return const_cast<Scalar&>(const_derived()(row, col));
  }
  Scalar& at(Index row, Index col)
  {
    return const_cast<Scalar&>(const_derived().at(row, col));
  }
  Scalar& operator[](Index i)
  {
    return const_cast<Scalar&>(const_derived()[i]);
  }

  ///> Define one of three for Derived if only other two are defined.
  Index rows() const { return derived().size() / derived().cols(); }
  Index cols() const { return derived().size() / derived().rows(); }
  Index size() const { return derived().rows() * derived().cols(); }

private:
  void for_each_el(auto func)
  {
    ranges::for_each(views::iota(Index(0), derived().size()), func);
  }
  void for_each_el(auto func) const
  {
    ranges::for_each(views::iota(Index(0), derived().size()), func);
  }
public:
  void mulByScalar(Scalar scalar)
  {
    for_each_el([this, scalar](Index i) { derived()[i] *= scalar; });
  }

#define DEFINE_FUNC_EVAL_ADD_SUB_TO(func, op) \
  template<typename Dest>\
    requires derived_from<Dest, MatrixBase<Dest, Scalar>>\
  void func(Dest& other) const\
  {\
    CHECK_DIM(other, derived());\
    for_each_el([this, &other](Index i)\
    {\
      other[i] op derived()[i];\
    });\
  }

  DEFINE_FUNC_EVAL_ADD_SUB_TO(evalTo, =)
  DEFINE_FUNC_EVAL_ADD_SUB_TO(addTo, +=)
  DEFINE_FUNC_EVAL_ADD_SUB_TO(subTo, -=)
#undef DEFINE_FUNC_EVAL_ADD_SUB_TO

#define DEFINE_ASSIGN_OPERATOR(op, func) \
  template<typename OtherDerived>\
    requires derived_from<OtherDerived, MatrixBase<OtherDerived, Scalar>>\
  Derived& operator op(const MatrixBase<OtherDerived, Scalar>& other)\
  {\
    other.derived().func(derived());\
    return derived();\
  }

  DEFINE_ASSIGN_OPERATOR(=, evalTo)
  DEFINE_ASSIGN_OPERATOR(+=, addTo)
  DEFINE_ASSIGN_OPERATOR(-=, subTo)
#undef DEFINE_ASSIGN_OPERATOR
};

#define DEFINE_ADD_SUB_MUL_OPERATOR(op, func) \
template<typename Dest, typename Derived, typename Scalar>\
  requires derived_from<Derived, MatrixBase<Derived, Scalar>>\
Dest operator op(const Dest& lhs, const MatrixBase<Derived, Scalar>& rhs)\
{\
  Dest tmp = lhs;\
  rhs.derived().func(tmp);\
  return tmp;\
}

DEFINE_ADD_SUB_MUL_OPERATOR(+, addTo)
DEFINE_ADD_SUB_MUL_OPERATOR(-, subTo)
DEFINE_ADD_SUB_MUL_OPERATOR(*, mulRightTo)

template<typename Derived, typename Scalar>
  requires derived_from<Derived, MatrixBase<Derived, Scalar>> && IsScalar<Scalar>
Derived operator*(const Scalar& lhs, const MatrixBase<Derived, Scalar>& rhs)
{
  Derived tmp = rhs.derived();
  tmp.mulByScalar(lhs);
  return tmp;
}

template<typename Derived, typename Scalar>
  requires derived_from<Derived, MatrixBase<Derived, Scalar>> && IsScalar<Scalar>
Derived operator*(const MatrixBase<Derived, Scalar>& lhs, const Scalar& rhs)
{
  return rhs * lhs;
}

} // namespace DistMat
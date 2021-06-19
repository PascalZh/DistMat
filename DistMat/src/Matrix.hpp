#pragma once
#include "MatrixBase.hpp"
#include "Util.hpp"

#include <vector>
#include <memory>
#include <array>
#include <type_traits>
#include <iterator>

namespace distmat
{

template<int X>
  concept Dynamic = (X == -1);

template<int X>
  concept Fixed = (X > 0);

template<typename T>
struct error_no_internal_storage { static_assert(util::always_false_v<T>, "No internal storage is matched!"); };

template<int Rows, int Cols>
  struct DefaultShape {};

template<int Rows, int Cols>
  requires Dynamic<Rows> && Dynamic<Cols>
  struct DefaultShape<Rows, Cols> {
    Index rows() const { return rows_; }
    Index cols() const { return cols_; }
    Index rows_;
    Index cols_;
  };

template<int Rows, int Cols>
  requires Fixed<Rows> && Fixed<Cols>
  struct DefaultShape<Rows, Cols> {
    constexpr Index rows() const { return Rows; }
    constexpr Index cols() const { return Cols; }
  };

template<
  IsScalar Scalar,
  int Rows = -1,
  int Cols = -1,
  typename InternalStorage = conditional_t<Rows == -1 && Cols == -1,
    vector<Scalar, util::default_init_allocator<Scalar>>,
    conditional_t<(Rows > 0 && Cols > 0),
      std::array<Scalar, Rows * Cols>,
      error_no_internal_storage<Scalar>
      >
    >,
  typename Shape = DefaultShape<Rows, Cols>
  >
  class Matrix;

template<IsScalar Scalar, int Rows, int Cols, typename InternalStorage, typename Shape>
class Matrix : public MatrixBase<Matrix<Scalar, Rows, Cols, InternalStorage, Shape>, Scalar> {
public:
  using scalar_type = Scalar;
  using Base = MatrixBase<Matrix, Scalar>;
  using Base::const_derived;

  Matrix() = default;
  Matrix(Matrix&& other) = default;
  Matrix(const Matrix& other) = default;

  // `vector(size_t n)` will default-insert the elements, thus value-initialize the elements.
  // So we replace the default allocator with the Utils::default_init_allocator
  template<typename T = Scalar>
    requires is_same_v<T, Scalar> && Dynamic<Rows> && Dynamic<Cols>
  Matrix(Index rows, Index cols) : storage_(rows * cols), shape_{rows, cols} {}

  template<typename T = Scalar>
    requires is_same_v<T, Scalar> && Fixed<Rows> && Fixed<Cols>
  constexpr explicit Matrix(InternalStorage storage) : storage_{std::move(storage)} {}

  constexpr ~Matrix() {}

  using Base::operator=;
  Matrix& operator=(Matrix&& other) = default;
  Matrix& operator=(const Matrix& other)
  {
    if (this != &other) {
      other.evalTo(*this);
    }
    return *this;
  }

  /// Assign matrix with initializer list, the matrix coeff is assigned row by row.
  /// e.g. `A = {1, 2, 3, 4, 5 6, 7, 8, 9};` will fill A with:
  /// 1 2 3
  /// 4 5 6
  /// 7 8 9
  Matrix& operator=(initializer_list<Scalar> l)
  {
    assert(l.size() <= storage_.size());
    auto it = storage_.begin();
    for (auto x : l) {
      *it = x;
      ++it;
    }
    return *this;
  }

  using Base::operator();
  using Base::at;
  using Base::operator[];
  constexpr const Scalar& operator()(Index row, Index col) const
  {
    // use C order instead of Fortran order
    return storage_[row * this->cols() + col];
  }
  constexpr const Scalar& at(Index row, Index col) const
  {
    // use C order instead of Fortran order
    if (!(row < this->rows() && col < this->cols())) {
      throw std::range_error("bound check errors");
    }
    return storage_[row * this->cols() + col];
  }
  constexpr const Scalar& operator[](Index i) const
  {
    return storage_[i];
  }

  constexpr Index rows() const { return shape_.rows(); }
  constexpr Index cols() const { return shape_.cols(); }
  constexpr Index size() const { return storage_.size(); }

  void foo(Scalar other)
  {
    other.call_some_func_that_dont_exist(); // OK, this is not instantiated if not called, thus don't check
  }
private:
  InternalStorage storage_;
  Shape shape_;

public:  // Some constexpr version of functions

  using Base::eye;
  using Base::zeros;
  using Base::ones;
  using Base::fill;
  static constexpr Matrix eye()
    requires (Rows > 0) && (Cols > 0)
  {
    auto ret = zeros();
    for (Index i = 0; i < std::min(Rows, Cols); ++i) {
      ret(i, i) = traits::scalar_traits<Scalar>::one;
    }
    return ret;
  }
  static constexpr Matrix zeros()
    requires (Rows > 0) && (Cols > 0) { return fill(traits::scalar_traits<Scalar>::zero); }
  static constexpr Matrix ones()
    requires (Rows > 0) && (Cols > 0) { return fill(traits::scalar_traits<Scalar>::one); }
  static constexpr Matrix fill(const Scalar& fillValue)
    requires (Rows > 0) && (Cols > 0)
  {
    Matrix ret;
    for (Index i = 0; i < ret.size(); ++i) {
      ret[i] = fillValue;
    }
    return ret;
  }

};  // class Matrix

template<typename _Scalar, int _Rows, int _Cols>
  requires (_Rows > 0) && (_Cols > 0)
constexpr Matrix<_Scalar, _Rows, _Cols> operator*(const Matrix<_Scalar, _Rows, _Cols>& lhs, const Matrix<_Scalar, _Rows, _Cols>& rhs)
{
  auto tmp = Matrix<_Scalar, _Rows, _Cols>::zeros();
  mul::multiplyMatrix<Index>(lhs, rhs, tmp);
  return tmp;
}

namespace detail {

  // postpone the concept here, because during the construction of the
  // MatrixBase, it doesn't know anything about Derived.
  template<typename Scalar>
    requires IsMatrixBaseImplemented<Matrix<Scalar, -1, -1>, Scalar>
    class Test_Matrix_With_Concept {};
  // explicit instantiation, only test some type
  template class Test_Matrix_With_Concept<int>;
  template class Test_Matrix_With_Concept<double>;

}  // namespace detail

}  // end of namespace distmat

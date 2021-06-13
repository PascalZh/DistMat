#pragma once
#include "MatrixBase.hpp"
#include "Util.hpp"

#include "range.hpp"

#include <vector>
#include <memory>
#include <array>
#include <type_traits>
#include <iterator>

namespace distmat
{

using ::util::lang::range;
using ::util::lang::indices;

template<typename Scalar>
  using default_init_vector = std::vector<Scalar, util::default_init_allocator<Scalar>>;

template<typename T>
  struct error_no_internal_storage {
    static_assert(!is_same_v<T, T>, "No internal storage is matched!");
  };

template<int Rows, int Cols, typename T>
  struct internal_storage_selector {
    using type = std::conditional_t<Rows == -1 && Cols == -1,
      default_init_vector<T>,
      std::conditional_t<(Rows > 0 && Cols > 0),
        std::array<T, Rows * Cols>,
        error_no_internal_storage<T>
        >
      >;
  };

template<
  IsScalar Scalar,
  int Rows = -1,
  int Cols = -1,
  typename InternalStorage = internal_storage_selector<Rows, Cols, Scalar>::type
  >
  class Matrix;

template<int Rows, int Cols>
  struct Shape {
    Index rows() const { return m_rows; }
    Index cols() const { return m_cols; }
    Index m_rows;
    Index m_cols;
  };

template<int Rows, int Cols>
  requires (Rows > 0) && (Cols > 0)
  struct Shape<Rows, Cols> {
    constexpr Index rows() const { return Rows; }
    constexpr Index cols() const { return Cols; }
  };

template<IsScalar Scalar, int Rows, int Cols, typename InternalStorage>
class Matrix : public MatrixBase<Matrix<Scalar, Rows, Cols, InternalStorage>, Scalar> {
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
    requires is_same_v<T, Scalar> && (Rows == -1) && (Cols == -1)
  Matrix(Index rows, Index cols)
    : m_storage(rows * cols), m_shape{rows, cols} {}

  template<typename T = Scalar>
    requires is_same_v<T, Scalar>
  constexpr explicit Matrix(InternalStorage storage)
    : m_storage{std::move(storage)}
  {
    if constexpr(!(Rows > 0 && Cols > 0)) {
      static_assert(!is_same_v<T, T>, "Can not initialize the matrix without shape specified!");
    }
  }

  ~Matrix() = default;

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
    assert(l.size() <= m_storage.size());
    auto it = m_storage.begin();
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
    return m_storage[row * this->cols() + col];
  }
  constexpr const Scalar& at(Index row, Index col) const
  {
    // use C order instead of Fortran order
    if (!(row < this->rows() && col < this->cols())) {
      throw std::range_error("bound check errors");
    }
    return m_storage[row * this->cols() + col];
  }
  constexpr const Scalar& operator[](Index i) const
  {
    return m_storage[i];
  }

  constexpr Index rows() const { return m_shape.rows(); }
  constexpr Index cols() const { return m_shape.cols(); }
  constexpr Index size() const { return m_storage.size(); }

private:
  InternalStorage m_storage;
  Shape<Rows, Cols> m_shape;

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

};

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
    requires IsMatrixBaseImplemented<Matrix<Scalar, -1, -1, default_init_vector<Scalar> >, Scalar>
    class Test_Matrix_With_Concept {};
  // explicit instantiation, only test some type
  template class Test_Matrix_With_Concept<int>;
  template class Test_Matrix_With_Concept<double>;

} // namespace detail

} // end of namespace distmat

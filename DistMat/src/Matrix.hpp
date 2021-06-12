#pragma once
#include "MatrixBase.hpp"
#include "Util.hpp"

#include "range.hpp"

#include <vector>
#include <memory>

namespace distmat
{

using ::util::lang::range;
using ::util::lang::indices;

template<typename Scalar>
  using default_init_vector = std::vector<Scalar, util::default_init_allocator<Scalar>>;

template<IsScalar Scalar,
  template<typename T> typename InternalStorage = default_init_vector>
class Matrix;

template<IsScalar Scalar, template<typename T> typename InternalStorage>
class Matrix : public MatrixBase<Matrix<Scalar, InternalStorage>, Scalar> {
public:
  using scalar_type = Scalar;
  using Base = MatrixBase<Matrix, Scalar>;
  using Base::const_derived;

  Matrix() = default;
  Matrix(Matrix&& other) = default;
  Matrix(const Matrix& other)
    : m_storage(other.rows() * other.cols())
    , m_rows(other.rows()), m_cols(other.cols())
  {
    other.evalTo(*this);
  }

  // `vector(size_t n)` will default-insert the elements, thus value-initialize the elements.
  // So we replace the default allocator with the Utils::default_init_allocator
  Matrix(Index rows, Index cols)
    : m_storage(rows * cols), m_rows(rows), m_cols(cols) {}
    
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
  /// The list must have the same size as the matrix.
  Matrix& operator=(std::initializer_list<Scalar> l)
  {
    assert(l.size() == m_storage.size());
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
  const Scalar& operator()(Index row, Index col) const
  {
    // use C order instead of Fortran order
    return m_storage[row * this->cols() + col];
  }
  const Scalar& at(Index row, Index col) const
  {
    // use C order instead of Fortran order
    if (!(row < this->rows() && col < this->cols())) {
      throw std::range_error("bound check errors");
    }
    return m_storage[row * this->cols() + col];
  }
  const Scalar& operator[](Index i) const
  {
    return m_storage[i];
  }

  Index rows() const { return m_rows; }
  Index cols() const { return m_cols; }
  Index size() const { return m_storage.size(); }
private:
  InternalStorage<Scalar> m_storage;
  Index m_rows = 0;
  Index m_cols = 0;
};

namespace detail {

  // postpone the concept here, because during the construction of the
  // MatrixBase, it doesn't know anything about Derived.
  template<typename Scalar>
    requires IsMatrixBaseImplemented<Matrix<Scalar, default_init_vector>, Scalar>
    class Test_Matrix_With_Concept {};
  // explicit instantiation, only test some type
  template class Test_Matrix_With_Concept<int>;
  template class Test_Matrix_With_Concept<double>;

} // namespace detail

} // end of namespace distmat

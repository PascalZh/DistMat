#pragma once
#include "MatrixBase.hpp"
#include "Utils.hpp"
#include <vector>
#include <memory>

namespace DistMat
{

template <typename Scalar>
class Matrix : public MatrixBase<Matrix<Scalar>, Scalar> {
public:
  using scalar_type = Scalar;
  using Base = MatrixBase<Matrix<Scalar>, Scalar>;
  using Base::const_derived;

  Matrix() = default;
  Matrix(Matrix<Scalar>&& other) = default;
  Matrix<Scalar>& operator=(Matrix<Scalar>&& other) = default;

  // `vector(size_t n)` will default-insert the elements, thus value-initialize the elements.
  // So we replace the default allocator with the Utils::default_init_allocator
  Matrix(Index rows, Index cols)
    : m_plain_object(rows * cols), m_rows(rows), m_cols(cols), m_size(rows * cols) {}

  Matrix(const Matrix<Scalar>& other)
    : m_plain_object(other.rows() * other.cols())
    , m_rows(other.rows()), m_cols(other.cols()), m_size(other.size())
  {
    other.evalTo(*this);
  }

  using Base::operator=;
  Matrix<Scalar>& operator=(const Matrix<Scalar>& other)
  {
    if (this != &other) {
      other.evalTo(*this);
    }
    return *this;
  }

  using Base::operator();
  using Base::at;
  using Base::operator[];
  const Scalar& operator()(Index row, Index col) const
  {
    // use C order instead of Fortran order
    return m_plain_object[row * this->rows() + col];
  }
  const Scalar& at(Index row, Index col) const
  {
    // use C order instead of Fortran order
    if (!(row < this->rows() && col < this->cols())) {
      throw std::range_error("bound check errors");
    }
    return m_plain_object[row * this->rows() + col];
  }
  const Scalar& operator[](Index i) const
  {
    return m_plain_object[i];
  }

  bool operator==(const Matrix<Scalar>& other) const
  {
    CHECK_DIM((*this), other);
    bool isEqual = true;
    for (Index i = 0; i < m_size; ++i) {
      if (this->operator[](i) != other[i]) {
        isEqual = false;
        break;
      }
    }
    return isEqual;
  }

  template<typename Dest>
  void mulLeftTo(Dest& other) const
  { 
    // TODO
  }

  template <typename Dest>
  void mulRightTo(Dest &other) const
  {
    // TODO
  }

  Index rows() const { return m_rows; }
  Index cols() const { return m_cols; }
  Index size() const { return m_size; }
private:
  std::vector<Scalar, Utils::default_init_allocator<Scalar>> m_plain_object;
  Index m_rows = 0;
  Index m_cols = 0;
  Index m_size = 0;
};

// postpone the concept here, because during the construction of the
// MatrixBase, it doesn't know anything about Derived.
template <typename Scalar>
  requires IsMatrixBaseImplemented<Matrix<Scalar>, Scalar>
using TEST_MATRIX_WITH_CONCEPT = Matrix<Scalar>;

} // end of namespace DistMat

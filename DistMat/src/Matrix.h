#pragma once
#include <vector>
#include "MatrixBase.h"

namespace DistMat
{

template <typename Scalar>
class Matrix : public MatrixBase<Matrix<Scalar>, Scalar> {
public:
  using Base = MatrixBase<Matrix<Scalar>, Scalar>;
  using Base::const_derived;

  // postpone the concept here, because during the construction of the
  // MatrixBase, it doesn't know anything about Derived.
  ~Matrix() requires IsMatrixBaseImplemented<Matrix<Scalar>, Scalar> {}
  Matrix() = default;
  Matrix(const Matrix<Scalar>& other) = default;

  using Base::operator=;
  Matrix<Scalar>& operator=(const Matrix<Scalar>& other)
  {
    if (this != &other)
      other.evalTo(*this);
    return *this;
  }

  Matrix(Index rows, Index cols) // TODO: by default elements of vector is value-initialized, which may affects performance.
    : m_plain_object(rows * cols), m_rows(rows), m_cols(cols) {}

  Matrix(std::vector<Scalar> plain_object,Index rows, Index cols)
    : m_plain_object(plain_object), m_rows(rows), m_cols(cols) {}

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
    return m_plain_object.at(row * this->rows() + col);
  }
  const Scalar& operator[](Index i) const
  {
    return m_plain_object[i];
  }

  bool operator==(const Matrix<Scalar>& other) const
  {
    return this->m_plain_object == other.m_plain_object;
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
  Index size() const { return m_plain_object.size(); }
private:
  std::vector<Scalar> m_plain_object;
  Index m_rows = 0;
  Index m_cols = 0;
};

} // end of namespace DistMat
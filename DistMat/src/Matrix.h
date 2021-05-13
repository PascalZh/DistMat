#pragma once
#include <vector>
#include "MatrixBase.h"

namespace DistMat
{

template <typename Dtype, bool _Lazy = true>
class Matrix : public MatrixBase<Matrix<Dtype>> {
public:
  using value_type = Dtype;

  Matrix() = default;
  Matrix(Index rows, Index cols)
    : m_plain_object(rows * cols), m_rows(rows), m_cols(cols) { }

  const value_type& accessWithoutBoundsChecking(Index row, Index col) const
  {
    // use C order instead of Fortran order
    return m_plain_object[row * rows() + col];
  }

  const value_type& accessWithBoundsChecking(Index row, Index col) const
  {
    // use C order instead of Fortran order
    return m_plain_object.at(row * rows() + col);
  }

  Index rows() const { return m_rows; }
  Index cols() const { return m_cols; }
private:
  std::vector<value_type> m_plain_object;
  Index m_rows = 0;
  Index m_cols = 0;
};

} // end of namespace DistMat
#pragma once
#include <vector>
#include "MatrixBase.h"

namespace DistMat
{
template <typename Dtype>
class Matrix : public MatrixBase<Matrix<Dtype>> {
public:
  Matrix() = default;
  Matrix(size_t rows, size_t cols)
    : m_plain_object(rows * cols), m_rows(rows), m_cols(cols)
  {
  }
  size_t rows() const { return m_rows; }
  size_t cols() const { return m_cols; }
private:
  std::vector<Dtype> m_plain_object;
  size_t m_rows = 0;
  size_t m_cols = 0;
};

} // end of namespace DistMat
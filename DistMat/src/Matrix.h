#pragma once
#include <vector>
#include <algorithm>
#include <ranges>
#include "MatrixBase.h"

namespace DistMat
{

template <typename Dtype>
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

  template<typename Dest>
  void addTo_impl(Dest& other) const
  {
    if (this->rows() != other.rows() || this->cols() != other.rows()) {
      // TODO
      throw;
    }
    std::ranges::for_each(std::views::iota(Index(0), m_plain_object.size()), [this, &other](int i)
    {
      other.m_plain_object[i] += this->m_plain_object[i];
    });
  }

  Index rows() const { return m_rows; }
  Index cols() const { return m_cols; }
private:
  std::vector<value_type> m_plain_object;
  Index m_rows = 0;
  Index m_cols = 0;
};

} // end of namespace DistMat
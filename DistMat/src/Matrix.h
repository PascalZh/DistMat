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
    : m_plain_object(rows * cols), m_rows(rows), m_cols(cols) {}

  Matrix(vector<value_type> plain_object,Index rows, Index cols)
    : m_plain_object(plain_object), m_rows(rows), m_cols(cols) {}

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
  void addTo(Dest& other) const
  {
    if (this->rows() != other.rows() || this->cols() != other.cols()) {
      // TODO
      throw;
    }
    std::ranges::for_each(std::views::iota(Index(0), m_plain_object.size()), [this, &other](int i)
    {
      other.m_plain_object[i] += this->m_plain_object[i];
    });
  }

    void subTo(Dest& other) const
  {
    if (this->rows() != other.rows() || this->cols() != other.cols()) {
      // TODO
      throw;
    }
    std::ranges::for_each(std::views::iota(Index(0), m_plain_object.size()), [this, &other](int i)
    {
      other.m_plain_object[i] -= this->m_plain_object[i];
    });
  }
	void evalTo(Dest& other) const
  {
    if (this->rows() != other.rows() || this->cols() != other.cols()) {
      // TODO
      throw;
    }
    std::ranges::for_each(std::views::iota(Index(0), m_plain_object.size()), [this, &other](int i)
    {
      other.m_plain_object[i] = this->m_plain_object[i];
    });
  }

	void mulRightTo(Dest& other) const
  {
    if (this->rows() != other.cols() || this->cols() != other.rows()) {
      // TODO
      throw;
    }
	int k=0,l=0,tmp=0;
    for(int i=0;i<this->rows();i++){
		for(int m=0;m<this->rows();m++){
			tmp=0;
			for(int j=0;j<this->cols();j++;){
				k=i*this->cols()+j;
				l=j*this->rows()+m;
				tmp+=this->m_plain_object[k]*other->m_plain_object[l];
			}
			other->m_plain_object[m*this->rows()+i];
		}
	}
  }

  Index rows() const { return m_rows; }
  Index cols() const { return m_cols; }
private:
  std::vector<value_type> m_plain_object;
  Index m_rows = 0;//行数
  Index m_cols = 0;//列数
};

} // end of namespace DistMat
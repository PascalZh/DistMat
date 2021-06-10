#pragma once
#include <functional>

namespace multiplication {

/// A = A * B
/// \param n        A and B are both nxn matrices
/// \param tmp      a zero n-dimension vector for temporal storage of middle results
/// \param coeffVec use coeffVec(vec, i) to get the coefficient of a vector
/// \param coeff    use coeff(A, i, j) to get the coefficient of a matrix
template<typename _S, typename Index,
  template<typename Scalar_> class Mat,
  template<typename Scalar_> class Vec>
void multiplyMatrixRightToInplace(
  Mat<_S>& A, Mat<_S>& B, Index n, Vec<_S>& tmp,
  std::function<_S& (Vec<_S>&, Index)>
  coeffVec = [](Vec<_S>& v, Index i) -> _S& { return v[i]; },
  std::function<_S& (Mat<_S>&, Index, Index)>
  coeff = [](Mat<_S>& mat, Index row, Index col) -> _S& { return mat(row, col); })
{
  for (Index i = 0; i < n; i++) {
    for (Index j = 0; j < n; j++) {
      coeffVec(tmp, j) = 0;
    }
    for (Index j = 0; j < n; j++) {
      for (Index m = 0; m < n; m++) {
        coeffVec(tmp, j) += coeff(A, i, m) * coeff(B, m, j);
      }
    }
    for (Index j = 0; j < n; j++) {
      coeff(A, i, j) = coeffVec(tmp, j);
    }
  }
}

/// A = B * A
/// \see multiplyMatrixRightToInplace
template<typename _S, typename Index,
  template<typename Scalar_> class Mat,
  template<typename Scalar_> class Vec>
void multiplyMatrixLeftToInplace(
  Mat<_S>& A, Mat<_S>& B, Index n, Vec<_S>& tmp,
  std::function<_S& (Vec<_S>&, Index)>
  coeffVec = [](Vec<_S>& v, Index i) -> _S& { return v[i]; },
  std::function<_S& (Mat<_S>&, Index, Index)>
  coeff = [](Mat<_S>& mat, Index row, Index col) -> _S& { return mat(row, col); })
{
  for (Index j = 0; j < n; j++) {
    for (Index i = 0; i < n; i++) {
      coeffVec(tmp, i) = 0;
    }
    for (Index i = 0; i < n; i++) {
      for (Index m = 0; m < n; m++) {
        coeffVec(tmp, i) += coeff(B, i, m) * coeff(A, m, j);
      }
    }
    for (Index i = 0; i < n; i++) {
      coeff(A, i, j) = coeffVec(tmp, i);
    }
  }
}

} // namespace multiplication
#pragma once
#include <functional>

namespace multiplication {

/// A = A * B
/// \param n        A and B are both nxn matrices
/// \param tmp      a zero n-dimension vector for temporal storage of middle results
/// \param coeffVec use coeffVec(vec, i) to get the coefficient of a vector
/// \param coeff    use coeff(A, i, j) to get the coefficient of a matrix
template<class Scalar, class Index, class Mat, class Vec>
void multiplyMatrixRightToInplace(
  Mat& A, Mat& B, Index n, Vec& tmp,
  std::function<Scalar& (Vec&, Index)>
  coeffVec = [](Vec& v, Index i) -> Scalar& { return v[i]; },
  std::function<Scalar& (Mat&, Index, Index)>
  coeff = [](Mat& mat, Index row, Index col) -> Scalar& { return mat(row, col); })
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
template<class Scalar, class Index, class Mat, class Vec>
void multiplyMatrixLeftToInplace(
  Mat& A, Mat& B, Index n, Vec& tmp,
  std::function<Scalar& (Vec&, Index)>
  coeffVec = [](Vec& v, Index i) -> Scalar& { return v[i]; },
  std::function<Scalar& (Mat&, Index, Index)>
  coeff = [](Mat& mat, Index row, Index col) -> Scalar& { return mat(row, col); })
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
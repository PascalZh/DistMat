#pragma once
#include <functional>

namespace mul {

/// A = A * B
/// A and B are both nxn matrices
/// \param tmp      a n-dimension vector for temporal storage of middle results, must be zero before calling
template<typename Index>
constexpr void MultiplyMatrixRightToInplace(auto& A, auto& B, auto& tmp)
{
  const Index n = A.rows();
  for (Index i = 0; i < n; i++) {
    for (Index j = 0; j < n; j++) { // initialize temporal vector
      tmp[j] = 0;
    }
    for (Index j = 0; j < n; j++) { // evaluate some line, and save to vector
      for (Index m = 0; m < n; m++) {
        tmp[j] += A(i, m) * B(m, j);
      }
    }
    for (Index j = 0; j < n; j++) { // assign the vector to the matrix
      A(i, j) = tmp[j];
    }
  }
}

/// A = B * A
/// \see MultiplyMatrixRightToInplace
template<typename Index>
constexpr void MultiplyMatrixLeftToInplace(auto& A, auto& B, auto& tmp)
{
  const Index n = A.rows();
  for (Index j = 0; j < n; j++) {
    for (Index i = 0; i < n; i++) {
      tmp[i] = 0;
    }
    for (Index i = 0; i < n; i++) {
      for (Index m = 0; m < n; m++) {
        tmp[i] += B(i, m) * A(m, j);
      }
    }
    for (Index i = 0; i < n; i++) {
      A(i, j) = tmp[i];
    }
  }
}

/// C = A * B
/// \param A nxm matrix
/// \param B mxs matrix
/// \param C nxs matrix
template<class Index>
constexpr void multiplyMatrix(auto& A, auto& B, auto& C)
{
  const Index n = A.rows();
  const Index m = A.cols();
  const Index s = B.cols();
  for (Index i = 0; i < n; ++i) {
    for (Index j = 0; j < s; ++j) {
      for (Index k = 0; k < m; ++k) {
        C(i, j) += A(i, k) * B(k, j);
      }
    }
  }
}

} // namespace mul
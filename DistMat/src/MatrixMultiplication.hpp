#pragma once

template<typename Vec, typename Index, typename Func>
concept CoeffVec = requires (Vec vec, Index idx, Func coeff)
{
  coeff(vec, idx);
};
template<typename Mat, typename Index, typename Func>
concept Coeff = requires (Mat mat, Index col, Index row, Func coeff)
{
  coeff(mat, col, row);
};
/// A = A * B
/// \param n        A and B are both nxn matrices
/// \param tmp      a zero n-dimension vector for temporal storage of middle results
/// \param coeffVec use coeffVec(vec, i) to get the coefficient of a vector
/// \param coeff    use coeff(A, i, j) to get coefficient of a matrix
template<typename Index, typename Mat, typename Vec>
void multiplyMatrixRightToInplace(
  Mat& A, Mat& B, Index n, Vec& tmp,
  CoeffVec<Vec, Index> auto coeffVec = [](Vec& v, Index i) { return v[i]; },
  Coeff<Mat, Index> auto coeff = [](Mat& mat, Index row, Index col) { return mat(row, col); })
{
    for (Index i = 0; i < n; i++) {
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
/// \sa multiplyMatrixRightToInplace
template<typename Index, typename Mat, typename Vec>
void multiplyMatrixLeftToInplace(Mat& A, Mat& B, Index n, Vec& tmp, auto entryVec, auto entry)
{
    for (Index j = 0; j < n; j++) {
      for (Index i = 0; i < n; i++) {
        for (Index m = 0; m < n; m++) {
          entryVec(tmp, i) += entry(B, i, m) * entry(A, m, j);
        }
      }
      for (Index i = 0; i < n; i++) {
        entry(A, i, j) = entryVec(tmp, i);
      }
    }
}
#pragma once

/// A = A * B
///\param n        A and B are both nxn matrices
///\param tmp      a zero n-dimension vector for temporal storage of middle results
///\param entryVec use entryVec(vec, i) to get entries of a vector
///\param entry    use entry(A, i, j) to get entries of a matrix
template<typename Index, typename Mat, typename Vec>
void multiplyMatrixRightToInplace(Mat& A, Mat& B, Index n, Vec& tmp, auto entryVec, auto entry)
{
    for (Index i = 0; i < n; i++) {
      for (Index j = 0; j < n; j++) {
        for (Index m = 0; m < n; m++) {
          entryVec(tmp, j) += entry(A, i, m) * entry(B, m, j);
        }
      }
      for (Index m = 0; m < n; m++) {
        entry(A, i, m) = entryVec(tmp, j);
      }
    }
}
# DistMat
A distributed matrix library for easy use.

# comma initializer
We mimic the Eigen comma initializer syntax. But instead of using `operator<<` for assignment, we think `operator=` is more natural. Here is an example.
```cpp
Matrix<double> A(3, 3);  // declare a 3x3 matrix A, memory is allocated, but the value is undetermined
A = 1, 2, 3,
    4, 5, 6,
    7, 8, 9;
```
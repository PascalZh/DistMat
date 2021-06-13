# DistMat
A distributed matrix library for easy use.

# class `Matrix`
`Matrix` now have first 3 template parameters fixed, may not change in the future:
```cpp
template<typename Scalar, int Rows, int Cols>
class Matrix;
```
Other parameters are likely to be changed in the future evolution.

## Default template parameters
Default template parameters is provided in forward declaration, so in the definition of `Matrix`, full parameters list are required.

## Heap vs. Stack
Two version of `Matrix` is provided:
- in the case of `Rows > 0 && Cols > 0`, use stack version of Matrix
- in the case of `Rows == -1 && Cols == -1`, use heap version of Matrix
- otherwise, error

For some member functions/member variables, two versions is provided, differed by concept or SFINAE. Here are some examples:
```cpp
// make a non-template function to a template
// different conditions are provided in various form
template<typename T = Scalar>
  requires is_same_v<T, Scalar> && (Rows == -1) && (Cols == -1)  // here, conditions for heap version
Matrix(Index rows, Index cols)
  : m_storage(rows * cols), m_shape{rows, cols} {}

template<typename T = Scalar>
  requires is_same_v<T, Scalar>
constexpr explicit Matrix(InternalStorage storage)
  : m_storage{std::move(storage)}
{
  if constexpr(!(Rows > 0 && Cols > 0)) {  // here, conditions for stack version
    static_assert(!is_same_v<T, T>, "Can not initialize the matrix without shape specified!");
  }
}
```
Other common functions are shared.

### Stack version
Stack version of `Matrix` is provided to be `constexpr`, matrix plus/minus/multiplication are capable of being evaluated at compile time.
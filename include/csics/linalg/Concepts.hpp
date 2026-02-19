#pragma once
#include <concepts>

namespace csics::linalg {
template <typename T>
concept ComplexLike = requires(T a) {
    typename T::value_type;
    { a.real() } -> std::convertible_to<typename T::value_type>;
    { a.imag() } -> std::convertible_to<typename T::value_type>;
    { T(a.real(), a.imag()) } -> std::same_as<T>;
};

template <typename T>
concept ComplexPrimitive = std::is_arithmetic_v<T> && std::is_signed_v<T>;

template <typename T>
concept VecPrimitive = std::is_arithmetic_v<T>;

template <typename T>
concept ScalarLike = std::is_arithmetic_v<T>;  // maybe expanded later

template <typename T>
concept FloatScalarLike = std::floating_point<T> && ScalarLike<T>;

template <typename T>
concept Vec2Like = requires(T a) {
    typename T::value_type;
    { a.x() } -> std::same_as<typename T::value_type>;
    { a.y() } -> std::same_as<typename T::value_type>;
} && T::size_v == std::integral_constant<std::size_t, 2>{};

template <typename T>
concept Vec3Like = requires(T a) {
    typename T::value_type;
    { a.x() } -> std::same_as<typename T::value_type>;
    { a.y() } -> std::same_as<typename T::value_type>;
    { a.z() } -> std::same_as<typename T::value_type>;
} && T::size_v == std::integral_constant<std::size_t, 3>{};

template <typename T>
concept Vec4Like = requires(T a) {
    typename T::value_type;
    { a.x() } -> std::same_as<typename T::value_type>;
    { a.y() } -> std::same_as<typename T::value_type>;
    { a.z() } -> std::same_as<typename T::value_type>;
    { a.w() } -> std::same_as<typename T::value_type>;
} && T::size_v == std::integral_constant<std::size_t, 4>{};

template <typename T>
concept StaticVecLike =
    (Vec2Like<T> || Vec3Like<T> || Vec4Like<T>) && requires { T::size; };

template <typename T, typename U>
concept VecCompatible =
    StaticVecLike<T> && StaticVecLike<U> && T::size == U::size &&
    std::same_as<typename T::value_type, typename U::value_type>;

template <typename Mat>
concept SmallMatrix = Mat::rows_v <= 4 && Mat::cols_v <= 4;

template <typename Mat>
concept SquareMatrix = Mat::rows_v == Mat::cols_v;

template <typename Mat>
concept SmallSquareMatrix = SmallMatrix<Mat> && SquareMatrix<Mat>;

template <typename MatA, typename MatB>
concept MatrixCompatible =
    MatA::cols_v == MatB::rows_v && MatA::value_type == MatB::value_type;

template <typename Mat>
concept Matrix2x2 = Mat::rows_v == 2 && Mat::cols_v == 2;
template <typename Mat>
concept Matrix3x3 = Mat::rows_v == 3 && Mat::cols_v == 3;

};  // namespace csics::linalg

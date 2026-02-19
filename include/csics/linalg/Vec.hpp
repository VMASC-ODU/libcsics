#pragma once

#include <csics/linalg/Concepts.hpp>

#include <concepts>
#include <cstddef>
#include <tuple>
namespace csics::linalg {

template <VecPrimitive T, std::size_t N>
class Vec {
   public:
    using value_type = T;
    static constexpr std::size_t size_v = N;

    Vec() = default;
    template <typename... Args>
        requires(sizeof...(Args) == N) && (std::same_as<Args, T> && ...)
    Vec(Args... xs) : data_{xs...} {}

    template <std::size_t I>
    constexpr const T get() const noexcept {
        static_assert(I < N, "Index out of bounds");
        return data_[I];
    }

    consteval std::size_t size() { return N; }

   private:
    T data_[N];
};

template <VecPrimitive T>
class Vec2 {
   public:
    using value_type = T;
    static constexpr std::size_t size_v = 2;

    consteval std::size_t size() { return 2; }

    Vec2() = default;
    Vec2(T xs) : data_(xs, xs) {}
    Vec2(T x, T y) : data_(x, y) {}

    constexpr const T x() const noexcept { return data_[0]; }
    constexpr const T y() const noexcept { return data_[1]; }
    constexpr T x() noexcept { return data_[0]; }
    constexpr T y() noexcept { return data_[1]; }

    template <std::size_t I>
    constexpr const T get() const noexcept {
        static_assert(I < 2, "Index out of bounds");
        return data_[I];
    }

    Vec2(Vec<T, 2> v) : data_{v.template get<0>(), v.template get<1>()} {}

   private:
    T data_[2];
};

template <VecPrimitive T>
class Vec3 {
   public:
    using value_type = T;
    static constexpr std::size_t size_v = 3;

    consteval std::size_t size() { return 3; }

    Vec3() = default;
    Vec3(T xs) : data_(xs, xs, xs) {}
    Vec3(T x, T y, T z) : data_(x, y, z) {}
    Vec3(Vec<T, 3> v)
        : data_{v.template get<0>(), v.template get<1>(), v.template get<2>()} {}

    constexpr const T x() const noexcept { return data_[0]; }
    constexpr const T y() const noexcept { return data_[1]; }
    constexpr const T z() const noexcept { return data_[2]; }
    constexpr T x() noexcept { return data_[0]; }
    constexpr T y() noexcept { return data_[1]; }
    constexpr T z() noexcept { return data_[2]; }

    template <std::size_t I>
    constexpr const T get() const noexcept {
        static_assert(I < 3, "Index out of bounds");
        return data_[I];
    }

   private:
    T data_[3];
};

template <VecPrimitive T>
class Vec4 {
   public:
    using value_type = T;
    static constexpr std::size_t size_v = 4;

    consteval std::size_t size() { return 4; }

    Vec4() = default;
    Vec4(T xs) : data_(xs, xs, xs, xs) {}
    Vec4(T x, T y, T z, T w) : data_(x, y, z, w) {}
    Vec4(Vec<T, 4> v)
        : data_{v.template get<0>(), v.template get<1>(),
                v.template get<2>(), v.template get<3>()} {}

    constexpr const T x() const noexcept { return data_[0]; }
    constexpr const T y() const noexcept { return data_[1]; }
    constexpr const T z() const noexcept { return data_[2]; }
    constexpr const T w() const noexcept { return data_[3]; }
    constexpr T x() noexcept { return data_[0]; }
    constexpr T y() noexcept { return data_[1]; }
    constexpr T z() noexcept { return data_[2]; }
    constexpr T w() noexcept { return data_[3]; }

    template <std::size_t I>
    constexpr const T get() const noexcept {
        static_assert(I < 4, "Index out of bounds");
        return data_[I];
    }

   private:
    T data_[4];
};


static_assert(Vec2Like<Vec2<float>>);
static_assert(!Vec2Like<Vec3<float>>);
static_assert(!Vec2Like<Vec4<double>>);
static_assert(Vec3Like<Vec3<double>>);
static_assert(!Vec3Like<Vec2<int>>);
static_assert(!Vec3Like<Vec4<float>>);
static_assert(Vec4Like<Vec4<int>>);
static_assert(!Vec4Like<Vec2<double>>);
static_assert(!Vec4Like<Vec3<float>>);

template <typename VecA, typename VecB, std::size_t... Is>
    requires VecCompatible<VecA, VecB>
constexpr auto vec_add_impl(VecA&& a, VecB&& b, std::index_sequence<Is...>) {
    using T = typename VecA::value_type;
    return VecA{std::array<T, VecA::size_v_v>{
        (std::forward<VecA>(a).template get<Is>() +
         std::forward<VecB>(b).template get<Is>())...}};
}
template <typename VecA, typename VecB>
    requires VecCompatible<VecA, VecB>
constexpr auto operator+(VecA&& a, VecB&& b) {
    return vec_add_impl(std::forward<VecA>(a), std::forward<VecB>(b),
                        std::make_index_sequence<VecA::size_v>{});
}

template <typename VecA, typename VecB, std::size_t... Is>
    requires csics::linalg::VecCompatible<VecA, VecB>
constexpr auto vec_sub_impl(VecA&& a, VecB&& b, std::index_sequence<Is...>) {
    using T = typename VecA::value_type;
    return VecA{std::array<T, VecA::size_v>{
        (std::forward<VecA>(a).template get<Is>() -
         std::forward<VecB>(b).template get<Is>)...}};
}

template <typename VecA, typename VecB>
    requires csics::linalg::VecCompatible<VecA, VecB>
constexpr auto operator-(VecA&& a, VecB&& b) {
    return vec_sub_impl(std::forward<VecA>(a), std::forward<VecB>(b),
                        std::make_index_sequence<VecA::size_v>{});
}

template <typename VecA, typename Scalar, std::size_t... Is>
    requires csics::linalg::StaticVecLike<VecA> && ScalarLike<Scalar>
constexpr auto vec_scalar_mul_impl(VecA&& v, Scalar s,
                                   std::index_sequence<Is...>) {
    using T = typename VecA::value_type;
    return VecA{std::array<T, VecA::size_v>{
        (std::forward<VecA>(v).template get<Is>() * s)...}};
}

template <typename VecA, typename Scalar>
    requires csics::linalg::StaticVecLike<VecA> && ScalarLike<Scalar>
constexpr auto operator*(VecA&& v, Scalar s) {
    return vec_scalar_mul_impl(std::forward<VecA>(v), s,
                               std::make_index_sequence<VecA::size_v>{});
}

template <typename VecA, typename Scalar>
    requires csics::linalg::StaticVecLike<VecA> && ScalarLike<Scalar>
constexpr auto operator*(Scalar s, VecA&& v) {
    return vec_scalar_mul_impl(std::forward<VecA>(v), s,
                               std::make_index_sequence<VecA::size_v>{});
}

template <typename VecA, typename Scalar, std::size_t... Is>
    requires csics::linalg::StaticVecLike<VecA> && ScalarLike<Scalar>
constexpr auto vec_scalar_div_impl(VecA&& v, Scalar s,
                                   std::index_sequence<Is...>) {
    using T = typename VecA::value_type;
    return VecA{std::array<T, VecA::size_v>{
        (std::forward<VecA>(v).template get<Is>() / s)...}};
}

};  // namespace csics::linalg

namespace std {

template <size_t I, csics::linalg::StaticVecLike T>
constexpr typename T::value_type& get(const T& v) noexcept {
    return v.template get<I>(v);
}

template <size_t I, csics::linalg::StaticVecLike T>
constexpr typename T::value_type& get(T& v) noexcept {
    return v.template get<I>(v);
}

template <size_t I, csics::linalg::StaticVecLike T>
constexpr typename T::value_type&& get(T&& v) noexcept {
    return forward<T>(v).template get<I>(forward<T>(v));
}

template <csics::linalg::StaticVecLike VecA, csics::linalg::StaticVecLike VecB,
          size_t... Is>
    requires csics::linalg::VecCompatible<VecA, VecB>
constexpr auto vec_swap_impl(VecA&& a, VecB&& b,
                             std::index_sequence<Is...>) noexcept {
    using std::swap;
    (swap(std::forward<VecA>(a).template get<Is>(),
          std::forward<VecB>(b).template get<Is>()),
     ...);
}

template <csics::linalg::StaticVecLike T>
struct tuple_size<T> : std::integral_constant<std::size_t, T::size> {};

template <size_t I, csics::linalg::StaticVecLike T>
struct tuple_element<I, T> {
    using type = typename T::value_type;
};
};  // namespace std

#pragma once
#include <cmath>
#include <csics/linalg/Concepts.hpp>
#include <tuple>

namespace csics::linalg {

// multiply and accumulate
// a * b + c
class Mac {
   public:
    template <typename T>
    constexpr auto operator()(const T& a, const T& b,
                              const T& c) const noexcept {
        return apply(a, b, c);
    }

    template <ScalarLike Scalar>
    static auto apply(const Scalar& a, const Scalar& b, const Scalar& c) {
        return a * b + c;
    }

    template <FloatScalarLike Scalar>
    static auto apply(const Scalar& a, const Scalar& b, const Scalar& c) {
        return std::fma(a, b, c);
    }
};

constexpr Mac mac;

class Dot {
   public:
    template <typename T>
    constexpr auto operator()(const T& a, const T& b) const noexcept {
        return apply(a, b);
    }

    template <StaticVecLike VecU>
    static auto apply(const VecU& a, const VecU& b) {
        return std::apply(
            [&]<std::size_t... Is>(std::index_sequence<Is...>) {
                return ((a.template get<Is>(a) * b.template get<Is>(b)) + ...);
            },
            std::make_index_sequence<VecU::size_v>{});
    };
};

class Cross {
   public:
    template <typename T>
    constexpr auto operator()(T&& a, T&& b) const noexcept {
        return apply(std::forward<T>(a), std::forward<T>(b));
    }

    template <Vec3Like Vec3U>
    static auto apply(Vec3U&& a, Vec3U&& b) {
        return Vec3U(a.y() * b.z() - a.z() * b.y(),
                     a.z() * b.x() - a.x() * b.z(),
                     a.x() * b.y() - a.y() * b.x());
    };
};

class Mag {
   public:
    template <typename T>
    constexpr auto operator()(const T& v) const noexcept {
        return apply(v);
    }

    template <StaticVecLike VecU>
    static auto apply(const VecU& v) {
        return std::sqrt(std::apply(
            [&]<std::size_t... Is>(std::index_sequence<Is...>) {
                return ((v.template get<Is>(v) * v.template get<Is>(v)) + ...);
            },
            std::make_index_sequence<VecU::size_v>{}));
    }
};

class Abs {
   public:
    template <typename T>
    constexpr auto operator()(const T& v) const noexcept {
        return apply(v);
    }

    template <StaticVecLike VecU>
    static auto apply(const VecU& v) {
        return std::sqrt(std::apply(
            [&]<std::size_t... Is>(std::index_sequence<Is...>) {
                return ((v.template get<Is>(v) * v.template get<Is>(v)) + ...);
            },
            std::make_index_sequence<VecU::size_v>{}));
    }
};

class Conj {
   public:
    template <typename T>
    constexpr auto operator()(const T& v) const noexcept {
        return apply(v);
    }

    template <ComplexLike ComplexU>
    static auto apply(const ComplexU& c) {
        return ComplexU(c.real(), -c.imag());
    }
};

constexpr Dot dot;
constexpr Cross cross;

};  // namespace csics::linalg

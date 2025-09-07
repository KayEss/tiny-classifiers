/**
{{field created 2025-08-20T03:42:31.726Z}}
{{field lastUpdate 2025-09-07T04:55:36.116Z}}
{{field displayName Tiny Classifiers/array.hpp}}
 */
#pragma once


#include <array>
#include <concepts>
#include <utility>


/// ## `array_of` -- Create an array of `N` values
template<std::size_t N, std::invocable<std::size_t> V>
constexpr inline auto array_of(V &&v) {
    return [&]<std::size_t... Indices>(
                   std::integer_sequence<std::size_t, Indices...>) {
        return std::array{v(Indices)...};
    }(std::make_index_sequence<N>{});
}

template<std::size_t N, std::invocable<> V>
constexpr inline auto array_of(V &&v) {
    return array_of<N>([&](std::size_t) { return v(); });
}

template<std::size_t N, std::copyable V>
constexpr inline auto array_of(V const &v) {
    return array_of<N>([&](std::size_t) { return v; });
}

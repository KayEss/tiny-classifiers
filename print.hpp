/**
{{field created 2025-08-22T08:00:43.682Z}}
{{field lastUpdate 2025-08-22T08:31:38.287Z}}
{{field displayName Tiny Classifiers/print.hpp}}
 */
#pragma once


#include <iomanip>
#include <iostream>


/// Print an array of some printable type with an optional separator
template<typename V, std::size_t N>
inline std::ostream &print(
        std::ostream &os, std::array<V, N> const &a, char const sep = ' ') {
    os << a[0];
    by_index(1, a.size(), [&](auto const index) { os << sep << a[index]; });
    return os;
}

/// Print an array of some printable type
template<typename V, std::size_t N>
inline std::ostream &operator<<(std::ostream &os, std::array<V, N> const &a) {
    return print(os, a);
}

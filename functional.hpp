/**
{{field created 2025-08-21T06:54:17.304Z}}
{{field lastUpdate 2025-08-23T04:02:18.400Z}}
{{field displayName Tiny Classifiers/functional.hpp}}
 */
#pragma once


#include <cstddef>


template<typename Lambda>
inline auto by_index(
        std::size_t const start_index,
        std::size_t const max_index,
        Lambda &&lambda) {
    for (std::size_t index{start_index}; index < max_index; ++index) {
        lambda(index);
    }
}
template<typename Lambda>
inline auto by_index(std::size_t const max_index, Lambda &&lambda) {
    return by_index({}, max_index, std::forward<Lambda>(lambda));
}

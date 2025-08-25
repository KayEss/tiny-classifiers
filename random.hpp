/**
{{field created 2025-08-18T09:47:34.247Z}}
{{field lastUpdate 2025-08-18T09:52:57.824Z}}
{{field displayName Tiny Classifiers/random.hpp}}
*/
#pragma once


#include <random>


inline thread_local std::random_device device;
inline thread_local std::mt19937 generator(device());


/// Pick a number between zero and less than `upto`
inline std::size_t pick_index(std::size_t const upto) {
    return std::uniform_int_distribution<std::size_t>{0, upto - 1}(generator);
}


/// Linear random number between ±1
inline float random_weight() {
    return std::uniform_real_distribution<float>{-1.0f, 1.0f}(generator);
}

/**
{{field created 2025-08-20T03:43:59.802Z}}
{{field lastUpdate 2025-08-20T10:08:02.737Z}}
{{field displayName Tiny Classifiers/edf.hpp}}
*/
#pragma once


#include "array.hpp"


class exponential_decay final {
    float value{}, decay_rate;

  public:
    exponential_decay(float half_life)
    : decay_rate{std::powf(2, -1.0f / half_life)} {}

    void adjust(float v) {
        auto const a = (1.0f - decay_rate) * v;
        value = value * decay_rate + a;
    }

    float operator()() const noexcept { return value; }
};

std::ostream &operator<<(std::ostream &os, exponential_decay const &ed) {
    return os << ed();
}

template<std::size_t N>
void adjust(std::array<exponential_decay, N> &a, float const v) {
    for (auto &ed : a) { ed.adjust(v); }
}

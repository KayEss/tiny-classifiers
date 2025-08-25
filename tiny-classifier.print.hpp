/**
{{field created 2025-08-23T04:17:01.041Z}}
{{field lastUpdate 2025-08-24T06:13:56.729Z}}
{{field displayName Tiny Classifiers/tiny-classifier.print.hpp}}
 */
#pragma once


#include "print.hpp"


template<std::size_t Weights>
std::ostream &operator<<(std::ostream &os, neuron<Weights> const &n) {
    return os << "{" << n.bias << " [" << n.weights << "]}";
}


auto city_name(std::size_t const city_index) {
    return std::get<0>(cities[city_index]);
}


template<std::size_t Inputs, std::size_t Cities>
std::ostream &
        operator<<(std::ostream &os, ::result<Inputs, Cities> const &result) {
    if (result.example.city_index == result.answer) {
        os << "✅ ";
    } else {
        os << "X  ";
    }
    return os << city_name(result.example.city_index) << " -> "
              << city_name(result.answer) << " at "
              << (100 * result.probabilities[result.answer])
              << "%\nProbabilities [" << result.probabilities << "] Loss "
              << result.loss << '\n';
}


template<std::size_t Weights, std::size_t Cities>
std::ostream &operator<<(std::ostream &os, ::model<Weights, Cities> const &m) {
    os << "Generation " << m.generation << "\nCorrectness: ";
    ::print(os, m.success_rate, '/') << "\nLosses: ";
    ::print(os, m.losses, '/') << '\n';
    return os << m.network << '\n';
}

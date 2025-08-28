cmake_minimum_required(VERSION 3.6)
project(tiny-classifiers)


add_executable(tiny-classifier
        tiny-classifier.cpp
    )
target_compile_features(tiny-classifier PRIVATE cxx_std_26)
target_compile_options(
        tiny-classifier PUBLIC
        -Werror
        -fdiagnostics-color=always
        -fdiagnostics-show-option
        -Wall -Wextra -Wpedantic
    )

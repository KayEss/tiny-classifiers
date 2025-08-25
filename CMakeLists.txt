cmake_minimum_required(VERSION 3.6)
project(tiny-classifiers)


add_executable(tiny-classifier
        tiny-classifier.cpp
    )
target_compile_features(tiny-classifier PRIVATE cxx_std_26)

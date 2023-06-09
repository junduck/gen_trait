#!/bin/bash

python3 ../gen_trait.py ./sample.json > ./sample.hpp
clang-format -i ./sample.hpp

clang -std=c++17 sample.cpp -lc++ -o sample.out

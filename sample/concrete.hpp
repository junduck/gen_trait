#pragma once

#include <iostream>
#include <string>
#include <tuple>

namespace jduck::gen_trait::sample {
template <size_t I> class packer {
  std::tuple<int, int, size_t> v;

public:
  std::tuple<int, int, size_t> pack(int a, int b, size_t c) {
    v = {a, b, c};
    return v;
  }
  auto get() const { return std::get<I>(v); }
};

class mysignal {
  int val;

public:
  void send(int val) { this->val = val; }
  int get_val() const { return val; }
};

class mysignal2 {
  int val{};
  std::string name;

public:
  mysignal2(std::string name) : name{std::move(name)} {}
  void send(int val) {
    this->val = val * 2;
    std::cout << "send: " << name << std::endl;
  }
  int get_val() const { return val; }
};
} // namespace jduck::gen_trait::sample
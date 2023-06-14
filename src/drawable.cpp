#include "gtest/gtest.h"
#include "drawable.hpp"
#include <vector>

struct square {
  void draw(std::ostream &os) const {
    os << "square";
  }
  void draw_cap(std::ostream &os) const {
    os << "SQUARE";
  }
};

struct circle {
  void draw(std::ostream &os) const {
    os << "circle";
  }
  void draw_cap(std::ostream &os) const {
    os << "CIRCLE";
  }
};

TEST(drawable, same_old_cliche_drawable) {
  using namespace jduck::gen_trait::sample;
  std::vector<drawable> v;
  v.emplace_back(square{});
  v.emplace_back(circle{});
  std::ostringstream oss;
  for (auto &d : v) {
    // print on oss for gtest
    d.draw(oss);
    // also print on stdout
    std::cout << "drawing: ";
    d.draw(std::cout);
    std::cout << std::endl;
    d.draw_cap(std::cout);
    std::cout << std::endl;
  }
  EXPECT_EQ(oss.str(), "squarecircle");
}

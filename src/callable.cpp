#include "gtest/gtest.h"
#include "callable.hpp"

#include <unordered_set>

struct stateful_callable {
  int i;

  int operator()(int j) {
    i += j;
    return i;
  }
};

struct stateful_callable2 {
  int i;

  int operator()(int j) {
    i += j * 2;
    return i;
  }
};

TEST(callable, basic) {
  using namespace jduck::gen_trait::sample;
  stateful_callable s{};

  // can construct callable_ref from object, this ctor can be implicit
  callable_ref<int, int> cr = s;
  // can invoke callable_ref as if it is stateful_callable
  EXPECT_EQ(cr(1), 1);
  EXPECT_EQ(cr(2), 3);
  // this is a ref, not a copy
  EXPECT_EQ(s.i, 3);

  // can construct callable from object, this ctor can be implicit
  callable<int, int> c = s; // can move in as well: callable<int, int> c = std::move(s);
  // can invoke
  EXPECT_EQ(c(1), 4);
  EXPECT_EQ(c(2), 6);
  // this is a copy, not a ref
  EXPECT_EQ(s.i, 3);

  // can construct callable shared from object, this ctor can be implicit
  callable_shared<int, int> cs = s; // can move in as well: callable_shared<int, int> cs = std::move
  // shares same underlying object just like std::shared_ptr
  auto cs1 = cs;
  // can invoke
  EXPECT_EQ(cs(1), 4);
  EXPECT_EQ(cs(2), 6);
  // copy is shared
  EXPECT_EQ(cs1(1), 7);
  EXPECT_EQ(cs1(2), 9);
  // this is a copy, not a ref
  EXPECT_EQ(s.i, 3);

  auto sp = std::make_unique<stateful_callable>();
  // can construct callable ref from raw ptr, noted ctor from ptr is explicit
  callable_ref<int, int> cr2(sp.get());
  // can construct from unique_ptr, noted ctor from ptr is explicit
  callable_ref<int, int> cr3(sp); // same for std::shared_ptr, not tested here
  // ref to same obj
  EXPECT_EQ(cr2(1), 1);
  EXPECT_EQ(cr3(2), 3);

  // can construct callable shared from unique_ptr, noted ctor from ptr is explicit
  callable_shared<int, int> cs2(std::move(sp));
  // other ops, incl. ctor, just same as how std::shared_ptr behaves
  cs = cs2;
  // moved in
  EXPECT_EQ(cs2(1), 4);
  EXPECT_EQ(cs2(2), 6);
  // copy is shared
  EXPECT_EQ(cs(1), 7);
  EXPECT_EQ(cs(2), 9);

  sp = std::make_unique<stateful_callable>();
  // can construct callable from unique ptr
  callable<int, int> c2(std::move(sp));
  EXPECT_EQ(c2(1), 1);
  EXPECT_EQ(c2(2), 3);
  // other ops, incl. ctor, just same as how std::unique_ptr behaves
  c = std::move(c2);
  // moved in
  EXPECT_EQ(c(1), 4);
  EXPECT_EQ(c(2), 6);

  // other members are also trivial, not tested
}

TEST(callable, hashable) {
  using namespace jduck::gen_trait::sample;

  stateful_callable s1{};
  stateful_callable s2{};
  stateful_callable2 s3{};

  // can be used in containers as usual value types
  std::unordered_set<callable_ref<int, int>> crs;

  crs.insert(s1);
  crs.insert(s2);
  crs.insert(s1);
  crs.insert(s3);
  // hashable
  EXPECT_EQ(crs.size(), 3);

  for (auto &cr : crs) {
    cr(1);
  }
  EXPECT_EQ(s1.i, 1);
  EXPECT_EQ(s2.i, 1);
  EXPECT_EQ(s3.i, 2);
}

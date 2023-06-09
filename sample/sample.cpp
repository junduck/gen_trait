#include "sample.hpp"
#include "concrete.hpp"
#include <unordered_map>
#include <vector>

using namespace jduck::gen_trait::sample;

using tmap = std::unordered_map<pack_and_get<2, int, int, size_t>, int>;

int main() {
  tmap m;
  m.emplace(new packer<2>{}, 4);

  std::vector<sendable_ref> v;
  mysignal s1{};
  auto s2 = std::make_shared<mysignal2>("s2");
  sendable s3(new mysignal());
  sendable_shared s4(new mysignal2("s4"));
  auto s5p = std::make_unique<mysignal2>("s5");
  sendable s5(std::move(s5p));
  sendable s6(new mysignal2("s6"));
  s5 = std::move(s6);
  v.emplace_back(&s1);
  v.emplace_back(s2);
  v.emplace_back(s3);
  v.emplace_back(s4);
  v.emplace_back(s5);

  for (auto &s : v) {
    s.send(3);
  }
  std::cout << s1.get_val() << std::endl;
  std::cout << s2->get_val() << std::endl;
}
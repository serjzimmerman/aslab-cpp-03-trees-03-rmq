#include <chrono>
#include <iostream>

#include "cartesian_tree.hpp"
#include "disjoint_map_forest.hpp"
#include "disjoint_set_forest.hpp"

#include <cstdlib>

int main() {
  throttle::disjoint_map_forest<int, bool> set;

  for (int i = 0; i < 128; ++i) {
    set.make_set(i, false);
  }

  for (int i = 0; i < 64; ++i) {
    int index1 = rand() % 128;
    int index2 = rand() % 128;
    set.union_set(index1, index2);
  }

  // std::cout << set;

  throttle::cartesian_tree<int> tree;
  std::vector<int> test_case;
  std::generate_n(std::back_inserter(test_case), 1 << 8, []() { return std::rand() % (1 << 6); });

  for (const auto &v : test_case) {
    tree.append(v);
  }

  std::cout << tree;
}
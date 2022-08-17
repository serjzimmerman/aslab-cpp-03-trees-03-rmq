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
  tree.append(4);
  tree.append(1);
  tree.append(2);
  tree.append(4);
  tree.append(4);
  tree.append(5);
  tree.append(2);
  tree.append(8);
  tree.append(1);

  std::cout << tree;
}
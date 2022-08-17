#include <chrono>
#include <iostream>

#include "disjoint_set_forest.hpp"
#include "disjoint_map_forest.hpp"
#include <cstdlib>

int main() {
  throttle::disjoint_map_forest<int, bool> set;

  for (int i = 0; i < 128; ++i) {
    set.make_set(i, false);
  }

  for (int i = 0; i < 128; ++i) {
    int index1 = rand() % 128;
    int index2 = rand() % 128;
    set.union_set(index1, index2);
  }

  std::cout << set;
}
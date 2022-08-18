#include <chrono>
#include <iostream>

#include "cartesian_map.hpp"
#include "cartesian_set.hpp"

#include "disjoint_map_forest.hpp"
#include "disjoint_set_forest.hpp"
#include "offline_rmq.hpp"

template class throttle::cartesian_set<int>;
template class throttle::cartesian_map<int, bool>;
#include <cstdlib>

#if 0
namespace {
template <typename t_proxy> void inorder_walk_helper(t_proxy node) {
  if (!node) return;
  inorder_walk_helper(node.left());
  std::cout << node->first << " -- " << node->second << "\n";
  inorder_walk_helper(node.right());
}

template <typename t_map> void inorder_walk(const t_map &map) {
  inorder_walk_helper(map.root());
}
} // namespace
#endif

int main() {
  std::vector<int> vec{5, 10, 20, 3, -5, 7, 28, -6, 13, -44, 0, 1};
  std::vector<std::pair<std::size_t, std::size_t>> q{{1, 3}, {4, 9}, {0, 11}, {10, 11}};
  auto ans = throttle::offline_rmq<int>(vec.begin(), vec.end(), q.begin(), q.end());

  for (const auto &v : ans) {
    std::cout << v << " ";
  }
  std::cout << "\n";
}
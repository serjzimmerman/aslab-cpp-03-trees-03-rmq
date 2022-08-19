#include <chrono>
#include <iostream>

#include "cartesian_map.hpp"
#include "cartesian_set.hpp"

#include "disjoint_map_forest.hpp"
#include "disjoint_set_forest.hpp"
#include "offline_rmq.hpp"

#if 0
template class throttle::cartesian_set<int>;
template class throttle::cartesian_map<int, bool>;
template class throttle::disjoint_map_forest<int, int>;
template class throttle::disjoint_set_forest<int>;
#endif

#include <cstdlib>

int main() {
  std::size_t n, m;
  if (!(std::cin >> n)) {
    std::cout << "Invalid input\n";
    return 1;
  }

  std::vector<int> vec{};
  vec.reserve(n);

  for (std::size_t i = 0; i < n; ++i) {
    int temp;
    if (!(std::cin >> temp)) {
      std::cout << "Invalid input\n";
      return 1;
    }
    vec.push_back(temp);
  }

  if (!(std::cin >> m)) {
    std::cout << "Invalid input\n";
    return 1;
  }

  size_t max_index = vec.size() - 1;
  std::vector<std::pair<std::size_t, std::size_t>> q_vec{};
  for (std::size_t i = 0; i < m; ++i) {
    std::size_t left, right;
    if (!(std::cin >> left >> right) || (left >= right) || (left > max_index) || (right > max_index)) {
      std::cout << "Invalid input\n";
      return 1;
    }
    q_vec.push_back({left, right});
  }

  auto ans = throttle::offline_rmq<int>(vec.begin(), vec.end(), q_vec.begin(), q_vec.end());

  for (const auto &v : ans) {
    std::cout << vec.at(v) << " ";
  }
  std::cout << "\n";
}
#include <chrono>
#include <iostream>

#include "cartesian_set.hpp"

#include "disjoint_map_forest.hpp"
#include "disjoint_set_forest.hpp"
#include "indexed_disjoint_map.hpp"
#include "offline_rmq.hpp"

#include <cstdlib>

int main() {
  unsigned n, m;
  if (!(std::cin >> n)) {
    std::cout << "Can't read number of elements \"n\"\n";
    return 1;
  }

  std::vector<int> vec{};
  vec.reserve(n);

  for (unsigned i = 0; i < n; ++i) {
    int temp;
    if (!(std::cin >> temp)) {
      std::cout << "Can't read element at index " << i << " out of " << n << "\n";
      return 1;
    }
    vec.push_back(temp);
  }

  if (!(std::cin >> m)) {
    std::cout << "Can't read number of queries \"m\"\n";
    return 1;
  }

  size_t max_index = vec.size() - 1;
  std::vector<std::pair<unsigned, unsigned>> q_vec{};
  for (unsigned i = 0; i < m; ++i) {
    unsigned left, right;
    if (!(std::cin >> left >> right)) {
      std::cout << "Can't read query at index " << i << " out of " << m << "\n";
    }
    if ((left >= right) || (left > max_index) || (right > max_index)) {
      std::cout << "[" << left << ", " << right << "] in not a valid range in array of size " << n << "\n";
      return 1;
    }
    q_vec.push_back({left, right});
  }

  auto ans = throttle::iterative_offline_rmq<int, std::less<int>>(vec.begin(), vec.end(), q_vec.begin(), q_vec.end());

  for (const auto &v : ans) {
    std::cout << vec.at(v) << " ";
  }

  std::cout << "\n";
}
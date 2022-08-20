#include <chrono>
#include <iostream>

#include "cartesian_map.hpp"
#include "cartesian_set.hpp"

#include "disjoint_map_forest.hpp"
#include "disjoint_set_forest.hpp"
#include "offline_rmq.hpp"

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

  auto recursive_start = std::chrono::high_resolution_clock::now();
  auto &&ans_rec = throttle::recursive_offline_rmq<int>(vec.begin(), vec.end(), q_vec.begin(), q_vec.end());
  auto recursive_finish = std::chrono::high_resolution_clock::now();
  auto recursive_elapsed = std::chrono::duration<double, std::milli>(recursive_finish - recursive_start);

  auto iterative_start = std::chrono::high_resolution_clock::now();
  auto &&ans_iter = throttle::iterative_offline_rmq<int>(vec.begin(), vec.end(), q_vec.begin(), q_vec.end());
  auto iterative_finish = std::chrono::high_resolution_clock::now();
  auto iterative_elapsed = std::chrono::duration<double, std::milli>(iterative_finish - iterative_start);

  std::cout << "throttle::recursive_offline_rmq took " << recursive_elapsed.count() << "ms to run\n";
  std::cout << "throttle::iterative_offline_rmq " << iterative_elapsed.count() << "ms to run\n"; 
}
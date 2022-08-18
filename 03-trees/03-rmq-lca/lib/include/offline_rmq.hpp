/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <tsimmerman.ss@phystech.edu>, wrote this file.  As long as you
 * retain this notice you can do whatever you want with this stuff. If we meet
 * some day, and you think this stuff is worth it, you can buy me a beer in
 * return.
 * ----------------------------------------------------------------------------
 */

#pragma once

#include <cstddef>
#include <functional>
#include <unordered_map>
#include <utility>
#include <vector>

#include "cartesian_map.hpp"
#include "disjoint_map_forest.hpp"

namespace throttle {
namespace detail {

using rmq_query = std::pair<std::size_t, std::size_t>;
using rmq_query_2d_vec = std::unordered_map<std::size_t, std::vector<std::pair<std::size_t, std::size_t>>>;

template <typename t_map, typename t_dsu> struct recursive_offline_rmq {
  t_map &m_map;
  t_dsu &m_dsu;
  const rmq_query_2d_vec &m_queries;
  std::vector<std::size_t> &m_ans;

  void fill_ans(typename t_map::node_proxy p_node) {
    p_node->second = true; // Visited
    const auto curr_index = p_node.index();
    *(m_dsu.find_set(curr_index)) = curr_index;

    typename t_map::node_proxy left = p_node.left(), right = p_node.right();

    if (left) {
      fill_ans(left);
      m_dsu.union_set(curr_index, left.index());
      *(m_dsu.find_set(curr_index)) = curr_index;
    }

    if (right) {
      fill_ans(right);
      m_dsu.union_set(curr_index, right.index());
      *(m_dsu.find_set(curr_index)) = curr_index;
    }

    auto found = m_queries.find(curr_index);
    if ((found != m_queries.end())) {
      for (const auto &v : found->second) {
        typename t_map::node_proxy other = m_map.at(v.first);
        if (other->second) {
          m_ans[v.second] = *m_dsu.find_set(other.index());
        }
      }
    }
  }
};

} // namespace detail

template <typename T, typename t_data_inp_iter, typename t_query_inp_iter, typename t_comp = std::less<T>>
std::vector<std::size_t> offline_rmq(t_data_inp_iter p_start_dat, t_data_inp_iter p_finish_dat,
                                     t_query_inp_iter p_start_q, t_query_inp_iter p_finish_q) {
  cartesian_map<T, bool> map{}; // T -> visited map
  using map_size_type = typename ::throttle::cartesian_map<T, bool>::size_type;
  disjoint_map_forest<map_size_type, map_size_type> dsu{}; // Index -> Ancestor DSU
  detail::rmq_query_2d_vec queries{};

  std::size_t i = 0;
  for (; p_start_dat != p_finish_dat; ++p_start_dat) {
    map.append({*p_start_dat, false});
    dsu.make_set(i++, 0);
  }

  i = 0;
  for (; p_start_q != p_finish_q; ++p_start_q, ++i) {
    queries[p_start_q->first].push_back({p_start_q->second, i});
    queries[p_start_q->second].push_back({p_start_q->first, i});
  }

  std::vector<std::size_t> ans_vec = {};
  ans_vec.resize(i);

  using solver_type =
      typename detail::recursive_offline_rmq<cartesian_map<T, bool>, disjoint_map_forest<map_size_type, map_size_type>>;
  solver_type solver{map, dsu, queries, ans_vec};
  solver.fill_ans(map.root());

  return solver.m_ans;
}

} // namespace throttle
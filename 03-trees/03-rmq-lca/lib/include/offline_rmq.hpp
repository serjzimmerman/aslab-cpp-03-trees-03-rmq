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
#include <list>

#include "cartesian_map.hpp"
#include "disjoint_map_forest.hpp"

namespace throttle {
namespace detail {

using rmq_query = std::pair<std::size_t, std::size_t>;
using rmq_query_2d_vec = std::unordered_map<std::size_t, std::vector<std::pair<std::size_t, std::size_t>>>;

template <typename T> class recursive_offline_rmq_solver {
  using map_type = cartesian_map<T, bool>;
  using map_size_type = typename map_type::size_type;
  using dsu_type = disjoint_map_forest<map_size_type, map_size_type>;

  map_type m_map;
  dsu_type m_dsu;
  rmq_query_2d_vec m_queries;
  std::vector<std::size_t> m_ans;

public:
  template <typename t_data_inp_iter, typename t_query_inp_iter, typename t_comp = std::less<T>>
  recursive_offline_rmq_solver(t_data_inp_iter p_start_dat, t_data_inp_iter p_finish_dat, t_query_inp_iter p_start_q,
                        t_query_inp_iter p_finish_q)
      : m_map{}, m_dsu{}, m_queries{}, m_ans{} {
    std::size_t i = 0;
    for (; p_start_dat != p_finish_dat; ++p_start_dat) {
      m_map.append({*p_start_dat, false});
      m_dsu.make_set(i++, 0);
    }

    i = 0;
    for (; p_start_q != p_finish_q; ++p_start_q, ++i) {
      m_queries[p_start_q->first].push_back({p_start_q->second, i});
      m_queries[p_start_q->second].push_back({p_start_q->first, i});
    }

    m_ans.resize(i);
  }

private:
  void fill_ans_helper(typename map_type::node_proxy p_node) {
    p_node->second = true; // Visited
    const auto curr_index = p_node.index();
    *(m_dsu.find_set(curr_index)) = curr_index;

    typename map_type::node_proxy left = p_node.left(), right = p_node.right();

    if (left) {
      fill_ans_helper(left);
      m_dsu.union_set(curr_index, left.index());
      *(m_dsu.find_set(curr_index)) = curr_index;
    }

    if (right) {
      fill_ans_helper(right);
      m_dsu.union_set(curr_index, right.index());
      *(m_dsu.find_set(curr_index)) = curr_index;
    }

    auto found = m_queries.find(curr_index);
    if ((found != m_queries.end())) {
      for (auto its = (found->second).begin(), ite = (found->second).end(); its != ite; ++its) {
        typename map_type::node_proxy other = m_map.at(its->first);
        if (other->second) {
          m_ans[its->second] = *m_dsu.find_set(other.index());
        }
      }
    }
  }

public:
  void fill_ans() {
    fill_ans_helper(m_map.root());
  }

  std::vector<std::size_t> get_ans() && {
    return std::move(m_ans);
  }
};

} // namespace detail

template <typename T, typename t_data_inp_iter, typename t_query_inp_iter, typename t_comp = std::less<T>>
std::vector<std::size_t> offline_rmq(t_data_inp_iter p_start_dat, t_data_inp_iter p_finish_dat,
                                     t_query_inp_iter p_start_q, t_query_inp_iter p_finish_q) {
  detail::recursive_offline_rmq_solver<T> solver{p_start_dat, p_finish_dat, p_start_q, p_finish_q};
  solver.fill_ans();
  return std::move(solver).get_ans();
}

} // namespace throttle
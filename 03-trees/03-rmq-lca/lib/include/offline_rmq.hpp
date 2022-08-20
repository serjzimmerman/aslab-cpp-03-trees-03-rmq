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
#include <list>
#include <unordered_map>
#include <utility>
#include <vector>

#include "cartesian_map.hpp"
#include "disjoint_map_forest.hpp"

namespace throttle {
namespace detail {

using rmq_query = std::pair<std::size_t, std::size_t>;
using rmq_query_2d_vec = std::unordered_map<std::size_t, std::vector<std::pair<std::size_t, std::size_t>>>;

template <typename T> class offline_rmq_solver_base {
protected:
  using map_type = cartesian_map<T, bool>;
  using map_size_type = typename map_type::size_type;
  using dsu_type = disjoint_map_forest<map_size_type, map_size_type>;

  map_type m_map;
  dsu_type m_dsu;
  rmq_query_2d_vec m_queries;
  std::vector<std::size_t> m_ans;

  template <typename t_data_inp_iter, typename t_query_inp_iter, typename t_comp = std::less<T>>
  offline_rmq_solver_base(t_data_inp_iter p_start_dat, t_data_inp_iter p_finish_dat, t_query_inp_iter p_start_q,
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

  void write_ans_after_subtree_complete(typename map_type::size_type p_curr_index) {
    auto found = m_queries.find(p_curr_index);
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
  std::vector<std::size_t> get_ans() && {
    return std::move(m_ans);
  }
};

template <typename T> class recursive_offline_rmq_solver : public offline_rmq_solver_base<T> {
  using base = offline_rmq_solver_base<T>;

  using typename base::dsu_type;
  using typename base::map_size_type;
  using typename base::map_type;

public:
  template <typename t_data_inp_iter, typename t_query_inp_iter, typename t_comp = std::less<T>>
  recursive_offline_rmq_solver(t_data_inp_iter p_start_dat, t_data_inp_iter p_finish_dat, t_query_inp_iter p_start_q,
                               t_query_inp_iter p_finish_q)
      : offline_rmq_solver_base<T>{p_start_dat, p_finish_dat, p_start_q, p_finish_q} {}

private:
  void fill_ans_helper(typename map_type::node_proxy p_node) {
    p_node->second = true; // Visited
    const auto curr_index = p_node.index();
    *(this->m_dsu.find_set(curr_index)) = curr_index;

    typename map_type::node_proxy left = p_node.left(), right = p_node.right();

    if (left) {
      fill_ans_helper(left);
      this->m_dsu.union_set(curr_index, left.index());
      *(this->m_dsu.find_set(curr_index)) = curr_index;
    }

    if (right) {
      fill_ans_helper(right);
      this->m_dsu.union_set(curr_index, right.index());
      *(this->m_dsu.find_set(curr_index)) = curr_index;
    }

    this->write_ans_after_subtree_complete(curr_index);
  }

public:
  void fill_ans() {
    fill_ans_helper(this->m_map.root());
  }
};

template <typename T> class iterative_offline_rmq_solver : public offline_rmq_solver_base<T> {
  using base = offline_rmq_solver_base<T>;

  using typename base::dsu_type;
  using typename base::map_size_type;
  using typename base::map_type;

public:
  template <typename t_data_inp_iter, typename t_query_inp_iter, typename t_comp = std::less<T>>
  iterative_offline_rmq_solver(t_data_inp_iter p_start_dat, t_data_inp_iter p_finish_dat, t_query_inp_iter p_start_q,
                               t_query_inp_iter p_finish_q)
      : offline_rmq_solver_base<T>{p_start_dat, p_finish_dat, p_start_q, p_finish_q} {}

public:
  void fill_ans() {
    typename map_type::node_proxy curr = this->m_map.root();

    while (curr) {
      typename map_type::node_proxy left = curr.left(), right = curr.right();
      const auto curr_index = curr.index();
      const bool descending = !curr->second;

      // Here we came from curr.parent() and neither left or right child have been visited.
      if (descending) {
        curr->second = true;
        *(this->m_dsu.find_set(curr_index)) = curr_index;

        if (left) {
          curr = left;
        } else if (right) {
          curr = right;
        } else {
          curr = curr.parent();
          this->write_ans_after_subtree_complete(curr_index);
        }
        continue;
      }

      // If there's a left node and we are ascending to the root then there are 2 cases.
      if (left) {
        // 1. There isn't a right child or it hasn't been visited. Then we've come from curr.left().
        if (!right || !right->second) {
          this->m_dsu.union_set(curr_index, left.index());
          *(this->m_dsu.find_set(curr_index)) = curr_index;
          if (!right) this->write_ans_after_subtree_complete(curr_index);
          curr = (right ? right : curr.parent());
          continue;
        }
        // 2. There is a right child and it has been visited. Then we just came from it and need to continue traversing
        // to the root.
        this->m_dsu.union_set(curr_index, right.index());
        *(this->m_dsu.find_set(curr_index)) = curr_index;
        this->write_ans_after_subtree_complete(curr_index);
        curr = curr.parent();
      }

      // There is no left child but the right one has already been traversed, because otherwise this block of code
      // wouldn't be reached.
      else {
        this->m_dsu.union_set(curr_index, right.index());
        *(this->m_dsu.find_set(curr_index)) = curr_index;
        this->write_ans_after_subtree_complete(curr_index);
        curr = curr.parent();
      }
    }
  }
};

} // namespace detail

template <typename T, typename t_data_inp_iter, typename t_query_inp_iter, typename t_comp = std::less<T>>
std::vector<std::size_t> recursive_offline_rmq(t_data_inp_iter p_start_dat, t_data_inp_iter p_finish_dat,
                                               t_query_inp_iter p_start_q, t_query_inp_iter p_finish_q) {
  detail::recursive_offline_rmq_solver<T> solver{p_start_dat, p_finish_dat, p_start_q, p_finish_q};
  solver.fill_ans();
  return std::move(solver).get_ans();
}

template <typename T, typename t_data_inp_iter, typename t_query_inp_iter, typename t_comp = std::less<T>>
std::vector<std::size_t> iterative_offline_rmq(t_data_inp_iter p_start_dat, t_data_inp_iter p_finish_dat,
                                               t_query_inp_iter p_start_q, t_query_inp_iter p_finish_q) {
  detail::iterative_offline_rmq_solver<T> solver{p_start_dat, p_finish_dat, p_start_q, p_finish_q};
  solver.fill_ans();
  return std::move(solver).get_ans();
}

} // namespace throttle
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
#include <utility>

#include "cartesian_tree.hpp"

namespace throttle {

template <typename T, typename t_comp = std::less<T>> class cartesian_set final : public cartesian_tree<T, t_comp> {
private:
  using base = cartesian_tree<T, t_comp>;
  using typename base::node_type;

public:
  using key_type = typename base::value_type;
  using typename base::comp;
  using typename base::size_type;
  using typename base::value_type;

  cartesian_set() : base{} {}

  template <typename t_inp_iter> cartesian_set(t_inp_iter start, t_inp_iter finish) : cartesian_set{} {
    for (; start != finish; ++start) {
      this->append(*start);
    }
  }

public:
  class node_proxy {
  private:
    friend class cartesian_set;
    using size_type = cartesian_set::size_type;

    const cartesian_set &m_tree;
    size_type m_curr_index;
    const node_type &m_curr_node;

    using value_type = key_type;
    using reference = const key_type &;
    using pointer = const key_type *;

    node_proxy(const cartesian_set &p_tree, size_type p_index = 0)
        : m_tree{p_tree}, m_curr_index{p_index}, m_curr_node{m_tree.at_index(m_curr_index)} {}

  public:
    size_type index() const {
      return m_curr_index - 1;
    }

    node_proxy left() const {
      return node_proxy{m_tree, m_curr_node.m_left};
    }

    node_proxy right() const {
      return node_proxy{m_tree, m_curr_node.m_right};
    }

    node_proxy parent() const {
      return node_proxy{m_tree, m_curr_node.m_parent};
    }

    bool valid() const {
      return m_curr_index;
    }

    operator bool() const {
      return valid();
    }

    reference operator*() const {
      return m_tree.value(m_curr_index);
    }

    pointer operator->() const {
      return &m_tree.value(m_curr_index);
    }

    bool operator==(const node_proxy &p_rhs) const {
      return m_curr_index == p_rhs.m_curr_index;
    }

    bool operator!=(const node_proxy &p_rhs) const {
      return !(*this == p_rhs);
    }
  };

  node_proxy at(size_type p_index) const {
    return node_proxy{*this, p_index + 1};
  }

  node_proxy root() const {
    return node_proxy(*this, this->m_root);
  }
};

} // namespace throttle
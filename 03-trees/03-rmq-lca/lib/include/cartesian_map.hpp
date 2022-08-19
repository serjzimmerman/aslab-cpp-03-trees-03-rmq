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

namespace detail {
template <typename t_key, typename t_mapped> using cartesian_map_value_type = std::pair<const t_key, t_mapped>;

template <typename t_key, typename t_mapped, typename t_comp> struct cartesian_map_comp {
  using value_type = cartesian_map_value_type<t_key, t_mapped>;
  bool operator()(const value_type &p_lhs, const value_type &p_rhs) {
    return t_comp{}(p_lhs.first, p_rhs.first);
  }
};
}; // namespace detail

template <typename t_key, typename t_mapped, typename t_comp = std::less<t_key>>
class cartesian_map final : public cartesian_tree<detail::cartesian_map_value_type<t_key, t_mapped>,
                                                  detail::cartesian_map_comp<t_key, t_mapped, t_comp>> {
private:
  using base = cartesian_tree<detail::cartesian_map_value_type<t_key, t_mapped>,
                              detail::cartesian_map_comp<t_key, t_mapped, t_comp>>;
  using typename base::node_type;

public:
  using key_type = t_key;
  using mapped_type = t_mapped;
  using typename base::comp;
  using typename base::size_type;
  using typename base::value_type;

  cartesian_map() : base{} {}

  template <typename t_inp_iter> cartesian_map(t_inp_iter start, t_inp_iter finish) : cartesian_map{} {
    for (; start != finish; ++start) {
      this->append(*start);
    }
  }

public:
  class node_proxy {
    friend class cartesian_map;
    using size_type = cartesian_map::size_type;

    cartesian_map *m_tree;
    size_type m_curr_index;
    const node_type *m_curr_node;

    using value_type = std::pair<const key_type, mapped_type>;
    using reference = std::pair<const key_type &, mapped_type &>;

    struct pointer {
      reference m_ref;
      reference *operator->() {
        return &m_ref;
      }
    };

    node_proxy(cartesian_map *p_tree, size_type p_index = 0)
        : m_tree{p_tree}, m_curr_index{p_index}, m_curr_node{&m_tree->at_index(m_curr_index)} {}

  public:
    size_type index() const {
      return m_curr_index - 1;
    }

    node_proxy left() const {
      return node_proxy{m_tree, m_curr_node->m_left};
    }

    node_proxy right() const {
      return node_proxy{m_tree, m_curr_node->m_right};
    }

    node_proxy parent() const {
      return node_proxy{m_tree, m_curr_node->m_parent};
    }

    bool valid() const {
      return m_curr_index;
    }

    operator bool() const {
      return valid();
    }

    reference operator*() const {
      auto &val = m_tree->value(m_curr_index);
      return {val.first, val.second};
    }

    pointer operator->() {
      auto &val = m_tree->value(m_curr_index);
      return {{val.first, val.second}};
    }

    bool operator==(const node_proxy &p_rhs) const {
      return m_curr_index == p_rhs.m_curr_index;
    }

    bool operator!=(const node_proxy &p_rhs) const {
      return !(*this == p_rhs);
    }
  };

  class const_node_proxy {
    friend class cartesian_map;
    using size_type = cartesian_map::size_type;

    const cartesian_map *m_tree;
    size_type m_curr_index;
    const node_type *m_curr_node;

    using value_type = std::pair<const key_type, const mapped_type>;
    using reference = std::pair<const key_type &, const mapped_type &>;

    struct pointer {
      reference m_ref;
      reference *operator->() {
        return &m_ref;
      }
    };

    const_node_proxy(const cartesian_map *p_tree, size_type p_index = 0)
        : m_tree{p_tree}, m_curr_index{p_index}, m_curr_node{&m_tree->at_index(m_curr_index)} {}

  public:
    size_type index() const {
      return m_curr_index - 1;
    }

    const_node_proxy left() const {
      return {m_tree, m_curr_node->m_left};
    }

    const_node_proxy right() const {
      return {m_tree, m_curr_node->m_right};
    }

    const_node_proxy parent() const {
      return {m_tree, m_curr_node->m_parent};
    }

    bool valid() const {
      return m_curr_index;
    }

    operator bool() const {
      return valid();
    }

    reference operator*() const {
      auto &val = m_tree->value(m_curr_index);
      return {val.first, val.second};
    }

    pointer operator->() {
      auto &val = m_tree->value(m_curr_index);
      return {{val.first, val.second}};
    }

    bool operator==(const node_proxy &p_rhs) const {
      return m_curr_index == p_rhs.m_curr_index;
    }

    bool operator!=(const node_proxy &p_rhs) const {
      return !(*this == p_rhs);
    }
  };

  node_proxy at(size_type p_index) {
    return {this, p_index + 1};
  }

  const_node_proxy at(size_type p_index) const {
    return {this, p_index + 1};
  }

  node_proxy root() {
    return {this, this->m_root};
  }

  const_node_proxy root() const {
    return {this, this->m_root};
  }
};

} // namespace throttle
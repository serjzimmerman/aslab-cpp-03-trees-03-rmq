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

namespace throttle {

namespace detail {
struct cartesian_tree_node {
  using size_type = unsigned;

  size_type m_parent;
  size_type m_left;
  size_type m_right;

  cartesian_tree_node() : m_parent{0}, m_left{0}, m_right{0} {}
};
} // namespace detail

template <typename t_value_type, typename t_comp = std::less<t_value_type>> class cartesian_tree {
protected:
  using node_type = typename detail::cartesian_tree_node;

public:
  using size_type = node_type::size_type;
  using value_type = t_value_type;
  using comp = t_comp;

protected:
  std::vector<node_type> m_tree_vec;
  std::vector<t_value_type> m_key_vec;
  size_type m_rightmost;
  size_type m_root;

  node_type &at_index(size_type p_index) {
    return m_tree_vec.at(p_index);
  }

  const node_type &at_index(size_type p_index) const {
    return m_tree_vec.at(p_index);
  }

  t_value_type &value(size_type p_index) {
    return m_key_vec.at(p_index - 1);
  }

  const t_value_type &value(size_type p_index) const {
    return m_key_vec.at(p_index - 1);
  }

public:
  cartesian_tree() : m_tree_vec{}, m_key_vec{}, m_rightmost{0}, m_root{0} {
    m_tree_vec.emplace_back(); // Sentinel value that is used to indicate that there is no parent, left or right node.
                               // It's otherwise unreacheble and contains garbage;
  }

  bool empty() const noexcept {
    return !size();
  }

  size_type size() const noexcept {
    return m_key_vec.size();
  }

private:
  void insert(const t_value_type &p_key) {
    m_tree_vec.emplace_back();
    try {
      m_key_vec.emplace_back(p_key);
    } catch (...) {
      m_tree_vec.pop_back();
      throw;
    }
  }

public:
  void append(const t_value_type &p_key) {
    bool is_empty = empty();
    insert(p_key);

    size_type new_index = m_key_vec.size();
    if (is_empty) {
      m_root = m_rightmost = new_index;
      return;
    }

    size_type curr = m_rightmost;
    bool is_key_less = t_comp{}(p_key, value(curr));
    while (curr && is_key_less) {
      curr = at_index(curr).m_parent;
      if (curr) is_key_less = t_comp{}(p_key, value(curr));
    }

    m_rightmost = new_index;
    node_type &rightmost_node = m_tree_vec.back();
    if (!curr) {
      rightmost_node.m_left = m_root;
      at_index(m_root).m_parent = new_index;
      m_root = m_rightmost;
      return;
    }

    node_type &curr_node = at_index(curr);
    rightmost_node.m_left = curr_node.m_right;
    at_index(curr_node.m_right).m_parent = m_rightmost;
    curr_node.m_right = m_rightmost;
    rightmost_node.m_parent = curr;
  }
#if 0
  template <typename t_stream> void dump(t_stream &p_ostream) const {
    p_ostream << "digraph {\n";
    for (size_type i = 1; i < m_tree_vec.size(); ++i) {
      p_ostream << "\tnode_" << i << " [label = \"" << value(i) << "\"];\n";
      const node_type &node = at_index(i);

      if (node.m_left) {
        p_ostream << "\tnode_" << i << " -> node_" << node.m_left << ";\n";
      } else {
        p_ostream << "\tnode_" << i << " -> node_0_" << i << ";\n";
        p_ostream << "\tnode_0_" << i << " [label = \"NIL\"];\n";
      }

      if (node.m_right) {
        p_ostream << "\tnode_" << i << " -> node_" << node.m_right << ";\n";
      } else {
        p_ostream << "\tnode_" << i << " -> node_0__" << i << ";\n";
        p_ostream << "\tnode_0__" << i << " [label = \"NIL\"];\n";
      }
    }
    p_ostream << "}\n";
  }
#endif
};

#if 0
template <typename t_stream, typename t_value_type, typename t_comp>
t_stream &operator<<(t_stream &p_ostream, cartesian_tree<t_value_type, t_comp> &p_tree) {
  p_tree.dump(p_ostream);
  return p_ostream;
}
#endif

} // namespace throttle
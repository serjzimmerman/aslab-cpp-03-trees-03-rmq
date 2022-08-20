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
template <typename t_value_type> struct disjoint_map_forest_node {
  using size_type = unsigned;

  t_value_type m_val;
  size_type m_parent_index;
  size_type m_rank;

  disjoint_map_forest_node(size_type p_parent_index, t_value_type p_val)
      : m_val{p_val}, m_parent_index{p_parent_index}, m_rank{0} {}
};
} // namespace detail

template <typename t_key_type, typename t_value_type, typename t_eq = std::equal_to<t_key_type>,
          typename t_hash = std::hash<t_key_type>>
class disjoint_map_forest final {
  using node_type = typename detail::disjoint_map_forest_node<t_value_type>;

public:
  using value_type = t_value_type;
  using size_type = typename node_type::size_type;
  using key_type = t_key_type;

private:
  std::unordered_map<key_type, size_type, t_hash, t_eq> m_key_index_map;
  std::vector<node_type> m_node_vec;

public:
  disjoint_map_forest() : m_key_index_map{}, m_node_vec{} {}

  class individual_set_proxy {
    friend class disjoint_map_forest;

    using pointer = value_type *;
    using reference = value_type &;

    size_type m_curr_index;
    disjoint_map_forest *m_map;

  public:
    individual_set_proxy(size_type m_node, disjoint_map_forest *p_map) : m_curr_index{m_node}, m_map{p_map} {}
    reference operator*() {
      return m_map->at_index(m_curr_index).m_val;
    }

    pointer operator->() {
      return &(m_map->at_index(m_curr_index).m_val);
    }
  };

  void make_set(const key_type &p_key, const value_type &p_val) {
    size_type index = m_node_vec.size();
    bool inserted = m_key_index_map.emplace(std::make_pair(p_key, index)).second;
    if (inserted) m_node_vec.emplace_back(index, p_val);
  }

  void make_set(const key_type &p_key, value_type &&p_val) {
    size_type index = m_node_vec.size();
    bool inserted = m_key_index_map.emplace(std::make_pair(p_key, index)).second;
    if (inserted) m_node_vec.emplace_back(index, std::move(p_val));
  }

private:
  node_type &at_index(size_type p_index) {
    return m_node_vec.at(p_index);
  }

  size_type &parent_index(size_type p_node) {
    return at_index(p_node).m_parent_index;
  }

  size_type find_set_impl(size_type p_node) {
    size_type &parent = parent_index(p_node);
    if (parent != p_node) parent = find_set_impl(parent);
    return parent;
  }

  void link(size_type p_left, size_type p_right) {
    node_type &left = at_index(p_left), right = at_index(p_right);
    if (left.m_rank > right.m_rank)
      right.m_parent_index = p_left;

    else {
      left.m_parent_index = p_right;
      if (left.m_rank == right.m_rank) ++right.m_rank;
    }
  }

public:
  individual_set_proxy find_set(const key_type &p_key) {
    return individual_set_proxy{find_set_impl(m_key_index_map.at(p_key)), this};
  }

  void union_set(const key_type &p_left, const key_type &p_right) {
    size_type left = find_set_impl(m_key_index_map.at(p_left)), right = find_set_impl(m_key_index_map.at(p_right));
    if (p_left != p_right) link(left, right);
  }

  template <typename t_stream> void dump(t_stream &p_ostream) {
    p_ostream << "digraph {\n";
    for (const auto &v : m_key_index_map) {
      p_ostream << "\tnode_" << v.second << " [label = \"" << v.first << ":" << at_index(v.second).m_val << "\"];\n";
      p_ostream << "\tnode_" << v.second << " -> node_" << at_index(v.second).m_parent_index << ";\n";
    }
    p_ostream << "}\n";
  }
};

template <typename t_stream, typename t_value_type, typename t_key_type, typename t_eq, typename t_hash>
t_stream &operator<<(t_stream &p_ostream, disjoint_map_forest<t_key_type, t_value_type, t_eq, t_hash> &p_set) {
  p_set.dump(p_ostream);
  return p_ostream;
}

} // namespace throttle
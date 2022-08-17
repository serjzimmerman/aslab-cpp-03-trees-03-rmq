#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <functional>
#include <gtest/gtest.h>
#include <iterator>
#include <numeric>
#include <set>
#include <string>

#define private public
#define protected public
#include "cartesian_tree.hpp"
#undef private
#undef protected

using cartesian_tree_int = throttle::cartesian_tree<int>;

namespace {
bool validate_min_heap_helper(const cartesian_tree_int &p_tree, cartesian_tree_int::size_type p_node_index) {
  bool valid = true;
  auto node = p_tree.at_index(p_node_index);
  auto key = p_tree.key(p_node_index);
  if (node.m_left) {
    valid = valid && (key <= p_tree.key(node.m_left)) && validate_min_heap_helper(p_tree, node.m_left);
  }
  if (node.m_right) {
    valid = valid && (key <= p_tree.key(node.m_right)) && validate_min_heap_helper(p_tree, node.m_right);
  }
  return valid;
}

bool validate_min_heap_cartesian_tree(const cartesian_tree_int &p_tree) {
  return validate_min_heap_helper(p_tree, p_tree.m_root);
}

cartesian_tree_int::size_type leftmost(const cartesian_tree_int &p_tree, cartesian_tree_int::size_type p_node_index) {
  using size_type = cartesian_tree_int::size_type;
  size_type curr = p_node_index;
  while (p_tree.at_index(curr).m_left)
    curr = p_tree.at_index(curr).m_left;
  return curr;
}

cartesian_tree_int::size_type successor(const cartesian_tree_int &p_tree, cartesian_tree_int::size_type p_node_index) {
  using size_type = cartesian_tree_int::size_type;
  size_type curr = p_node_index;
  if (p_tree.at_index(curr).m_right) return leftmost(p_tree, p_tree.at_index(curr).m_right);
  return 0;
}

cartesian_tree_int::size_type inorder_successor(const cartesian_tree_int &p_tree,
                                                cartesian_tree_int::size_type p_node_index) {
  using size_type = cartesian_tree_int::size_type;

  size_type curr = p_node_index;
  if (p_tree.at_index(curr).m_right) {
    curr = successor(p_tree, curr);
  }

  else {
    size_type parent = p_tree.at_index(curr).m_parent;
    while (parent && (curr == p_tree.at_index(p_tree.at_index(curr).m_parent).m_right)) {
      curr = parent;
      parent = p_tree.at_index(parent).m_parent;
    }
    curr = parent;
  }

  return curr;
}

template <typename t_inp_iter>
bool validate_inorder_walk(const cartesian_tree_int &p_tree, t_inp_iter start, t_inp_iter finish) {
  using size_type = cartesian_tree_int::size_type;

  size_type curr = leftmost(p_tree, p_tree.m_root);
  while (curr && start != finish) {
    if (p_tree.key(curr) != *(start++)) {
      return false;
    };
    curr = inorder_successor(p_tree, curr);
  }

  if (curr != 0 || start != finish) {
    return false;
  }

  return true;
}

} // namespace

TEST(cartesian_tree, test_heap_property_0) {
  cartesian_tree_int tree;
  std::vector<int> test_case{4, 1, 2, 4, 4, 5, 2, 8, 1};

  for (const auto &v : test_case) {
    tree.append(v);
  }

  EXPECT_TRUE(validate_min_heap_cartesian_tree(tree));
  EXPECT_TRUE(validate_inorder_walk(tree, test_case.begin(), test_case.end()));
}

TEST(cartesian_tree, test_heap_property_1) {
  cartesian_tree_int tree;
  std::vector<int> test_case;
  std::srand(unsigned(std::time(nullptr)));
  std::generate_n(std::back_inserter(test_case), 1 << 18, std::rand);

  for (const auto &v : test_case) {
    tree.append(v);
  }

  EXPECT_TRUE(validate_min_heap_cartesian_tree(tree));
  EXPECT_TRUE(validate_inorder_walk(tree, test_case.begin(), test_case.end()));
}

TEST(cartesian_tree, test_large_insert_0) {
  cartesian_tree_int tree;
  std::vector<int> test_case;

  std::srand(unsigned(std::time(nullptr)));
  std::generate_n(std::back_inserter(test_case), 1 << 22, []() { return std::rand() & ((1 << 18) - 1); });

  for (const auto &v : test_case) {
    tree.append(v);
  }

  EXPECT_TRUE(validate_min_heap_cartesian_tree(tree));
  EXPECT_TRUE(validate_inorder_walk(tree, test_case.begin(), test_case.end()));
}
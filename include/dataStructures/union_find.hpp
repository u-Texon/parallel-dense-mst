#pragma once

#include <vector>
#include <unordered_map>

#include "../definitions.hpp"


class UnionFind {
public:
  UnionFind(std::size_t n) : data(n, -1) {}
  void unify(VId a, VId b) {
    VId root_a = find(a);
    VId root_b = find(b);
    if (root_a == root_b)
      return;
    if (data[root_b] < data[root_a])
      std::swap(root_b, root_a); // sizes are encoded as negative integers
    data[root_a] += data[root_b];
    data[root_b] = root_a;
  }
  VId find(VId a) {
    if (data[a] < 0)
      return a;
    VId parent = static_cast<VId>(data[a]);
    data[a] = find(parent);
    return data[a];
  }
  void clear(std::size_t n) {
      data.resize(n);
      for (long & i : data) {
          i = -1;
      }
  }

    void clear() {
        for (long & i : data) {
            i = -1;
        }
    }

private:
  std::vector<std::int64_t> data;
};

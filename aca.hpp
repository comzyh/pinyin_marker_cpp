#ifndef ACA_HPP
#define ACA_HPP
#include <unordered_map>
#include <utility>
#include <vector>
#include <queue>

template <typename CharT, typename DataT>
struct TrieNode {
  std::unordered_map<CharT, size_t> nexts;
  size_t fail;
  std::basic_string<CharT> str;
  DataT data;
};

template <typename CharT, typename DataT>
struct Automation {
  const static size_t root_node = 0;
  std::vector<TrieNode<CharT, DataT> > nodes;
  Automation () {
    nodes.resize(root_node + 1);
  }
  void insert(const std::basic_string<CharT> &str, const std::function<void(DataT &data)> &callback) {
    size_t current = root_node;
    for (auto chr = str.begin(); chr != str.end(); chr ++) {
      auto next = nodes[current].nexts.find(*chr);
      if (next == nodes[current].nexts.end()) {
        nodes.push_back(TrieNode<CharT, DataT>());
        nodes[current].nexts.insert(std::make_pair(*chr, nodes.size() - 1));
        current = nodes.size() - 1;
      } else {
        current = next->second;
      }
    }
    nodes[current].str = str;
    callback(nodes[current].data); // callback, to allow user to modify data
  }
  void build() {
    std::queue<int> q;
    nodes[root_node].fail = root_node;
    q.push(root_node);
    while (!q.empty()) {
      TrieNode<CharT, DataT> &h = nodes[q.front()];
      for (auto it=h.nexts.begin(); it != h.nexts.end(); it++ ) {
        size_t f = h.fail;
        const char32_t chr = it->first;
        while(nodes[f].nexts.find(chr) == nodes[f].nexts.end() && f != root_node) {
          f = nodes[f].fail;
        }
        if (nodes[f].nexts.find(chr) != nodes[f].nexts.end() && q.front() != root_node) {
          nodes[h.nexts[chr]].fail = nodes[f].nexts.find(chr)->second;
        } else {
          nodes[h.nexts[chr]].fail = root_node;
        }
        q.push(h.nexts[chr]);
      }
      q.pop();
    }
  }
  void find(const std::basic_string<CharT> &str, const std::function<void(size_t index, const DataT &data)> &callback) const {
    size_t node = root_node;
    for (size_t i = 0; i < str.length(); i++) {
      char32_t chr = str[i];
      while(nodes[node].nexts.find(chr) == nodes[node].nexts.end() && node != root_node) {
        node = nodes[node].fail;
      }
      if (nodes[node].nexts.find(chr) != nodes[node].nexts.end()) {
        node = nodes[node].nexts.find(chr)->second;
      } else {
        node = root_node;
      }
      for (size_t f = node; f != root_node; f = nodes[f].fail) {
        callback(i, nodes[f].data);
      }
    }
  }
};
#endif

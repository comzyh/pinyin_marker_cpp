#ifndef ACA_HPP
#define ACA_HPP
#include <unordered_map>
#include <utility>
#include <vector>
#include <queue>

template <typename CharT, typename DataT>
struct TrieNode {
  std::unordered_map<CharT, TrieNode<CharT, DataT> *> nexts;
  TrieNode<CharT, DataT> * fail;
  std::basic_string<CharT> str;
  DataT data;
  TrieNode () {
    fail = nullptr;
  }
};

template <typename CharT, typename DataT>
struct Automation {
  typedef TrieNode<CharT, DataT> NodeType;
  NodeType * root_node = nullptr;
  Automation () {
    root_node = new NodeType();
  }
  ~Automation () {
    destory(root_node);
  }
  void destory(NodeType *node){ // free all node recursively
    for (auto it=node->nexts.begin(); it != node->nexts.end(); it++ ) {
      destory(it->second);
    }
    delete node;
  }
  void insert(const std::basic_string<CharT> &str, const std::function<void(DataT &data)> &update_cb) {
    NodeType * current = root_node;
    for (auto chr = str.begin(); chr != str.end(); chr ++) {
      auto next = current->nexts.find(*chr);
      if (next == current->nexts.end()) {
        NodeType *new_node = new NodeType();
        current->nexts.insert(std::make_pair(*chr, new_node));
        current = new_node;
      } else {
        current = next->second;
      }
    }
    current->str = str;
    update_cb(current->data); // callback, to allow user to update data
  }
  void build() {
    std::queue<NodeType *> q;
    root_node->fail = root_node;
    q.push(root_node);
    while (!q.empty()) {
      NodeType *h = q.front();
      for (auto it=h->nexts.begin(); it != h->nexts.end(); it++ ) {
        NodeType * f = h->fail;
        const char32_t chr = it->first;
        while(f->nexts.find(chr) == f->nexts.end() && f != root_node) {
          f = f->fail;
        }
        if (f->nexts.find(chr) != f->nexts.end() && q.front() != root_node) {
          h->nexts[chr]->fail = f->nexts.find(chr)->second;
        } else {
          h->nexts[chr]->fail = root_node;
        }
        q.push(h->nexts[chr]);
      }
      q.pop();
    }
  }
  void find(const std::basic_string<CharT> &str, const std::function<void(size_t index, const DataT &data)> &callback) const {
    NodeType * node = root_node;
    for (size_t i = 0; i < str.length(); i++) {
      char32_t chr = str[i];
      while(node->nexts.find(chr) == node->nexts.end() && node != root_node) {
        node = node->fail;
      }
      if (node->nexts.find(chr) != node->nexts.end()) {
        node = node->nexts.find(chr)->second;
      } else {
        node = root_node;
      }
      for (NodeType *f = node; f != root_node; f = f->fail) {
        callback(i, f->data);
      }
    }
  }
};
#endif

#ifndef PINYIN_MARKER_HPP
#define PINYIN_MARKER_HPP

#include <codecvt>
#include <codecvt>
#include <cstdio>
#include <iostream>
#include <locale>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include <queue>
namespace pinyin_marker {
std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;

struct PinyinUnit {
    std::vector<std::string> pinyin;
    int freq;
    PinyinUnit() {}
    PinyinUnit(const std::u32string &pinyin_block, int freq) : freq(freq) {
        size_t begin = 0;
        for (size_t i = pinyin_block.find(' '); i != std::u32string::npos; i = pinyin_block.find(' ', i + 1)) {
            pinyin.push_back( converter.to_bytes(pinyin_block.substr(begin, i - begin)));
            begin = i + 1;
        }
        pinyin.push_back(converter.to_bytes(pinyin_block.substr(begin, pinyin_block.length() - begin)));
    }
    const size_t length() const {
        return pinyin.size();
    }
};
std::ostream &operator<<(std::ostream &out, const PinyinUnit &unit) {
    out << "[";
    for (auto it = unit.pinyin.begin(); it != unit.pinyin.end(); it++) {
        out << *it << ", ";
    }
    out << "] = " << unit.freq;
    return out;
}
struct MarkUnit {};

struct TrieNode {
    std::unordered_map<char32_t, size_t> nexts;
    std::u32string str;
    std::vector<PinyinUnit> units;
    size_t fail;
};
struct Automation {
    std::vector<TrieNode> nodes;
    Automation () {
        nodes.resize(2);
    }
    void insert(const std::u32string &str, const PinyinUnit &unit) {
        size_t current = 0;
        for (auto chr = str.begin(); chr != str.end(); chr ++) {
            auto next = nodes[current].nexts.find(*chr);
            if (next == nodes[current].nexts.end()) {
                nodes.push_back(TrieNode());
                current = nodes.size() - 1;
                nodes[current].str = str;
            } else {
                current = next->second;
            }
        }
        nodes[current].units.push_back(unit);
    }
    void build() {
        std::queue<int> q;
        nodes[1].fail = 1;
        q.push(1);
        while (!q.empty()) {
            TrieNode &h = nodes[q.front()];
            q.pop();
            for (auto it=h.nexts.begin(); it != h.nexts.end(); it++ ) {
                size_t f = h.fail;
                const char32_t &chr = it->first;
                while(nodes[f].nexts.find(chr) == nodes[f].nexts.end() && f != 1) {
                    f = nodes[f].fail;
                }
                if (nodes[f].nexts.find(chr) != nodes[f].nexts.end() && f != 1) {
                    nodes[h.nexts[chr]].fail = nodes[f].nexts.find(chr)->second;
                } else {
                    nodes[h.nexts[chr]].fail = 1;
                }
                q.push(h.nexts[chr]);
            }
        }
    }
    void find(const std::u32string &str) const {
        size_t node = 1;
        for (size_t i = 0; i < str.length(); i++) {
            char32_t chr = str[i];
            while(nodes[node].nexts.find(chr) == nodes[node].nexts.end() && node != 1) {
                node = nodes[node].fail;
            }
            if (nodes[node].nexts.find(chr) != nodes[node].nexts.end()) {
                node = nodes[node].nexts.find(chr)->second;
            }
            for (size_t f = node; f != 1; f = nodes[f].fail) {
                if (nodes[f].str.length() == 0)
                    break;
                std::cout << converter.to_bytes(nodes[f].str) << std::endl;
                for (auto it = nodes[f].units.begin(); it != nodes[f].units.end(); it ++) {
                    std:: cout << *it << std::endl;
                }
            }
        }
    }
};
std::pair<std::u32string, PinyinUnit> parseline(const std::u32string &line) {
    size_t p1 = line.find('|');
    size_t p2 = line.find('|', p1 + 1);

    int freq;
    sscanf(converter.to_bytes(line.substr(p2 + 1, line.length() - p2 - 1)).c_str(),"%d", &freq);
    return std::make_pair(std::u32string(line, 0, p1),PinyinUnit(line.substr(p1 + 1, p2 - p1 - 1), freq));
}
}

#endif

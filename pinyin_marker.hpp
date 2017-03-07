#ifndef PINYIN_MARKER_HPP
#define PINYIN_MARKER_HPP

#include <codecvt>
#include <cstdio>
#include <iostream>
#include <locale>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <queue>
#include <functional>
#include "aca.hpp"
namespace pinyin_marker {
std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;

template<typename ElementT>
struct PinyinUnit {
  std::vector<ElementT> pinyin;
  int freq;
  PinyinUnit() {}
  PinyinUnit(std::vector<ElementT> &&pinyin, int freq) : pinyin(pinyin), freq(freq) {}
  PinyinUnit(const std::vector<ElementT> &inyin, int freq) : pinyin(pinyin), freq(freq) {}
  const size_t length() const {
    return pinyin.size();
  }
};

template<typename ElementT>
std::ostream &operator<<(std::ostream &out, const PinyinUnit<ElementT> &unit) {
  out << "[";
  for (auto it = unit.pinyin.begin(); it != unit.pinyin.end(); it++) {
    out << *it << ", ";
  }
  out << "] = " << unit.freq;
  return out;
}

struct PinyinMarker {
public:
  typedef PinyinUnit<std::string> PinyinUnitType;
  Automation<char32_t, std::vector<PinyinUnitType> > aca;
  PinyinMarker() {

  }
  void mark(const std::u32string &str, std::vector<std::string> &result)const {
    std::vector<std::pair<size_t, int> /* length, freq */> compare(str.length());
    aca.find(str, [&compare, &result](const size_t index, const std::vector<PinyinUnitType> &data) -> void {
      for (auto unit_it = data.cbegin(); unit_it != data.cend(); unit_it ++) {
        const auto &unit = *unit_it;
        const size_t pinyin_length =  unit.pinyin.size();
        const size_t start_point = index - pinyin_length + 1;
        for (size_t i = 0; i < pinyin_length; i++) {
          size_t result_index = start_point + i;
          if (std::make_pair(pinyin_length, unit.freq) > compare[result_index]) {
            compare[result_index] = std::make_pair(pinyin_length, unit.freq);
            result[result_index] = unit.pinyin[i];
          }
        }
      }
    });
  }
};
std::pair<std::u32string, PinyinUnit<std::string> > parseline(const std::u32string &line) {
  size_t p1 = line.find('|');
  size_t p2 = line.find('|', p1 + 1);

  int freq;
  sscanf(converter.to_bytes(line.substr(p2 + 1, line.length() - p2 - 1)).c_str(),"%d", &freq);

  // parse pinyin
  std::u32string pinyin_block = line.substr(p1 + 1, p2 - p1 - 1);
  std::vector<std::string> pinyin;
  size_t begin = 0;
  for (size_t i = pinyin_block.find(' '); i != std::u32string::npos; i = pinyin_block.find(' ', i + 1)) {
    pinyin.push_back( converter.to_bytes(pinyin_block.substr(begin, i - begin)));
    begin = i + 1;
  }
  pinyin.push_back(converter.to_bytes(pinyin_block.substr(begin, pinyin_block.length() - begin)));
  //
  return std::make_pair(std::u32string(line, 0, p1),PinyinUnit<std::string>(std::move(pinyin), freq));
}
}

#endif

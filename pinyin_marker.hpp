#ifndef PINYIN_MARKER_HPP
#define PINYIN_MARKER_HPP

#include <codecvt>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <locale>
#include <string>
#include <utility>
#include <vector>
#include <functional>
#include "aca.h"
namespace pinyin_marker {
std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;

template<typename PinyinT>
struct PinyinUnit {
  std::vector<PinyinT> pinyin;
  int freq;
  PinyinUnit() {}
  PinyinUnit(std::vector<PinyinT> &&pinyin, int freq) : pinyin(pinyin), freq(freq) {}
  PinyinUnit(const std::vector<PinyinT> &inyin, int freq) : pinyin(pinyin), freq(freq) {}
  const size_t length() const {
    return pinyin.size();
  }
  bool pinyin_equal(const PinyinUnit &unit) const {
    if (unit.pinyin.size() != pinyin.size()) {
      return false;
    }
    for (size_t i = 0; i < pinyin.size(); i++) {
      if (pinyin[i] != unit.pinyin[i]) {
        return false;
      }
    }
    return true;
  }
};

template<typename PinyinT>
std::ostream &operator<<(std::ostream &out, const PinyinUnit<PinyinT> &unit) {
  out << "[";
  for (auto it = unit.pinyin.begin(); it != unit.pinyin.end(); it++) {
    out << *it << ", ";
  }
  out << "] = " << unit.freq;
  return out;
}

template<typename CharT, typename PinyinT>
struct PinyinMarker {
public:
  typedef PinyinUnit<PinyinT> PinyinUnitType;
  Automation<CharT, std::vector<PinyinUnitType> > aca;
  PinyinMarker() {

  }
  void mark(const std::basic_string<CharT> &str, std::vector<std::string> &result)const {
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
  static std::pair<std::basic_string<CharT>, PinyinUnitType> parseline(const std::basic_string<CharT> &line) {
    size_t p1 = line.find('|');
    size_t p2 = line.find('|', p1 + 1);

    int freq;
    sscanf(converter.to_bytes(line.substr(p2 + 1, line.length() - p2 - 1)).c_str(),"%d", &freq);

    // parse pinyin
    std::basic_string<CharT> pinyin_block = line.substr(p1 + 1, p2 - p1 - 1);
    std::vector<std::string> pinyin;
    size_t begin = 0;
    for (size_t i = pinyin_block.find(' '); i != std::basic_string<CharT>::npos; i = pinyin_block.find(' ', i + 1)) {
      pinyin.push_back( converter.to_bytes(pinyin_block.substr(begin, i - begin)));
      begin = i + 1;
    }
    pinyin.push_back(converter.to_bytes(pinyin_block.substr(begin, pinyin_block.length() - begin)));

    return std::make_pair(std::basic_string<CharT>(line, 0, p1),PinyinUnitType(std::move(pinyin), freq));
  }
  void insert(const std::basic_string<CharT> &str, const PinyinUnitType &unit) {
    aca.insert(str, [&unit](std::vector<PinyinUnitType> &data)->void{
      // deduplicate
      for (size_t i = 0; i < data.size(); i++) {
        if (unit.pinyin_equal(data[i])) {
          data[i].freq += unit.freq;
          return;
        }
      }
      data.push_back(unit);
    });
  }
  void build() {
    aca.build();
  }
};
void load_dicts_char32(PinyinMarker<char32_t, std::string> &marker, std::vector<std::string> filenames) {
  std::string buffer;
  for (auto filename : filenames) {
    std::cout << "loading: " << filename << std::endl;
    std::ifstream dictfile(filename);
    while (getline(dictfile, buffer)) {
      auto line = marker.parseline(converter.from_bytes(buffer));
      marker.insert(line.first, line.second);
    }
  }
}

}

#endif

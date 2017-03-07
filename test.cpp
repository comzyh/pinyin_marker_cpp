#include "pinyin_marker.hpp"
#include <fstream>
#include <iostream>
#include <locale>
#include <string>
#include <cstdio>
#include <vector>
using namespace std;
int main() {
  std::ifstream dictfile("clean_janx_phrases.dat");
  std::string buffer;
  std::u32string line;

  pinyin_marker::PinyinMarker<char32_t, string> marker;

  while (getline(dictfile, buffer)) {
    std::pair<std::u32string, pinyin_marker::PinyinUnit<string> > line;
    line = marker.parseline(pinyin_marker::converter.from_bytes(buffer));
    // cout << pinyin_marker::converter.to_bytes(line.first) << line.second << endl;
    marker.aca.insert(line.first, [&line](vector<pinyin_marker::PinyinUnit<string> > &data)->void{
      data.push_back(line.second);
    });
  }
  // cout << marker.aca.nodes.size() << endl;
  cout << "Building." << endl;
  marker.aca.build();
  cout << "Build finish." << endl;
  while (cin >> buffer) {
    u32string line = pinyin_marker::converter.from_bytes(buffer);
    vector<string> result(line.length());
    marker.mark(line, result);
    for (auto it = result.begin(); it != result.end(); it ++) {
      cout << *it << ", ";
    }
    cout << endl;
  }
  return 0;
}

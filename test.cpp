#include "pinyin_marker.hpp"
#include <fstream>
#include <iostream>
#include <locale>
#include <string>
#include <cstdio>
using namespace std;
int main() {
  std::ifstream dictfile("clean_janx_phrases.dat");
  std::string buffer;
  std::u32string line;

  pinyin_marker::Automation acm;

  while (getline(dictfile, buffer)) {
    std::pair<std::u32string, pinyin_marker::PinyinUnit> line;
    line = pinyin_marker::parseline(pinyin_marker::converter.from_bytes(buffer));
    // cout << pinyin_marker::converter.to_bytes(line.first) << line.second << endl;
    acm.insert(line.first, line.second);
    // break;
  }
  cout << acm.nodes.size() << endl;
  cout << "Building." << endl;
  acm.build();
  cout << "Build finish." << endl;
  while (cin >> buffer) {
    u32string line = pinyin_marker::converter.from_bytes(buffer);
    cout << buffer << endl;
    acm.find(line);
  }
  return 0;
}

#include "pinyin_marker.hpp"
#include <fstream>
#include <iostream>
#include <locale>
#include <string>
#include <cstdio>
#include <vector>
using namespace std;
const vector<string> dicts = {
  "clean_aizuyan_words.dat",
  "clean_janx_phrases.dat",
  "letter.dat",
  "new_marking.dat",
  "polyphone.dat",
  "single_word.dat",
};
int main() {

  std::ifstream dictfile("clean_janx_phrases.dat");
  std::string buffer;
  std::u32string line;

  pinyin_marker::PinyinMarker<char32_t, string> marker;
  pinyin_marker::load_dicts_char32(marker, dicts);

  cout << "Building." << endl;
  marker.build();
  cout << "Build finish." << endl;
  while (getline(cin, buffer)) {
    u32string line = pinyin_marker::converter.from_bytes(buffer);
    vector<string> result(line.length(), "");
    marker.mark(line, result);
    for (auto it = result.begin(); it != result.end(); it ++) {
      cout << *it << ", ";
    }
    cout << endl;
  }
  return 0;
}

#include "pinyin_marker.hpp"
#include <fstream>
#include <iostream>
#include <locale>
#include <string>
#include <cstdio>
#include <vector>
#include <chrono>

using namespace std;
using namespace std::chrono;
using std::chrono::system_clock;

const vector<string> dicts = {
  "clean_aizuyan_words.dat",
  "clean_janx_phrases.dat",
  "letter.dat",
  "new_marking.dat",
  "polyphone.dat",
  "single_word.dat",
};
int main() {

  std::string buffer;
  std::u32string line;

  pinyin_marker::PinyinMarker<char32_t, string> marker;
  pinyin_marker::load_dicts_char32(marker, dicts);

  cout << "Building." << endl;
  marker.build();
  cout << "Build finish." << endl;

  std::ifstream testfile("small");
  system_clock::time_point start_time = system_clock::now();
  while (getline(testfile, buffer)) {
    u32string line = pinyin_marker::converter.from_bytes(buffer);
    size_t end = 0;
    for (size_t i = 0; i < line.length(); i++) {
      if (line[i] == ' '){
        continue;
      }
      line[end++] = line[i];
    }
    line.resize(end);
    vector<string> result(line.length(), "");
    marker.mark(line, result);
    // for (auto it = result.begin(); it != result.end(); it ++) {
    //   cout << *it << ", ";
    // }
    // cout << endl;
  }
  auto finish_time = system_clock::now();
  printf("Finsih Searching: %.4lf s\n", duration_cast<milliseconds>(finish_time - start_time).count()/ 1000.0);
  return 0;
}

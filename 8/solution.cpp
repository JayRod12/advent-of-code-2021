#include <algorithm>
#include <cassert>
#include <climits>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

using tokens = pair<vector<string>, vector<string>>;

set<char> intersection(vector<set<char>>& sets);
set<char> intersection(set<char>& set1, set<char>& set2);

set<char> charsetForString(string& s) {
  set<char> code;
  for (char c : s) {
    code.insert(c);
  }
  return code;
}

set<char> intersection(set<char>& set1, set<char>& set2) {
  vector<set<char>> v{set1, set2};
  return intersection(v);
}

set<char> intersection(vector<set<char>>& sets) {
  set<char>& last_inter = sets[0];
  set<char> cur_inter;
  for (int i = 1; i < sets.size(); i++) {
    set_intersection(sets[i].begin(), sets[i].end(),
                     last_inter.begin(), last_inter.end(),
                     inserter(cur_inter, cur_inter.begin()));
    swap(last_inter, cur_inter);
    cur_inter.clear();
  }
  return last_inter;
}

int solveTokens(tokens ts) {
  // 'abcdefg'
  map<int, set<char>> decoder;
  vector<set<char>> group235;
  vector<set<char>> group069;

  vector<string>& in = ts.first;
  vector<string>& out = ts.second;

  for (string& s : in) {
    int l = s.length();
    switch (l) {
     case 2:
      decoder[1] = charsetForString(s);
      break;
     case 4:
      decoder[4] = charsetForString(s);
      break;
     case 3:
      decoder[7] = charsetForString(s);
      break;
     case 7:
      decoder[8] = charsetForString(s);
      break;
     case 5:
      group235.push_back(charsetForString(s));
      break;
     case 6:
      group069.push_back(charsetForString(s));
      break;
     default:
      assert(false);
    }
  }

  //cout << "New display" << endl;
  //for (const auto& [k, v] : decoder) {
  //  cout << "Entry " << k << " ";
  //  cout << "Values: ";
  //  for (auto e : v) {
  //    cout << e << ", ";
  //  }
  //  cout << endl;
  //}


  // abcdefg
  char a, b, c, d, e, f, g;
  // a
  std::vector<char> diff;
  set_difference(decoder[7].begin(), decoder[7].end(),
                   decoder[1].begin(), decoder[1].end(),
                   back_inserter(diff));
  assert(diff.size() == 1);
  a = diff[0];

  set<char> known;
  known.insert(a);

  // d
  vector<set<char>> group2345(group235);
  group2345.push_back(decoder[4]);

  set<char> ford = intersection(group2345);
  assert(ford.size() == 1);
  d = *ford.begin();

  // b
  set<char> forb = decoder[4];
  forb.erase(d);
  for (char c1 : decoder[1]) {
    forb.erase(c1);
  }
  assert(forb.size() == 1);
  b = *forb.begin();

  // g
  set<char> forg = intersection(group235);
  forg.erase(a);
  forg.erase(d);
  assert(forg.size() == 1);
  g = *forg.begin();

  // f
  vector<set<char>> group0169(group069);
  group0169.push_back(decoder[1]);
  set<char> forf = intersection(group0169);
  assert(forf.size() == 1);
  f = *forf.begin();

  // c
  set<char> forc = decoder[1];
  forc.erase(f);
  assert(forc.size() == 1);
  c = *forc.begin();

  // e
  set<char> fore{'a', 'b', 'c', 'd', 'e', 'f', 'g'};
  fore.erase(a);
  fore.erase(b);
  fore.erase(c);
  fore.erase(d);
  fore.erase(f);
  fore.erase(g);
  assert(fore.size() == 1);
  e = *fore.begin();

  //cout << "a = " << a << endl;
  //cout << "b = " << b << endl;
  //cout << "c = " << c << endl;
  //cout << "d = " << d << endl;
  //cout << "e = " << e << endl;
  //cout << "f = " << f << endl;
  //cout << "g = " << g << endl;

  // map coded letter to original
  map<char, char> letterMap;
  letterMap[a] = 'a';
  letterMap[b] = 'b';
  letterMap[c] = 'c';
  letterMap[d] = 'd';
  letterMap[e] = 'e';
  letterMap[f] = 'f';
  letterMap[g] = 'g';

  // mapping sorted strings to the number they represent
  map<string, int> encoder = {
    {"abcefg", 0},
    {"cf", 1},
    {"acdeg", 2},
    {"acdfg", 3},
    {"bcdf", 4},
    {"abdfg", 5},
    {"abdefg", 6},
    {"acf", 7},
    {"abcdefg", 8},
    {"abcdfg", 9},
  };

  string display;
  for (string& s : out) {
    string fixed;
    for (char c : s) {
      fixed.push_back(letterMap[c]);
    }
    sort(fixed.begin(), fixed.end());
    //cout << "Out " << s << " becomes " << fixed << endl;
    display += to_string(encoder[fixed]);
  }
  cout << "Display is " << display << endl;

  return stoi(display);
}

int solve2(vector<tokens> ts) {

  int total = 0;
  for (auto& t : ts) {
    total += solveTokens(t);
  }
  return total;
}

int solve1(vector<tokens> ts) {
  set unique_sizes{2, 3, 4, 7};

  int count = 0;
  for (auto& t : ts) {
    for (auto& outputToken : t.second) {
      if (unique_sizes.contains(outputToken.length())) count++;
    }
  }
  return count;
}

vector<string> splitString(string s) {
  vector<string> list;
  stringstream ss(s);
  string token;
  while (ss.good()) {
    ss >> token;
    list.push_back(token);
  }
  return list;
}

int main(void) {
  cout << "Problem 8" << endl;

  vector<tokens> ts;

  int v;
  char buf;

  string line;
  while (getline(cin, line)) {
    stringstream ss(line);

    string inputStr;
    getline(ss, inputStr, '|');
    vector<string> inputs = splitString(inputStr);
    cout << "Input part: " << inputStr << endl;

    string outputStr = line.substr(inputStr.length() + 1);
    vector<string> outputs = splitString(outputStr);
    cout << "Output part: " << outputStr << endl;
    ts.push_back(make_pair(inputs, outputs));
  }

  cout << "Part 1" << endl;
  cout << solve1(ts) << endl;
  cout << "Part 2" << endl;
  cout << solve2(ts) << endl;
  //for (const auto& tokens : ts) {
  //  for (const auto& in : tokens.first) {
  //    cout << in << " ";
  //  }
  //  cout << endl;
  //  for (const auto& out : tokens.second) {
  //    cout << out << " ";
  //  }
  //  cout << endl;
  //}
}

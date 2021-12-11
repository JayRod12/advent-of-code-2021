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
using ll = long long;

map<char, int> corruptedScores = {
	{')', 3},
	{']', 57},
	{'}', 1197},
	{'>', 25137},
};

map<char, int> missingScores = {
	{')', 1},
	{']', 2},
	{'}', 3},
	{'>', 4},
};

map<char, char> reverseMap(map<char, char> m) {
  map<char, char> m_rev;
  for (auto [from, to] : m) {
    m[to] = from;
  }
  return m_rev;
}

map<char, char> openToCloseMap = {{'(', ')'}, {'[', ']'}, {'{', '}'}, {'<', '>'}};
map<char, char> closeToOpenMap = reverseMap(openToCloseMap);
set<char> closing = {')', ']', '}', '>'};

ll solve2(vector<vector<char>> lines) {
  vector<ll> scores;
  for (auto& line : lines) {
    vector<char> stack;
    bool corrupted = false;
    for (char c : line) {
      if (openToCloseMap.contains(c)) {
        stack.push_back(c);
      } else {
        char open = stack.back();
        stack.pop_back();
        if (openToCloseMap[open] != c) {
          // mismatch, discard line
          corrupted = true;
          break;
        }
      }
    }
    if (stack.empty() || corrupted) continue;
    // incomplete
    vector<char> missingPart;
    ll missingScore = 0;
    // cout << "Remainder: ";
    while (!stack.empty()) {
      char c = stack.back();
      stack.pop_back();
      char c_match = openToCloseMap[c];
      missingPart.push_back(c_match);
      // cout << c << "+" << c_match << " ";
      missingScore = (missingScore * 5) + missingScores[c_match];
    }
    // cout << " Score = " << missingScore << endl;
    scores.push_back(missingScore);
  }
  sort(scores.begin(), scores.end());

  return scores[scores.size() / 2];
}

int solve1(vector<vector<char>> lines) {
  int mistakeScore = 0;
  for (auto& line : lines) {
    vector<char> stack;
    for (char c : line) {
      if (openToCloseMap.contains(c)) {
        stack.push_back(c);
      } else {
        char open = stack.back();
        stack.pop_back();
        if (openToCloseMap[open] != c) {
          // mismatch
          mistakeScore += corruptedScores[c];
          break;
        }
      }
    }
    // if !stack.size() == 0
  }
  return mistakeScore;
}

int main(void) {
  cout << "Problem 8" << endl;

  vector<vector<char>> lines;

  int v;
  char buf;

  string line;
  while (getline(cin, line)) {
    vector<char> row;
    for (char c : line) {
      row.push_back(c);
    }
    lines.push_back(row);
  }

  cout << "Part 1" << endl;
  cout << solve1(lines) << endl;

  cout << "Part 2" << endl;
  cout << solve2(lines) << endl;
}

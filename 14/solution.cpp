#include <cassert>
#include <climits>
#include <limits>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#include <optional>

using namespace std;

using ll = long long;

void increaseOrInsert(unordered_map<string, ll>& freq, string s, ll count) {
  if (!freq.contains(s)) {
    freq[s] = count;
  } else {
    freq[s] += count;
  }
}

ll stats(unordered_map<string, ll>& freq) {
  unordered_map<char, ll> letterFreq;
  for (auto& [s, count] : freq) {
    for (char c : s) {
      if (!letterFreq.contains(c)) {
        letterFreq[c] = 0;
      }
      letterFreq[c] += count;
    }
  }

  ll maxv = 0;
  ll minv = std::numeric_limits<ll>::max();

  for (auto& [c, count] : letterFreq) {
    ll actualCount = (count + 1) / 2;
    // cout << "Freq: " << c << " = " << actualCount << endl;
    maxv = max(actualCount, maxv);
    minv = min(actualCount, minv);
  }
  return maxv - minv;
}

unordered_map<string, ll> evolve(unordered_map<string, ll>& freq, unordered_map<string, char>& rules) {
  unordered_map<string, ll> nextFreq;

  for (auto& [s, count] : freq) {
    if (!rules.contains(s)) {
      // same as before
      increaseOrInsert(nextFreq, s, count);
    } else {
      char mapping = rules[s];

      string first(1, s[0]);
      first.push_back(mapping);

      string second(1, mapping);
      second.push_back(s[1]);

      increaseOrInsert(nextFreq, first, count);
      increaseOrInsert(nextFreq, second, count);
    }
  }

  return nextFreq;
}

ll solve2(string start, unordered_map<string, char> rules, int iterations) {
  unordered_map<string, ll> freq;

  string s = start;
  for (int i = 0; i < s.length() - 1; i++) {
    string pref = s.substr(i, 2);
    if (!freq.contains(pref)) {
      freq[pref] = 0;
    }
    freq[pref] += 1;
  }

  for (int i = 0; i < iterations; i++) {
    freq = evolve(freq, rules);
  }
  return stats(freq);
}

int stats(string s) {
  unordered_map<char, int> freq;
  for (char c : s) {
    if (!freq.contains(c)) {
      freq[c] = 0;
    }
    freq[c] += 1;
  }
  int maxv = 0;
  int minv = INT_MAX;
  for (auto& [c, val] : freq) {
    maxv = max(val, maxv);
    minv = min(val, minv);
  }
  return maxv - minv;
}

string evolve(string s, unordered_map<string, char> rules) {
  if (s.empty()) return s;
  string next = "";
  for (int32_t i = 0; i < s.length() - 1; i++) {
    string pref = s.substr(i, 2);
    if (i == 0) {
      next += pref[0];
    }
    if (rules.contains(pref)) {
      next += rules[pref];
    }
    next += pref[1];
  }
  return next;
}

int solve(string start, unordered_map<string, char> rules, int iterations) {
  string s = start;
  for (int i = 0; i < iterations; i++) {
    s = evolve(s, rules);
  }
  return stats(s);
}

int main(void) {

  unordered_map<string, char> rules;

  string line;
  getline(cin, line);

  string start(line);

  while (getline(cin, line)) {
    if (line.length() == 0) {
      continue;
    }
    string from = line.substr(0, 2);
    char to = line[line.length() - 1];
    rules[from] = to;
  }

  cout << "Part 1" << endl;
  cout << solve(start, rules, 10) << endl;
  cout << "Part 2" << endl;
  cout << solve2(start, rules, 40) << endl;
}

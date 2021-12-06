#include <cassert>
#include <iostream>
#include <sstream>
#include <vector>
#include <queue>
#include <unordered_map>

using namespace std;

constexpr int kStartLife = 6;
constexpr int kWaitPeriod = 2;

using ll = long long;
using memo_map = unordered_map<ll, ll>; 

ll f(ll t, memo_map& memo) {
  //cout << "f(" << t << ")" << endl;
  if (t <= 0) {
    return 1;
  }

  if (memo.contains(t)) {
    return memo[t];
  }

  ll sol = f(t - 9, memo) + f(t - 7, memo);
  memo[t] = sol;
  return sol;
}

ll solvedp(vector<int> fish_vec, int days) {
  ll ans = 0;
  unordered_map<ll, ll> memo;
  for (int fish : fish_vec) {
    ans += f(days - fish, memo);
  }
  return ans;
}

int solve(vector<int> fish_vec, int days) {
  queue<int, deque<int>> fish_q(deque<int>(fish_vec.begin(), fish_vec.end()));
  for (int day = 0; day < days; day++) {
    int current_size = fish_q.size();
    for (int i = 0; i < current_size; i++) {
      int fish = fish_q.front();
      fish_q.pop();
      if (fish == 0) {
        // reset fish
        fish_q.push(kStartLife);
        // spawn child
        fish_q.push(kStartLife+kWaitPeriod);
      } else {
        fish_q.push(fish - 1);
      }
    }
  }
  return fish_q.size();
}

int main(void) {
  cout << "Problem 6" << endl;

  vector<int> fish;

  int v;
  char buf;

  string line;
  getline(cin, line);
  stringstream ss(line);
  ss >> v;
  fish.push_back(v);
  while (ss.good()) {
    ss >> buf;
    ss >> v;
    fish.push_back(v);
  }

  cout << "Part 1" << endl;
  cout << solve(fish, 80) << endl;

  cout << "Part 2" << endl;
  cout << solvedp(fish, 256) << endl;
}

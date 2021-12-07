#include <cassert>
#include <iostream>
#include <sstream>
#include <vector>
#include <climits>
#include <functional>

using namespace std;

using cost_fn = std::function<int(vector<int>, int)>;

/*
 *
 * Do binary search to find the optimal position for all crabs to move to.
 * 
 * The optimal position is within the range of positions, so compute max/min to
 * mark left and right search points.
 * 
 * For the binary search part, use the cost function at the middle point and
 * immediately to its right (similar to computing a derivative). If l -> r
 * increases, then search left as the minimum will be towards the bottom of the
 * cost function hill. Opposite if l -> r decreases.
 * 
 * This works because the cost functions have a single minimum.
 */

// cost function for part 1
int f(vector<int> xs, int y) {
  int cost = 0;
  for (int x : xs) {
    cost += abs(x - y);
  }
  return cost;
}

// cost function for part 2
// g(y) = 1 + 2 + ... + d = (d+1)*d / 2
int g(vector<int> xs, int y) {
  int cost = 0;
  for (int x : xs) {
    int d = abs(x - y);
    cost += (d+1)*d/2;
  }
  return cost;
}

pair<int, int> solve(vector<int>& xs, int l, int r, cost_fn f) {
  int lv = f(xs, l), rv = f(xs, r);
  while (l + 1 < r) {
    int mid = (l + r) / 2;
    int midv = f(xs, mid);
    cout << "(l, lv) = (" << l << ", " << lv << ") |\t" ;
    cout << "(m, mv) = (" << mid << ", " << midv << ") |\t";
    cout << "(r, rv) = (" << r << ", " << rv << ") |" << endl;

    int midp = mid + 1;
    int midpv = f(xs, midp);

    if (midpv > midv) {
      // falling hill r->l, search left
      r = mid;
      rv = midv;
    } else {
      // falling hill l->r, search right
      l = mid;
      lv = midv;
    }
  }

  assert (l + 1 == r);
  if (lv < rv) {
    return make_pair(l, lv);
  } else {
    return make_pair(r, rv);
  }
}

pair<int, int> solve(vector<int>& xs, cost_fn f) {
  int minx = INT_MAX, maxx = INT_MIN;
  for (int x : xs) {
    minx = min(x, minx);
    maxx = max(x, maxx);
  }
  return solve(xs, minx, maxx, f);
}


int main(void) {
  cout << "Problem 7" << endl;

  vector<int> xs;

  int v;
  char buf;

  string line;
  getline(cin, line);
  stringstream ss(line);
  ss >> v;
  xs.push_back(v);
  while (ss.good()) {
    ss >> buf;
    ss >> v;
    xs.push_back(v);
  }

  cout << "Part 1" << endl;
  auto [position, cost] = solve(xs, f);
  cout << "Best position is " << position << " with cost " << cost << endl;

  cout << "Part 2" << endl;
  auto [position_g, cost_g] = solve(xs, g);
  cout << "Best position is " << position_g << " with cost " << cost_g << endl;
}

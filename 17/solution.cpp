#include <algorithm>
#include <cassert>
#include <climits>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#include <utility>
#include <memory>
#include <functional>

using namespace std;

constexpr int kMaxSteps = 1000;
constexpr int kMaxVel = 1000;

struct Range {
  int xStart;
  int xEnd;
  int yStart;
  int yEnd;
};

struct Point {
  int x;
  int y;

  bool operator==(const Point& other) const {
    return x == other.x && y == other.y;
  }

  friend ostream& operator<<(ostream& os, const Point& p) {
    os << "Point (" << p.x << "," << p.y << ") ";
    return os;
  }
};

struct Vel {
  int dx;
  int dy;

  friend ostream& operator<<(ostream& os, const Vel& v) {
    os << "Vel (" << v.dx << "," << v.dy << ") ";
    return os;
  }
};

void updateVel(Vel& curVel) {
  if (curVel.dx > 0) {
    curVel.dx -= 1;
  } else if (curVel.dx < 0) {
    curVel.dx += 1;
  }
  curVel.dy -= 1;
}

pair<bool, int> check(Point& target, Vel& startVelocity) {
  int maxy = 0;
  Vel curVel = startVelocity; 
  Point pos{0, 0};
  for (int i = 0; i < kMaxSteps; i++) { // stop early
    pos.x += curVel.dx;
    pos.y += curVel.dy;
    updateVel(curVel);
    maxy = max(maxy, pos.y);
    if (pos == target) {
      return make_pair(true, maxy);
    }
  }
  return make_pair(false, maxy);
}
void maybeUpdateHighest(int x, int y, int vx, int vy, int& maxy, int& maxvx, int& maxvy) {
  Point p{x, y};
  Vel v{vx, vy};
  auto [ok, localmax] = check(p, v);
  if (ok) {
    if (localmax > maxy) {
      maxy = localmax;
      maxvx = vx;
      maxvy = vy;
    }
  }
}

pair<bool, int> searchVelHorizontal(int target, int steps, bool log) {
  int begin = -kMaxVel;
  int end = kMaxVel;

  for (int v = begin; v < end; v++) {
    // q: do you get to `target` at start speed `v` in exactly `steps` steps
    int stepsToTake = min(abs(v), steps);
    // int position = stepsToTake*(stepsToTake+1)/2;

    int vend;
    if (v > 0) {
      vend = v - stepsToTake + 1;
    } else {
      vend = v + stepsToTake - 1;
    }

    int position = stepsToTake * (v + vend) / 2;
    if (log) {
      cout << "steps: " << stepsToTake << " position: " << position << endl;
    }

    if (position == target) {
      if (log) {
        cout << "Found suitable v: " << v << endl;
      }
      return {true, v};
    }
  }
  return {false, 0};
}

pair<bool, int> searchVelVertical(int target, int steps, bool log) {
  int begin = -kMaxVel;
  int end = kMaxVel;

  if (log) {
    cout << "Target:" <<target<< " Steps:" << steps<<endl;
  }

  for (int v = begin; v < end; v++) {
    // q: do you get to `target` at start speed `v` in exactly `steps` steps
    int stepsTaken = 0;
    int position = 0;
    int vel = v;
    if (vel < 0) {
      // vel -> vel - steps + 1
      position += steps*(vel + (vel - steps + 1)) / 2;
      if (position == target) {
        if (log) {
          cout << "Found suitable v=" << v <<endl;
        }
        return {true, v};
      } else {
        continue;
      }
    } else if (vel > 0) {
      // there are v0 steps until velocity stops
      int stepsToTake = min(vel, steps);
      position += stepsToTake*(stepsToTake+1)/2;
      stepsTaken += stepsToTake;
      vel = 0; 
      if (log) {
        cout << "steps: " << stepsTaken << " position:" << position << endl;
      }
    }

    // Velocity becomes negative
    if (stepsTaken < steps && vel == 0) {
      stepsTaken++;
      vel -= 1;
      if (log) {
        cout << "stepsTaken:" << stepsTaken << endl;
      }
    }

    // velocity = [velocity ... velocity - stepsToTake]
    if (stepsTaken < steps) {
      int stepsToTake = steps - stepsTaken;
      position -= stepsToTake*(stepsToTake+1)/2;
      stepsTaken += stepsToTake;

      if (log) {
        cout << "stepsTaken:" << stepsToTake << " position: " << position << endl;
      }
    }

    if (position == target) {
      if (log) {
        cout << "Found target at v=" << v <<endl;
      }
      return {true, v};
    }
  }
  return {false, 0};
}

/*
 * Finding formulaically is a bit hard for some parts of the parabollic path, abandoing the approach
 */

//// find velocity to reach a target in return trip: go up, then start falling down, go below 0
//pair<bool, int> findVelAdvancedA(int target, int steps) {
//
//  // hit while going up
//  if (target > 0) {
//    return {false, 0};
//  }
//
//  // v0 = (2*target-k*k+3*k-2) / (2*(1-k))
//  int k = steps + 1;
//  int numer = 2*target+k*(1+k);
//  int denom = 2*(1-k);
//  if (numer % denom != 0) {
//    return {false, 0};
//  } else {
//    return {true, numer / denom};
//  }
//
//  return {false, 0};
//}
//
//// find velocity to reach positive point target, with decreasing velocity by -1 in each step until 0
//pair<bool, int> findVelSimple(int target, int steps) {
//  if (steps == 0) {
//    return {false, 0};
//  }
//  if (steps == 1) {
//    return {true, target};
//  }
//  // v0 = (2*target + k(1+k)) / (2*(1+k))
//  int k = steps - 1;
//  int numer = 2*target+k*(1+k);
//  int denom = 2*(1+k);
//  if (numer % denom != 0) {
//    return {false, 0};
//  } else {
//    return {true, numer / denom};
//  }
//}

pair<int, int> solve(Range& r) {
  int maxy = INT_MIN;
  int maxvx = 0;
  int maxvy = 0;

  set<pair<int, int>> distinctVelocities;
  for (int x = r.xStart; x <= r.xEnd; x++) {
    for (int y = r.yStart; y <= r.yEnd; y++) {
      for (int steps = 1; steps < kMaxSteps; steps++) {
        auto [okx, vx] = searchVelHorizontal(x, steps, false);
        auto [oky, vy] = searchVelVertical(y, steps, false);
        if (okx && oky) {
          distinctVelocities.emplace(vx, vy);
          //cout << "Reach x=" << x << " with speed vx = " << vx << " in " << steps << " steps" << endl;
          //cout << "Reach y=" << y << " with speed vy = " << vy << " in " << steps << " steps" << endl;
          //cout << endl;
          maybeUpdateHighest(x, y, vx, vy, maxy, maxvx, maxvy);
        }
      }
    }
  }
  cout << "Maxy: " << maxy << " with velocity: " << maxvx << "," << maxvy << endl;
  return make_pair(maxy, distinctVelocities.size());
}

void tests() {
  // search pos p in k steps == v velocity
  assert(searchVelVertical(-9, 9, false).second == 3);
  assert(searchVelVertical(6, 3, false).second == 3);
  assert(searchVelHorizontal(28, 7, false).second == 7);
  assert(searchVelVertical(-7, 7, false).second == 2);
}

int main(void) {
  string line;
  getline(cin, line);
  string pref = "target area: x=";
  line = line.substr(pref.length());
  cout << line << endl;
  stringstream ss(line);
  int x0, x1, y0, y1;
  char buf;
  ss >> x0 >> buf >> buf >> x1;
  ss >> buf >> buf >> buf;
  cout << ss.str() << endl;
  ss >> y0 >> buf >> buf >> y1;

  cout << "x in [" << x0 << "," << x1 << "]" << endl;
  cout << "y in [" << y0 << "," << y1 << "]" << endl;

  Range r{x0, x1, y0, y1};
  auto [maxy, distinctVel] = solve(r);
  cout << "Max: " << maxy << endl;
  cout << "Distinct: " << distinctVel << endl;
  tests();
}

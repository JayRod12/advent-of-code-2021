#include <cassert>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#include <optional>

using namespace std;

void printGrid(set<pair<int, int>>& points) {
  int maxx = 0;
  int maxy = 0;
  for (auto& [x, y] : points)  {
    maxx = max(x, maxx);
    maxy = max(y, maxy);
  }
  
  vector<vector<bool>> grid(maxx + 1, vector<bool>(maxy + 1));

  for (auto& [x, y] : points)  {
    grid[x][y] = true;
  }

  cout << "Grid " << endl;
  for (int j = 0; j <= maxy; j++) {
    string row = "";
    for (int i = 0; i <= maxx; i++) {
      if (grid[i][j]) {
        row += '#';
      } else {
        row += '.';
      }
    }
    cout << row << endl;
  }
}

optional<pair<int, int>> applyFold(const pair<int, int>& p, char dim, int foldVal) {
  int value;
  if (dim == 'x') {
    value = p.first;
  } else {
    value = p.second;
  }

  if (value < foldVal) {
    return make_optional(p);
  }

  int newVal = foldVal - (value - foldVal);
  if (newVal < 0) {
    return nullopt;
  }

  if (dim == 'x') {
    return make_optional(make_pair(newVal, p.second));
  } else {
    return make_optional(make_pair(p.first, newVal));
  }
}

int solve(vector<pair<int, int>> points, vector<pair<char, int>> folds, bool onlyFirstFold) {
  set<pair<int, int>> pointset;
  for (auto& p : points) {
    pointset.insert(p);
  }

  set<pair<int, int>> nextset;
  for (auto& [dim, foldVal] : folds) {
    // printGrid(pointset);
    for (auto& p : pointset) {
      auto p2 = applyFold(p, dim, foldVal);
      if (p2.has_value()) {
        //cout << "x, y = " << p.first << "," << p.second;
        //cout << " -> ";
        //cout << "x, y = " << p2.value().first << "," << p2.value().second << endl;
        nextset.insert(p2.value());
      } else {
        cout << "empty fold" << endl;
      }
    }
    pointset = std::move(nextset);
    nextset.clear();

    if (onlyFirstFold) {
      return pointset.size();
    }
  }
  printGrid(pointset);

  return pointset.size();
}

int main(void) {

  vector<pair<int, int>> points;
  vector<pair<char, int>> folds;
  int x, y;
  char buf;
  char dim;
  int v;
  const string foldString = "fold along ";

  string line;
  while (getline(cin, line)) {
    if (line.length() == 0) {
      continue;
    }
    char first = line[0];
    if ('0' <= first && first <= '9') {
      // coords
      stringstream ss(line);
      ss >> x;
      ss >> buf;
      ss >> y;
      points.push_back(make_pair(x, y));
    } else {
      // fold 
      string data = line.substr(foldString.length());
      stringstream ss(data);

      ss >> dim;
      ss >> buf;
      ss >> v;
      folds.push_back(make_pair(dim, v));
    }
  }

  //for (auto& [x, y] : points) {
  //  cout << "(x, y) = " << x << ", " << y << endl;
  //}


  //for (auto& [dim, v] : folds) {
  //  cout << "Fold: " << dim << " = " << v << endl;
  //}

  cout << "Part 1" << endl;
  cout << solve(points, folds, true) << endl;
  cout << "Part 2" << endl;
  cout << solve(points, folds, false) << endl;
}

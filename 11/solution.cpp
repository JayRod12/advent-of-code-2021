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

using Grid = vector<vector<pair<int, bool>>>;

void showGrid(Grid& grid) {
  for (auto& row : grid) {
    for (auto p : row) {
      cout << p.first << ", ";
    }
    cout << endl;
  }
}

vector<pair<int, int>> getAdjacent(Grid& grid, int i, int j) {
  vector<pair<int, int>> adj;
  
  for (int di = -1; di <= 1; di++) {
    int ii = i + di;
    if (ii < 0 || ii >= grid.size()) continue;
    for (int dj = -1; dj <= 1; dj++) {
      int jj = j + dj;
      if (jj >= 0 && jj < grid[0].size()) {
        adj.push_back(make_pair(ii, jj));
      }
    }
  }
  return adj;
}

int maybeFlash(Grid& grid, int i, int j) {
  if (grid[i][j].first <= 9 || grid[i][j].second) return 0;

  int fCount = 1;
  grid[i][j].second = true;
  for (auto [ii, jj] : getAdjacent(grid, i, j)) {
    grid[ii][jj].first += 1;
    fCount += maybeFlash(grid, ii, jj);
  }
  return fCount;
}

int step(Grid& grid) {
  for (int i = 0; i < grid.size(); i++) {
    for (int j = 0; j < grid[0].size(); j++) {
      grid[i][j].first += 1;
    }
  }

  int fCount = 0;
  for (int i = 0; i < grid.size(); i++) {
    for (int j = 0; j < grid[0].size(); j++) {
      fCount += maybeFlash(grid, i, j);
    }
  }

  for (int i = 0; i < grid.size(); i++) {
    for (int j = 0; j < grid[0].size(); j++) {
      if (grid[i][j].second) {
        grid[i][j].first = 0;
        grid[i][j].second = false;
      }
    }
  }
  return fCount;
}

int solve2(Grid& grid) {
  int gridSize = grid.size() * grid[0].size();
  int stepCount = 1;
  while (step(grid) != gridSize) {
    stepCount++;
  }
  return stepCount;
}

int solve1(Grid& grid, int steps) {
  //cout << "Initial grid" << endl;
  //showGrid(grid);
  int flashes = 0;
  for (int st = 1; st <= steps; st++) {
    flashes += step(grid);
    //cout << "Step " << st << endl;
    //showGrid(grid);
  }
  return flashes;
}

int main(void) {
  Grid grid;

  int v;

  string line;
  while (getline(cin, line)) {
    vector<pair<int, bool>> row;
    for (auto x : line) {
      row.push_back(make_pair(static_cast<int>(x - '0'), false));
    }
    grid.push_back(row);
  }
  Grid gridCopy{grid};

  cout << "Part 1" << endl;
  cout << solve1(grid, 100) << endl;

  cout << "Part 2" << endl;
  cout << solve2(gridCopy) << endl;
}

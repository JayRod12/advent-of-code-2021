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

vector<pair<int, int>> getAdjacent(vector<vector<int>>& grid, int i, int j) {
  vector<pair<int, int>> adj;
  int dir[4][2] = {{i-1, j}, {i, j-1}, {i+1, j}, {i, j+1}};
  for (auto row : dir) {
    int ii = row[0];
    int jj = row[1];
    if (ii >= 0 && ii < grid.size() && jj >= 0 && jj < grid[0].size()) {
      adj.push_back(make_pair(ii, jj));
    }
  }
  return adj;
}

bool isLowPoint(vector<vector<int>>& grid, int i, int j) {
  int pValue = grid[i][j];
  for (auto [ii, jj] : getAdjacent(grid, i, j)) {
    if (pValue >= grid[ii][jj]) return false;
  }
  return true;
}

int dfs(vector<vector<int>>& grid, int i, int j, vector<vector<bool>>& visited, int count) {
  if (visited[i][j]) {
    return count;
  }
  visited[i][j] = true;
  int nextCount = count;
  for (auto [ii, jj] : getAdjacent(grid, i, j)) {
    if (grid[ii][jj] != 9) {
      nextCount = dfs(grid, ii, jj, visited, nextCount);
    }
  }
  return nextCount + 1;
}

int solve2(vector<vector<int>>& grid) {
  vector<pair<int, int>> lowPoints;
  for (int i = 0; i < grid.size(); i++) {
    for (int j = 0; j < grid[0].size(); j++) {
      if (isLowPoint(grid, i, j)) {
        lowPoints.push_back(make_pair(i, j));
      }
    }
  }

  vector<vector<bool>> visited(grid.size(), vector<bool>(grid[0].size(), false));
  vector<int> basinSizes;
  for (auto [i, j] : lowPoints) {
    int size = dfs(grid, i, j, visited, 0);
    basinSizes.push_back(size);
  }

  sort(basinSizes.begin(), basinSizes.end());
  int n = basinSizes.size();
  return basinSizes[n-1]*basinSizes[n-2]*basinSizes[n-3];
}


int solve1(vector<vector<int>>& grid) {
  int riskLevel = 0;
  for (int i = 0; i < grid.size(); i++) {
    for (int j = 0; j < grid[0].size(); j++) {
      if (isLowPoint(grid, i, j)) riskLevel += grid[i][j] + 1;
    }
  }
  return riskLevel;
}

int main(void) {
  cout << "Problem 8" << endl;

  vector<vector<int>> grid;

  int v;
  char buf;

  string line;
  while (getline(cin, line)) {
    vector<int> row;
    for (char c : line) {
      row.push_back(static_cast<int>(c - '0'));
    }
    grid.push_back(row);
  }
  cout << grid.size() << " x " << grid[0].size() << " grid" << endl;

  cout << "Part 1" << endl;
  cout << solve1(grid) << endl;

  cout << "Part 2" << endl;
  cout << solve2(grid) << endl;
}

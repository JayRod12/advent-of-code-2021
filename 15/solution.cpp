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

using namespace std;

using Grid = vector<vector<pair<int, int>>>; // value, minDistance

vector<pair<int, int>> getAdjacent(Grid& grid, int i, int j) {
  vector<pair<int, int>> adj;
  
  vector<pair<int, int>> edges = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
  for (auto [di, dj] : edges) {
    int ii = i + di;
    int jj = j + dj;
    if (ii >= 0 && ii < grid.size() && jj >= 0 && jj < grid[0].size()) {
      adj.push_back(make_pair(ii, jj));
    }
  }
  return adj;
}

int weight(Grid& grid, int x, int y) {
  return grid[x][y].second;
}

// This could be done with a min heap more efficiently, but it's not needed for 100x100 or 500x500 grid
pair<pair<int, int>, int> getClosestNeighbour(
    Grid& grid,
    set<pair<int, int>>& ns,
    set<pair<int, int>>& mst) {

  int minw = INT_MAX;
  int minx = 0;
  int miny = 0;
  for (auto& [x, y] : ns) {
    int w = weight(grid, x, y);
    if (w <= minw) {
      minx = x;
      miny = y;
      minw = w;
    }
  }
  return make_pair(make_pair(minx, miny), minw);
}

int dijsktra(Grid& grid) {
  int n = grid.size(), m = grid[0].size();
  int total = n*m;

  // minimum spanning tree
  set<pair<int, int>> mst;

  auto cmp = [&](auto& p1, auto& p2) {
    return grid[p1.first][p1.second].second < grid[p2.first][p2.second].second;
  };

  // neighbours
  set<pair<int, int>> ns;

  grid[0][0].second = 0;
  for (auto& [x, y] : getAdjacent(grid, 0, 0)) {
    ns.insert(make_pair(x, y));
    grid[x][y].second = grid[x][y].first; // set weight = value for first nodes
  }

  auto start = make_pair(0, 0);
  mst.insert(start);

  while (mst.size() < total) {
    // pick cheapest edge
    auto [u, uw] = getClosestNeighbour(grid, ns, mst);
    mst.insert(u);
    ns.erase(u);

    auto [ux, uy] = u;

    for (auto v : getAdjacent(grid, ux, uy)) {
      if (mst.contains(v)) {
        continue;
      }

      auto [x, y] = v;

      if (uw + grid[x][y].first < grid[x][y].second) {
        grid[x][y].second = uw + grid[x][y].first;
      }

      ns.insert(v);
    }
  }
  return grid[n-1][m-1].second;
}

void showGrid(Grid& grid) {
  for (auto& row : grid) {
    for (auto p : row) {
      cout << p.first << "(" << p.second << "), ";
    }
    cout << endl;
  }
}

void showGridBasic(Grid& grid) {
  for (auto& row : grid) {
    for (auto p : row) {
      cout << p.first;
    }
    cout << endl;
  }
}


int solve1(Grid& grid) {
  cout << grid.size() << " x " << grid[0].size() << endl;
  //showGrid(grid);
  int x = dijsktra(grid);
  //showGrid(grid);
  return x;
}

Grid multiply(Grid& grid) {
  int n = grid.size(), m = grid[0].size();
  Grid big(grid.size()*5, vector<pair<int, int>>(grid[0].size()*5));
  for (int k = 0; k < 5; k++) {
    for (int l = 0; l < 5; l++) {
      for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
          // every time we wrap from 9 -> 1, so if we wrap twice, e.g. for 8 + 16 = 24 -> 4 + 2 -> 6
          int newval = grid[i][j].first + k + l;
          int diff = newval/10;
          newval %= 10;
          newval += diff;

          int ii = n*k+i;
          int jj = m*l+j;
          big[ii][jj].first = newval;
          big[ii][jj].second = INT_MAX;
        }
      }
    }
  }
  return big;
}

int solve2(Grid& grid) {
  cout << grid.size() << " x " << grid[0].size() << endl;
  auto big = multiply(grid);
  //showGridBasic(big);
  int x = dijsktra(big);
  return x;
}

int main(void) {
  Grid grid;

  int v;

  string line;
  while (getline(cin, line)) {
    vector<pair<int, int>> row;
    for (auto x : line) {
      row.push_back(make_pair(static_cast<int>(x - '0'), INT_MAX));
    }
    grid.push_back(row);
  }
  Grid gridCopy{grid};

  cout << "Part 1" << endl;
  cout << solve1(grid) << endl;
  cout << "Part 2" << endl;
  cout << solve2(grid) << endl;
}

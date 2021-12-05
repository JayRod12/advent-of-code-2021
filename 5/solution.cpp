#include <cassert>
#include <iostream>
#include <sstream>
#include <vector>
#include <functional>

using namespace std;

constexpr int DANGER_LEVEL = 2;

class Point {
 public:
  Point(int x, int y) : x(x), y(y) {}

  friend ostream& operator<<(ostream& os, Point& p) {
    os << "(" << p.x << ", " << p.y << ") ";
    return os;
  }

  int x;
  int y;
 private:
};

using Line = pair<Point, Point>;
using Board = vector<vector<int>>;

Point readPoint(string& s) {
  const string delimiter = " -> ";

  size_t pos = s.find(delimiter);
  string pointAsStr;
  if (pos != string::npos) {
    pointAsStr = s.substr(0, pos);
    s.erase(0, pos + delimiter.length());
  } else {
    pointAsStr = s;
  }

  stringstream ss(pointAsStr);
  int x;
  int y;
  char buf;
  ss >> x >> buf >> y;
  Point p(x, y);
  return p;
}

void showBoard(Board& board) {
  // reverse [dim1][dim2] to show dim1 as columns(x) and dim2 as rows(y)

  for (int j = 0; j < board[0].size(); j++) {
    for (int i = 0; i < board.size(); i++) {
      cout << board[i][j] << ", ";
    }
    cout << endl;
  }
}

using Comparator = function<bool(int, int)>;

pair<Comparator, int> initDirection(int from, int to) {
  Comparator c;
  int delta = 1;
  if (from < to) {
    c = less<int>();
  } else {
    c = greater<int>();
    delta *= -1;
  }
  return make_pair(c, delta);
}

void mark2(Board& board, Line line) {
  Point p1 = line.first;
  Point p2 = line.second;

  auto dirx = initDirection(p1.x, p2.x);
  auto diry = initDirection(p1.y, p2.y);
  auto xComp = dirx.first;
  auto xDelta = dirx.second;
  auto yComp = diry.first;
  auto yDelta = diry.second;

  // cout << "Mark " << p1 << " -> " << p2 << endl;
  int x = p1.x;
  int y = p1.y;
  while (x != p2.x || y != p2.y) {
    // cout << "\tmark " << x << "," << y << endl;
    board[x][y] += 1;
    if (xComp(x, p2.x)) x += xDelta;
    if (yComp(y, p2.y)) y += yDelta;
  }
  // last point
  board[x][y] += 1;
  // cout << "\tmark " << x << "," << y << endl;
}

void mark(Board& board, Line line) {
  Point p1 = line.first;
  Point p2 = line.second;

  // No diag
  if (p1.x != p2.x && p1.y != p2.y) {
    // cout << "Unsupported diagonal line: " << p1 << " -> " << p2 << endl;
    return;
  }

  if (p1.x > p2.x || p1.y > p2.y)
    return mark(board, make_pair(p2, p1));

  // cout << "Mark " << p1 << " -> " << p2 << endl;

  int x = p1.x;
  int y = p1.y;
  while (x < p2.x || y < p2.y) {
    // cout << "\tmark " << x << "," << y << endl;
    board[x][y] += 1;
    if (x < p2.x) x++;
    if (y < p2.y) y++;
  }
  // last point
  board[x][y] += 1;
  // cout << "\tmark " << x << "," << y << endl;
}

int solve(vector<Line>& lines, int sizeX, int sizeY, bool partTwo) {
  vector<vector<int>> board(sizeX, vector(sizeY, 0));
  for (Line& line : lines) {
    if (partTwo) {
      mark2(board, line);
    } else {
      mark(board, line);
    }
  }
  // showBoard(board);
  int danger = 0;
  for (int x = 0; x < sizeX; x++) {
    for (int y = 0; y < sizeY; y++) {
      if (board[x][y] >= DANGER_LEVEL) danger++;
    }
  }
  return danger;
}

int main(void) {
  cout << "Problem 5" << endl;

  vector<Line> lines;
  int maxX = 0;
  int maxY = 0;

  string line;
  while (getline(cin, line)) {
    Point p1 = readPoint(line);
    Point p2 = readPoint(line);
    maxX = max(maxX, max(p1.x, p2.x));
    maxY = max(maxY, max(p1.y, p2.y));
    lines.push_back(make_pair(p1, p2));
  }

  int danger;
  danger = solve(lines, maxX + 1, maxY + 1, false);
  cout << "Danger part 1: " << danger << endl;

  danger = solve(lines, maxX + 1, maxY + 1, true);
  cout << "Danger part 2: " << danger << endl;
}

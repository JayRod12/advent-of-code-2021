#include <cassert>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

enum class Direction {
  Forward,
  Down,
  Up,
};

Direction strToEnum(string& str) {
  if (str[0] == 'f') {
    return Direction::Forward;
  } else if (str[0] == 'd') {
    return Direction::Down;
  } else if (str[0] == 'u') {
    return Direction::Up;
  } else {
    assert(false);
  }
}

int solve2(vector<pair<Direction, int>>& arr) {
  int depth = 0;
  int h_position = 0;
  int aim = 0;
  for (auto& p : arr) {
    auto d = p.first;
    auto v = p.second;
    switch (d) {
      case Direction::Forward:
        h_position += v;
        depth += aim * v;
        break;
      case Direction::Down:
        aim += v;
        break;
      case Direction::Up:
        aim -= v;
        break;
    }
  }

  cout << "Depth: " << depth << ". H Position: " << h_position << endl;
  return depth * h_position;
}

int solve(vector<pair<Direction, int>>& arr) {
  int depth = 0;
  int h_position = 0;
  for (auto& p : arr) {
    auto d = p.first;
    auto v = p.second;
    switch (d) {
      case Direction::Forward:
        h_position += v;
        break;
      case Direction::Down:
        depth += v;
        break;
      case Direction::Up:
        depth -= v;
        break;
    }
  }

  cout << "Depth: " << depth << ". H Position: " << h_position << endl;
  return depth * h_position;
}

int main(void) {
  cout << "Problem 1" << endl;

  string direction;
  int val;
  vector<pair<Direction, int> > arr;

  for (string line; getline(cin, line); ) {
    stringstream ss(line);
    ss >> direction;
    ss >> val;
    auto p = make_pair(strToEnum(direction), val);
    arr.push_back(p);
  }
  cout << "Result: " << solve(arr) << endl;
  cout << "Result part 2: " << solve2(arr) << endl;
}

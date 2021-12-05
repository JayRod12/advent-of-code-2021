#include <cassert>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

constexpr int boardSize = 5;

using Board = vector<vector<pair<int, bool>>>;

void reset(vector<Board>& boards) {
  for (auto& b : boards) {
    for (auto& row : b) {
      for (auto& p : row) {
        p.second = false;
      }
    }
  }
}

void showBoard(Board& board) {
  for (auto& row : board) {
    for (auto p : row) {
      cout << p.first << ", ";
    }
    cout << endl;
  }
}


bool checkRowWin(Board& board, int row) {
  for (int j = 0; j < board[row].size(); j++) {
    if (!board[row][j].second) return false;
  }
  return true;
}

bool checkColWin(Board& board, int col) {
  for (int i = 0; i < board.size(); i++) {
    if (!board[i][col].second) return false;
  }
  return true;
}

bool checkWin(Board& board, int row, int col) {
  return checkRowWin(board, row) || checkColWin(board, col);
}

bool checkWin(Board& board) {
  for (int i = 0; i < board.size(); i++) {
    if (checkRowWin(board, i)) return true;
  }
  for (int j = 0; j < board[0].size(); j++) {
    if (checkColWin(board, j)) return true;
  }
  return false;
}

bool mark(Board& board, int number) {
  for (int i = 0; i < board.size(); i++) {
    for (int j = 0; j < board[i].size(); j++) {
      auto& entry_pair = board[i][j];
      if (entry_pair.first == number) {
        entry_pair.second = true;
        return checkWin(board, i, j);
      }
    }
  }
  return checkWin(board);
}

int sumUnmarked(Board& board) {
  int sum = 0;
  for (int i = 0; i < board.size(); i++) {
    for (int j = 0; j < board[i].size(); j++) {
      auto& entry_pair = board[i][j];
      if (!entry_pair.second) sum += entry_pair.first;
    }
  }
  return sum;
}

bool solve2(vector<int>& numbers, vector<Board>& boards) {
  int boardsToWin = boards.size();
  vector<bool> boardsWhichWon(boards.size(), false);

  for (int x : numbers) {
    for (int i = 0; i < boards.size(); i++) {
      if (boardsWhichWon[i]) {
        continue;
      }
      Board& b = boards[i];
      if (mark(b, x)) {
        boardsWhichWon[i] = true;
        boardsToWin--;

        if (boardsToWin > 0) {
          continue;
        }

        int sumU = sumUnmarked(b);
        int score = x * sumU;
        cout << "Last board to win " << i << " with number " << x;
        cout << ". Sum unmarked " << sumU;
        cout << ". Overall score: " << score;
        cout << endl;

        return true;
      }
    }
  }
  return false;
}

bool solve(vector<int>& numbers, vector<Board>& boards) {
  bool winner = false;
  for (int x : numbers) {
    for (int i = 0; i < boards.size() && !winner; i++) {
      Board& b = boards[i];
      if (mark(b, x)) {
        int sumU = sumUnmarked(b);
        int score = x * sumU;
        cout << "Board " << i << " won! with number " << x;
        cout << ". Sum unmarked " << sumU;
        cout << ". Overall score: " << score;
        cout << endl;
        winner = true;
      }
    }
    if (winner) return true;
  }
  return false;
}

int main(void) {
  cout << "Problem 4" << endl;

  string bins;
  vector<int> nums;
  vector<Board> boards;

  // read bingo numbers
  {
    string line;
    int v;
    getline(cin, line);
    stringstream ss(line);

    while (ss.good()) {
      ss >> v;
      string substr;
      getline(ss, substr, ',');
      nums.push_back(v);
    }
  }

  // read boards
  {
    string line;
    int v;
    while (getline(cin, line)) {
      assert(line.size() == 0);
      Board board;
      for (int i = 0; i < boardSize; i++) {
        auto& stream = getline(cin, line);
        assert(stream.good());
        stringstream ss(line);
        vector<pair<int, bool>> row;
        for (int j = 0; j < boardSize; j++) {
          ss >> v;
          row.push_back(make_pair(v, false));
        }
        board.push_back(row);
      }
      //cout << "<<<<<<<<" << endl;
      //showBoard(board);
      //cout << ">>>>>>>>" << endl;
      boards.push_back(board);
    }
  }

  cout << "Part 1" << endl;
  solve(nums, boards);
  reset(boards);
  cout << "Part 2" << endl;
  solve2(nums, boards);
}

#include <cassert>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

vector<int> filter(vector<int>& indices, vector<string>& bits, int bit_pos, char target_bit) {
  if (indices.size() == 1) {
    return indices;
  }

  vector<int> next_indices;
  for (int j = 0; j < indices.size(); j++) {
    //cout << bits[indices[j]] << " bit " << bit_pos << " target " << target_bit << " -> ";
    if (bits[indices[j]][bit_pos] == target_bit) {
      //cout << "Kept" << endl;
      next_indices.push_back(indices[j]);
    } else {
      //cout << "Filtered" << endl;
    }
  }
  return next_indices;
}

vector<int> next(vector<int>& indices, vector<string>& bits, int bit_pos, bool compareMore) {
  int win = static_cast<int>((indices.size() + 1) / 2);
  int ones = 0;
  for (int str_idx : indices) {
    auto& s = bits[str_idx];
    char c = s[bit_pos];
    if (c == '1') ones += 1;
  }
  char digitA, digitB;
  if (compareMore) {
    digitA = '1';
    digitB = '0';
  } else {
    digitB = '1';
    digitA = '0';
  }

  char digit = (ones >= win) ? digitA : digitB;
  return filter(indices, bits, bit_pos, digit);
}

// v >= (N + 1) / 2
// odd: 2N+1 -> >= N + 1
// even: 2N -> => N
int solve2(vector<string>& arr) {
  const int bits = arr[0].size();
  int win = static_cast<int>((arr.size() + 1) / 2);
  vector<int> most_arr;
  vector<int> least_arr;

  // bit 0
  int ones = 0;
  int i = 0;
  for (auto& s : arr) {
    char c = s[i];
    if (c == '1') ones += 1;
  }

  char most_dig = (ones >= win) ? '1' : '0';

  for (int j = 0; j < arr.size(); j++) {
    if (arr[j][i] == most_dig) {
      most_arr.push_back(j);
    } else {
      least_arr.push_back(j);
    }
  }

  // bit 1..bits-1
  for (int i = 1; i < bits; i++) {
    if (most_arr.size() == 1 && least_arr.size() == 1) break;
    least_arr = next(least_arr, arr, i, false);
    most_arr = next(most_arr, arr, i, true);
  }

  assert(most_arr.size() == 1);
  assert(least_arr.size() == 1);

  string most = arr[most_arr[0]];
  string least = arr[least_arr[0]];
  cout << "m = " << most << " . l = " << least << endl;
  return stoi(most, 0, 2) * stoi(least, 0, 2);
}

// v >= (N + 1) / 2
// odd: 2N+1 -> >= N + 1
// even: 2N -> => N
int solve(vector<string>& arr) {
  const int bits = arr[0].size();
  int win = static_cast<int>(arr.size() / 2);
  vector<int> count_ones(bits, 0);
  for (int i = 0; i < bits; i++) {
    for (auto& s : arr) {
      char c = s[i];
      if (c == '1') count_ones[i] += 1;
    }
  }

  int gamma = 0;
  int epsilon = 0;
  string g;
  string e;
  int mult = 1 << (bits - 1);
  for (int i = 0; i < bits; i++) {
    if (count_ones[i] == win && arr.size() % 2 == 0) {
      cout << "Handle ties" << endl;
      return -1;
    }
    if (count_ones[i] > win) {
      gamma += mult;
      g += "1";
      e += "0";
    } else {
      epsilon += mult;
      g += "0";
      e += "1";
    }
    mult >>= 1;
  }
  cout << "g = " << g << ". e = " << e << endl;
  return gamma * epsilon;
}

int main(void) {
  cout << "Problem 3" << endl;

  string bins;
  vector<string> arr;

  for (string line; getline(cin, line); ) {
    stringstream ss(line);
    ss >> bins;
    arr.push_back(bins);
  }

  cout << "Result: " << solve(arr) << endl;
  cout << "Result 2: " << solve2(arr) << endl;
}

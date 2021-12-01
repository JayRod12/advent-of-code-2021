#include <cassert>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

int solve(vector<int>& arr, int groupsize) {
  // count increases
  assert(groupsize <= arr.size());

  int count = 0;
  int acc = 0;
  for (int i = 0; i < arr.size(); i++) {
    if (i < groupsize) {
      acc += arr[i];
    } else {
      int prev = acc;
      int cur = acc - arr[i-groupsize] + arr[i];
      
      if (prev < cur) count++;

      // Interestingly without this line (which is a bug), the result is the same
      acc = cur;
    }
  }
  return count;
}

int main(void) {
  cout << "Problem 1" << endl;

  int val;
  vector<int> arr;

  for (string line; getline(cin, line); ) {
    stringstream ss(line);
    ss >> val;
    arr.push_back(val);
  }
  // Problem 1.0
  cout << "With window size = 1: " << solve(arr, 1) << endl;
  // Problem 1.5
  cout << "With window size = 3: " << solve(arr, 3) << endl;
}

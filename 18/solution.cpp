#include <algorithm>
#include <cassert>
#include <limits>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <utility>
#include <variant>
#include <deque>
#include <unordered_map>

using namespace std;

constexpr int kMaxDepth = 4;
constexpr int kMaxValue = 10;

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

struct SnailNum;
using Num = SnailNum;

using Null = std::monostate;
using Int = int;
using Pair = vector<SnailNum>;
using Data = variant<Null, Int, Pair>;

using searchfn = std::function<bool(int,Data&)>;
using ParentMap = std::unordered_map<Num*, Num*>;

void printPair(ostream& os, Pair& p);

struct SnailNum {
  Data data;
  SnailNum(Data&& data_) : data{std::move(data_)} {}

  friend ostream& operator<<(ostream& os, SnailNum& sn) {
    os << sn.data;
    return os;
  }

  friend ostream& operator<<(ostream& os, Data& data) {
    std::visit(overloaded {
        [&](Pair& p) { printPair(os, p); },
        [&](int i) { os << i; },
        [&](std::monostate) { os << "null"; },
    }, data);
    return os;
  }
};

void printPair(ostream& os, Pair& p) {
  os << "[";
  for (int i = 0; i < p.size(); i++) {
    os << p[i];
    if (i != p.size() - 1) {
      os << ',';
    }
  }
  os << "]";
}

bool operator==(const SnailNum& lhs, const SnailNum& rhs) {
  return lhs.data == rhs.data;
}

bool operator==(const Data& lhs, const Data& rhs) {
  if (lhs.index() != rhs.index()) {
    return false;
  }
  if (std::holds_alternative<int>(lhs)) {
    return std::get<int>(lhs) == std::get<int>(rhs);
  } else if (std::holds_alternative<Pair>(lhs)) {
    auto pl = std::get<Pair>(lhs);
    auto pr = std::get<Pair>(lhs);
    return pl[0] == pr[0] && pl[1] == pr[1];
  } else {
    return true;
  }
}

inline Num empty() {
  return Num(std::monostate());
}

inline Num p(int a, int b) {
  return Num(Pair({Num(a), Num(b)}));
}

inline Num p(Num a, Num b) {
  return Num(Pair({a, b}));
}

pair<Num, int> parseNum(const char* line, int idx) {
  char c = line[idx];
  if (c == '[') {
    auto [lnum, idx2] = parseNum(line, idx+1);
    assert(line[idx2] == ',');
    auto [rnum, idx3] = parseNum(line, idx2+1);
    assert(line[idx3] == ']');
    return {p(lnum, rnum), idx3+1};
  } else if ('0' <= c && c <= '9') {
    return {Num(static_cast<int>(c - '0')), idx + 1};
  } else {
    assert(false);
    return {Num(0), -1};
  }
}
/*
 *   []
 * |  \
 * |     \
 * |       \
 * |         \
 * |           \
 * |             \
 * |               \
 * []              []
 * | \             | \ 
 * [] [0,0]        [] [9,5]
 * | \             | \ 
 * 0  [4,5]     [4,5] [2,6]
*/

Num* left(Num* node) {
  return addressof(std::get<Pair>(node->data)[0]);
}

Num* right(Num* node) {
  return addressof(std::get<Pair>(node->data)[1]);
}

Num split(Num& root, Num* splitNode) {
  if (addressof(root) == splitNode) {
    int value = std::get<Int>(root.data);
    return p(Num(static_cast<int>(value / 2)), Num(static_cast<int>((value + 1) / 2)));
  } else if (std::holds_alternative<Int>(root.data)) {
    return root;
  } else {
    return p(
        split(*left(addressof(root)), splitNode),
        split(*right(addressof(root)), splitNode));
  }
}

Num explode(Num& root, Num* explodingNode, Num* leftNode, Num* rightNode, int lvalue, int rvalue) {
  if (addressof(root) == explodingNode) {
    return Num(0); // exploding node replaced by 0
  } else if (addressof(root) == leftNode) {
    return Num(std::get<Int>(root.data) + lvalue);
  } else if (addressof(root) == rightNode) {
    return Num(std::get<Int>(root.data) + rvalue);
  } else if (std::holds_alternative<Int>(root.data)) {
    return root;
  } else {
    return Num(
        Pair({
          explode(*left(addressof(root)), explodingNode, leftNode, rightNode, lvalue, rvalue), 
          explode(*right(addressof(root)), explodingNode, leftNode, rightNode, lvalue, rvalue)
        }));
  }
}

Num* rightMost(Num* node) {
  Num* curNode = node;
  while (std::holds_alternative<Pair>(curNode->data)) {
    curNode = right(curNode);
  }
  return curNode;
}

Num* leftMost(Num* node) {
  Num* curNode = node;
  while (std::holds_alternative<Pair>(curNode->data)) {
    curNode = left(curNode);
  }
  return curNode;
}

/*
 * Finds the number immediately to the left of searchNode (we have
 * to traverse the tree up and left, then find the right most number).
 *
 * @return: pointer to the node directly to the left
 */
Num* findLeft(Num& root, Num* searchNode, ParentMap& parents) {
  Num* curNode = searchNode;
  while (parents.contains(curNode) && curNode == left(parents.at(curNode))) {
    curNode = parents.at(curNode);
  }
  // curNode points to ancestor where this node isn't the left node
  // or to the root if there isn't such a node
  if (curNode == addressof(root)) {
    return nullptr;
  }
  
  return rightMost(left(parents.at(curNode)));
}

/* Like findLeft but swapping left and right */
Num* findRight(Num& root, Num* searchNode, ParentMap& parents) {
  Num* curNode = searchNode;
  while (parents.contains(curNode) && curNode == right(parents.at(curNode))) {
    curNode = parents.at(curNode); 
  }
  if (curNode == addressof(root)) {
    return nullptr;
  }
  
  return leftMost(right(parents.at(curNode)));
}

/* Search functions for dfs, looking for explode or split condition respectively */
auto depthfn = [](int depth, Data& data) {
  return std::holds_alternative<Pair>(data) && depth == kMaxDepth;
};

auto valuefn = [](int depth, Data& data) {
  return std::holds_alternative<Int>(data) && std::get<Int>(data) >= kMaxValue;
};

/*
 * Do depth first search and visit left nodes first. Search continues until the search function "searchfn"
 * evaluates to true at a given node and depth.
 *
 * @parents: map that gets filled in inside dfs with the visited nodes and parent-child relationship
 */
pair<bool, Num*> dfs(Num& node, int depth, searchfn fn, ParentMap& parents) {
  if (std::holds_alternative<Int>(node.data)) {
    return {fn(depth, node.data), addressof(node)};
  } else {
    assert(std::holds_alternative<Pair>(node.data));

    if (fn(depth, node.data)) {
      return {true, addressof(node)};
    }
    Pair& pair = std::get<Pair>(node.data);

    parents[addressof(pair[0])] = addressof(node);
    parents[addressof(pair[1])] = addressof(node);
    auto p = dfs(pair[0], depth+1, fn, parents);
    if (p.first) {
      return p;
    }
    return dfs(pair[1], depth+1, fn, parents);
  }
}

Num reduce(Num& n) {
  Num& result = n;
  ParentMap parents;

  pair<bool, Num*> res = dfs(result, 0, depthfn, parents);
  if (res.first) {
    // 4-deep pair found, explode it
    Num& search = *(res.second);
    assert (std::holds_alternative<Pair>(search.data));
    auto v = std::get<Pair>(search.data);
    int lvalue = std::get<Int>(v[0].data);
    int rvalue = std::get<Int>(v[1].data);
    auto left = findLeft(result, addressof(search), parents);
    auto right = findRight(result, addressof(search), parents);
    result = explode(result, addressof(search), left, right, lvalue, rvalue);

    return reduce(result);
  }

  res = dfs(result, 0, valuefn, parents);
  if (res.first) {
    Num& search = *(res.second);
    result = split(result, addressof(search));
    return reduce(result);
  }

  return result;
}

Num add(Num& a, Num& b) {
  Num unreduced = p(a, b);
  return reduce(unreduced);
}

Num addAll(vector<Num>& vec) {
  Num cur = vec[0];
  for (int i = 1; i < vec.size(); i++) {
    cur = add(cur, vec[i]);
  }
  return cur;
}

int magnitude(const Num& n) {
  return std::visit(overloaded {
      [](const Pair& p) { return 3*magnitude(p[0]) + 2*magnitude(p[1]); },
      [](const int i) { return i; },
      [](const std::monostate) { assert(false); return 0; },
  }, n.data);
}

void print(const char* name, Num n) {
  cout << name << " (" << addressof(n) << "): " << n << endl;
}

void test() {
  Num searchNode = p(4,5);
  Num left = Num(0);
  Num right = Num(0);
  Num other = Num(3);
  Num v = p(p(p(p(left,searchNode),p(right,other)),p(p(p(4,5),p(2,6)),p(9,5))),p(3,4));
  
  ParentMap parents;
  auto [b, search] = dfs(v, 0, depthfn, parents);
  assert(b);
  auto l = findLeft(v, search, parents);
  assert(l != nullptr);
  
  auto result = reduce(v);
  auto [expected, idx] = parseNum("[[[[4,0],[5,7]],[[7,7],[6,0]]],[[5,5],4]]", 0);
  assert(result == expected);
}

int main(void) {
  test();

  string line;
  vector<Num> input;
  while(getline(cin, line)) {
    auto [n, i] = parseNum(line.c_str(), 0);
    input.push_back(n);
  }
  
  cout << "Part 1 solution: " << std::flush;
  auto result = addAll(input);
  cout << magnitude(result) << endl;

  cout << "Part 2 solution (wait for " << (input.size() / 10) << " dots) [" << std::flush;
  int mag = std::numeric_limits<int>::min();
  for (int i = 0; i < input.size(); i++) {
    if (i % 10 == 0) {
      cout << "." << std::flush;
    }

    for (int j = i+1; j < input.size(); j++) {
      mag = max(
              mag,
              max(
                magnitude(add(input[i], input[j])),
                magnitude(add(input[j], input[i]))));
    }
  }
  cout  << "]: ";
  cout << mag << endl;
}

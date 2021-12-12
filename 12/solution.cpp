#include <cassert>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <string>
#include <vector>

using namespace std;


using VisitedSet = unordered_set<string>;

const string startNode = "start";
const string endNode = "end";

class Graph {
 public:
  Graph() : adj_{} {}

  ~Graph() = default;

  void addEdge(string& a, string& b) {
    if (a == endNode || b == startNode) {
      // don't add edge directed ->start or end->
      return;
    }
    if (!adj_.contains(a)) {
      vector<string> v;
      adj_[a] = v;
    }
    adj_[a].push_back(b);
  }

  const vector<string>& edges(const string& a) {
    if (!adj_.contains(a)) {
      return empty_;  
    } else {
      return adj_[a];
    }
  }

  const vector<string> nodes() {
    vector<string> v;
    for (auto& [a, b_] : adj_) {
      v.push_back(a);
    }
    return v;
  }

  void print(ostream& os) {
    os << "Print graph:" << endl;
    for (auto& [n, ns] : adj_) {
      os << n << ": ";
      for (auto& nn : ns) {
        os << nn << ",";
      }
      os << endl;
    }
  }
  
 private:
  const vector<string> empty_; 
  unordered_map<string, vector<string>> adj_;
};

bool isSmallCave(const string& cave) {
  return 'a' <= cave[0] && cave[0] <= 'z';
}

void printVec(vector<string>& v) {
  for (auto& p : v) {
    cout << p << ",";
  }
  cout << endl;
}

int search(
    Graph& g,
    const string& from,
    const string& to,
    VisitedSet& vis,
    vector<string>& parents,
    bool allowDoubleVisit) {

  if (from == to) {
    printVec(parents);
    return 1;
  }

  // Handle visited logic
  bool usedDoubleVisit = false;
  if (vis.contains(from)) {
    if (!allowDoubleVisit) {
      return 0;
    }
    usedDoubleVisit = true;
  }

  if (isSmallCave(from)) {
    vis.insert(from);
  }

  int paths = 0;

  for (auto& edge : g.edges(from)) {
    parents.push_back(edge);
    paths += search(g, edge, to, vis, parents, allowDoubleVisit && !usedDoubleVisit); 
    parents.pop_back();
  }

  // Do not backtrack here if this is the second visit, backtrack at
  // the first search entry point, otherwise we'll go in infinite loop
  // revisiting this node.
  if (!usedDoubleVisit) {
    vis.erase(from);
  }

  return paths;
}

int solve(Graph& g, bool allowDoubleVisit) {
  VisitedSet visited;
  vector<string> parents{startNode};
  return search(g, startNode, endNode, visited, parents, allowDoubleVisit);
}

int main(void) {
  Graph g;

  int v;

  string line;
  while (getline(cin, line)) {
    stringstream ss(line);
    string nodeA;
    getline(ss, nodeA, '-');
    string nodeB = line.substr(nodeA.size()+1);
    // cout << "Node : " << nodeA << " <-> " << nodeB << endl;
    g.addEdge(nodeA, nodeB);
    g.addEdge(nodeB, nodeA);
  }
  g.print(cout);

  cout << "Part 1 (no double visit)" << endl;
  cout << solve(g, false) << endl;

  cout << "Part 2 (double visit)" << endl;
  cout << solve(g, true) << endl;
}

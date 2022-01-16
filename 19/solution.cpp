#include <algorithm>
#include <cassert>
#include <limits>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <utility>
#include <unordered_map>
#include <unordered_set>
#include <tuple>
#include <optional>

#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

constexpr int kMinMatches = 12;

using Scan = Matrix<int, 3, Dynamic>;

struct hash_pair {
    template <class T1, class T2>
    size_t operator()(const pair<T1, T2>& p) const
    {
        auto hash1 = hash<T1>{}(p.first);
        auto hash2 = hash<T2>{}(p.second);
        return hash1 ^ hash2;
    }
};

// map<
//   key = (int, int):
//      Scans key[0]=>key[1] (i.e. i=>j) have a transform
//   value = ((Mat, vec), bool):
//      Mat = Rotation matrix
//      vec = Translation vector
//      bool = whether the transform is i=>j (true) or j=>i (false)
// >
//
using ScanTransformMap = unordered_map<pair<int, int>, pair<pair<Matrix3i, Vector3i>, bool>, hash_pair>;

vector<Matrix3i> getRotations() {
  vector<Matrix2i> submatrices;
  {
    Matrix2i a, b, c, d;
    a << 1, 0,
         0, 1;
    b = -1 * a;
    c <<  0, 1,
         -1, 0;

    d = -1*c;
    submatrices.push_back(a);
    submatrices.push_back(b);
    submatrices.push_back(c);
    submatrices.push_back(d);
  }

  vector<vector<int>> indexing{
    {1,2}, // x
    {0,2}, // y
    {0,1}, // z
  };

  int xIndex = 0;
  int yIndex = 1;
  int zIndex = 2;

  vector<Matrix3i> transform_matrices;
  //cout << "Transforms!:" << endl;
  for (auto& submat : submatrices) {
    // X rotation
    Matrix3i rx = Matrix3i::Zero();
    rx(xIndex, xIndex) = 1;
    for (int row = 0; row < 2; row++) {
      for (int col = 0; col < 2; col++) {
        rx(indexing[xIndex][row], indexing[xIndex][col]) = submat(row, col);
      }
    }
    for (auto& submat : submatrices) {
      // Y rotation
      Matrix3i ry = Matrix3i::Zero();
      ry(yIndex, yIndex) = 1;
      for (int row = 0; row < 2; row++) {
        for (int col = 0; col < 2; col++) {
          ry(indexing[yIndex][row], indexing[yIndex][col]) = submat(row, col);
        }
      }
      for (auto& submat : submatrices) {
        // Z rotation
        Matrix3i rz = Matrix3i::Zero();
        rz(zIndex, zIndex) = 1;
        for (int row = 0; row < 2; row++) {
          for (int col = 0; col < 2; col++) {
            rz(indexing[zIndex][row], indexing[zIndex][col]) = submat(row, col);
          }
        }
        auto m = rx*ry*rz;
        transform_matrices.push_back(m);
        //cout << m << endl;
        //cout << "--------" << endl;
      }
    }
    //cout << "---------------------------------" << endl;
  }

  cout << "Number rotations = " << transform_matrices.size() << endl;
  return transform_matrices;
}

namespace std {

  template <>
  struct hash<Vector3i>
  {
    std::size_t operator()(const Vector3i& v) const
    {
      using std::size_t;
      using std::hash;
      using std::string;

      // Compute individual hash values for first,
      // second and third and combine them using XOR
      // and bit shifting:

      return ((73856093 * hash<int>()(v[0])) ^ (19349663*hash<int>()(v[1]))) ^ (83492791*hash<int>()(v[2]));
    }
  };
}

vector<Vector3i> getTranslations(Scan s1, Scan s2) {
  vector<Vector3i> vec;
  for (int i = 0; i < s1.cols(); i++) {
    for (int j = 0; j < s2.cols(); j++) {
      Vector3i diff = s2.col(j) - s1.col(i);
      vec.push_back(diff);
    }
  }
  return vec;
}

int countMatches(Scan s1, Scan s2) {
  unordered_set<Vector3i> unique_cols_s2;
  for (int i = 0; i < s2.cols(); i++) {
    Vector3i v = s2.col(i);
    unique_cols_s2.insert(v);
  }
  int matchcount = 0;
  for (int i = 0; i < s1.cols(); i++) {
    Vector3i v = s1.col(i);
    if (unique_cols_s2.contains(v)) {
      matchcount++;
    }
  }

  return matchcount;
}

optional<pair<Matrix3i, Vector3i>> match(int i, int j, Scan& s1, Scan& s2, vector<Matrix3i>& rotations) {
  for (auto& rot : rotations) {
    Scan s1rot = rot*s1;
    for (auto& translation : getTranslations(s1rot, s2)) {
      Scan rotated = s1rot;
      rotated.colwise() += translation;
      int x = countMatches(rotated, s2);
      assert(x > 0);

      if (x >= kMinMatches) {
        cout << "Match count=" << x << " scans=" << i << "," << j  << " Rotation & Translation: " << endl;
        cout << rot << endl << translation << endl;
        return std::optional(make_pair(rot, translation));
      }
    }
  }
  return {};
}

// 0 -> 1
// 1 -> {3, 4]
// 2 -> 4
// (0,1) -> R,T
bool dfs(unordered_map<int, vector<int>>& graph, unordered_set<int>& visited, vector<int>& path, int from, int to) {
  if (from == to) {
    return true;
  }

  if (!graph.contains(from) || visited.contains(from)) {
    return false;
  }

  visited.insert(from);
  for (int v : graph[from]) {
    path.push_back(v);
    if (dfs(graph, visited, path, v, to)) {
      return true;
    }
    path.pop_back();
  }
  visited.erase(from);
  return false;
}

void insert(unordered_map<int, vector<int>>& graph, int i, int j) {
  if (!graph.contains(i)) {
    vector<int> vec;
    graph[i] = vec;
  }
  graph[i].push_back(j);
}

// Transform the probe vector `vecIn` via multiple transforms:
//  path[0]->path[1]->...path[end].
// Where each path[i] is a `scan` coordinate frame.
Vector3i transform(
    const Vector3i& vecIn,
    ScanTransformMap& scanTransforms,
    vector<int>& path) {
  Vector3i vec = vecIn;
  for (size_t i = 0; i < path.size() - 1; i++) {
    auto& [transform, isDirect] = scanTransforms[make_pair(path[i], path[i+1])];
    auto& [rot, trans] = transform;
    if (isDirect) {
      vec = rot*vec + trans;
    } else {
      vec = rot.transpose()*(vec - trans);
    }
  }
  return vec;
}

// Apply transform() to all probes and add to the `probesFromZeroPov`
void transformAll(
    Scan& scan,
    ScanTransformMap& scanTransforms,
    vector<int>& path,
    unordered_set<Vector3i>& probesFromZeroPov) {
  for (int i = 0; i < scan.cols(); i++) {
    probesFromZeroPov.insert(transform(scan.col(i), scanTransforms, path));
  }
}

// Complexity: O(nscan^2 * nrot * probes_per_scan^3) ~ 2.6*10^9 ~ 10min in debug mode ~ 15s in release mode
void solve(vector<Scan>& scanners) {
  int nScans = scanners.size();
  auto rotations = getRotations();
  ScanTransformMap scanTransforms;

  unordered_map<int, vector<int>> graph;
  // Find which scans can be converted to another via a rotation and translation
  for (int i = 0; i < nScans; i++) {
    for (int j = i + 1; j < nScans; j++) {
      auto opt = match(i, j, scanners[i], scanners[j], rotations);
      if (opt.has_value()) {
        scanTransforms[make_pair(i, j)] = make_pair(opt.value(), true);
        scanTransforms[make_pair(j, i)] = make_pair(opt.value(), false);

        insert(graph, i, j);
        insert(graph, j, i);
      }
    }
  }

  // Compute full set of probes, starting with those known from scan0
  unordered_set<Vector3i> probesFromZeroPov;
  for (int i = 0; i < scanners[0].cols(); i++) {
    probesFromZeroPov.insert(scanners[0].col(i));
  }
  // Compute all origins relative to zero
  vector<Vector3i> origins;
  origins.push_back({0, 0, 0}); // zero rel to zero

  // Find the combination of rotations/translations needed to convert each scan to the
  // 0'th scan coordinate space (by going through intermediate scan spaces).
  // Then convert each full scan to the 0'th frame of reference.

  for (int i = 1; i < nScans; i++) {
    unordered_set<int> vis;
    vector<int> path;
    path.push_back(i);
    bool has_path = dfs(graph, vis, path, i, 0);
    assert(has_path); // every scan should be transformable to 0'th space
    cout << "Path:" << endl;
    for (auto x : path) {
      cout << x << ",";
    }
    cout << endl;
    auto originRelativeToZero = transform({0, 0, 0}, scanTransforms, path);
    cout << "Relative position of " << i << " w.r.t Scan 0:" << endl
      << originRelativeToZero << endl;
    transformAll(scanners[i], scanTransforms, path, probesFromZeroPov);
    origins.push_back(originRelativeToZero);
  }
  int totalProbes = probesFromZeroPov.size();
  cout << "Total probe number: " << totalProbes << endl;

  int maxManhattanDistance = std::numeric_limits<int>::min();
  for (auto& oFrom : origins) {
    for (auto& oTo : origins) {
      int d = 0;
      d += (oFrom-oTo).cwiseAbs().sum();
      maxManhattanDistance = max(maxManhattanDistance, d);
    }
  }
  cout << "Max manhattan distance: " << maxManhattanDistance << endl;
}

Scan readScan(int number) {
  vector<string> asstr;
  string line;
  while (true) {
    getline(cin, line);
    if (line.empty()) {
      break;
    }
    asstr.push_back(line);
  }
  int x, y, z;
  char buf;
  Scan scan(3, asstr.size());
  for (size_t i = 0; i < asstr.size(); i++) {
    auto& s = asstr[i];
    stringstream ss(s); 
    ss >> x >> buf >> y >> buf >> z;
    Vector3i vec{x, y, z};
    scan.col(i) = vec; 
  }
  cout << "Scan " << number << endl;
  cout << scan;
  cout << endl;
  return scan;
}

int main(void) {
  vector<Scan> scanners;
  int i = 0;
  string line;
  while (getline(cin, line)) {
    scanners.push_back(readScan(i));
    i++;
  }

  solve(scanners);
}

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
#include <memory>
#include <functional>

using namespace std;

const char* hexChar2Bin(char c);
string hex2Bin(const string& hex);

constexpr int pVerLen = 3;
constexpr int pTypeIdLen = 3;

using ll = long long;

pair<int, int> getAsInt(const string& binStr, int startPos, int len) {
  string asStr(binStr.substr(startPos, len));
  int val = stoi(asStr, 0, 2);
  return make_pair(val, startPos+len);
}

pair<string, int> getAsStr(const string& binStr, int startPos, int len) {
  string asStr(binStr.substr(startPos, len));
  return make_pair(asStr, startPos+len);
}

class Packet {
 public:
  // Packet format:
  // 0-2: Version num
  // 3-5: Type ID
  // [ Packet Content ]
  
  enum class Type {
    OpSum, // 0
    OpProd, // 1
    OpMin, //2
    OpMax, //3
    LitVal, // 4
    OpGt, // 5
    OpLt, // 6
    OpEq, // 7
  };

  static pair<unique_ptr<Packet>, int> parse(const string& binStr, int startPos);

  static pair<pair<int, int>, int> parseHeader(const string& binStr, int startPos) {
    auto [v, pos] = getAsInt(binStr, startPos, 3);
    auto [t, pos2] = getAsInt(binStr, pos, 3);
    return make_pair(make_pair(v, t), pos2);
  }

  Packet(int version, int type) : version_{version}, type_{static_cast<Type>(type)}  {}
  virtual ~Packet() = default;
  virtual int getVersionTotal() = 0;
  virtual ll eval() = 0;

  virtual void print(ostream& os) const {
    os << "Packet: v=" << version_ << " , type=" << static_cast<int>(type_);
  }

  int version_;
  Type type_;
};

class LitPacket : public Packet {
 public:
  static pair<unique_ptr<LitPacket>, int> parse(const string& binStr, int startPos, int version, int type);

  LitPacket(int version, int type, ll value) : Packet(version, type), value_{value} {}
  ~LitPacket() override = default;

  int getVersionTotal() override {
    return version_;
  }

  ll eval() override {
    return value_;
  }

  void print(ostream& os) const override {
    Packet::print(os);
    os << " of type LitPacket: val=" << value_ << endl;
  }

  ll value_;
};

ll binApply(function<ll(ll, ll)> f, ll acc, vector<unique_ptr<Packet>>& subpackets_) {
  ll val = acc;
  for (auto& p : subpackets_) {
    val = f(val, p->eval());
  }
  return val;
}

ll compApply(function<ll(ll, ll)> f, vector<unique_ptr<Packet>>& subpackets_) {
  return f(subpackets_[0]->eval(), subpackets_[1]->eval());
}

ll minMaxApply(Packet::Type type, vector<unique_ptr<Packet>>& subpackets_) {
  ll val;
  if (type == Packet::Type::OpMin) {
    val = std::numeric_limits<ll>::max();
  } else {
    val = std::numeric_limits<ll>::min();
  }

  for (auto& p : subpackets_) {
    if (type == Packet::Type::OpMin) {
      val = std::min(val, p->eval());
    } else {
      val = std::max(val, p->eval());
    }
  }
  return val;
}

class OpPacket : public Packet {
 public:
  static pair<unique_ptr<OpPacket>, int> parse(const string& binStr, int startPos, int version, int type);

  OpPacket(int version, int type, vector<unique_ptr<Packet>> subpackets) : Packet(version, type), subpackets_{move(subpackets)} {}
  ~OpPacket() override = default;

  int getVersionTotal() override {
    int total = version_;
    for (auto& p : subpackets_) {
      total += p->getVersionTotal();
    }
    return total;
  }

  ll eval() override {
    switch (type_) {
      case Packet::Type::OpSum: 
        return binApply(std::plus<ll>(), 0, subpackets_);
      case Packet::Type::OpProd:
        return binApply(std::multiplies<ll>(), 1, subpackets_);
      case Packet::Type::OpMin:
        return minMaxApply(Packet::Type::OpMin, subpackets_);
      case Packet::Type::OpMax:
        return minMaxApply(Packet::Type::OpMax, subpackets_);
      case Packet::Type::LitVal:
        assert(false);
        return 0;
      case Packet::Type::OpGt:
        return compApply(std::greater<ll>(), subpackets_);
      case Packet::Type::OpLt:
        return compApply(std::less<ll>(), subpackets_);
      case Packet::Type::OpEq:
        return compApply(std::equal_to<ll>(), subpackets_);
    }
  }

  void print(ostream& os) const override {
    Packet::print(os);
    os << " of type OpPacket (" << subpackets_.size() << " subpackets): " << endl;
    for (auto& x : subpackets_) {
      x->print(os);
    }
  }

  vector<unique_ptr<Packet>> subpackets_;
};

ostream& operator<<(ostream& os, const Packet& p) {
  p.print(os);
  return os;
}

pair<unique_ptr<Packet>, int> Packet::parse(const string& binStr, int startPos) {
  auto [tup, pos] = Packet::parseHeader(binStr, startPos);
  auto [v, t] = tup;
  switch(t) {
    case 4:
      return LitPacket::parse(binStr, pos, v, t);
    default:
      return OpPacket::parse(binStr, pos, v, t);
  }
}

pair<unique_ptr<LitPacket>, int> LitPacket::parse(const string& binStr, int startPos, int version, int type) {
  bool keepReading = true;
  int pos = startPos;
  string litVal;
  while (keepReading) {
    keepReading = (binStr[pos] == '1');
    litVal += binStr.substr(pos+1, 4);
    pos += 5;
  }
  return make_pair(make_unique<LitPacket>(version, type, stoll(litVal, 0, 2)), pos);
}

pair<unique_ptr<OpPacket>, int> OpPacket::parse(const string& binStr, int startPos, int version, int type) {
  char lengthTypeId = binStr[startPos];
  int curPos = startPos + 1;
  vector<unique_ptr<Packet>> vec;
  if (lengthTypeId == '0') {
    // 15 bit total len
    auto [len, pos] = getAsInt(binStr, curPos, 15);
    curPos = pos;
    int finalPos = curPos+len;

    // parse sub packets up to len bits
    while(curPos < finalPos) {
      auto tup = Packet::parse(binStr, curPos);
      curPos = tup.second;
      vec.push_back(move(tup.first));
    }
  } else {
    auto [nSubPackets, pos] = getAsInt(binStr, startPos+1, 11);
    curPos = pos;
    for (int i = 0; i < nSubPackets; i++) {
      auto tup = Packet::parse(binStr, curPos);
      curPos = tup.second;
      vec.push_back(move(tup.first));
    }
  }
  return make_pair(make_unique<OpPacket>(version, type, move(vec)), curPos);
}

ll solve(string hexStr) {
  string binStr = hex2Bin(hexStr);

  auto [p, pos] = Packet::parse(binStr, 0);
  cout << ">Parsed<" << endl;
  cout << *p << endl;
  cout << "Remainder string: " << binStr.substr(pos) << endl;
  cout << "Part 1 Ans: Total version addition is: " << p->getVersionTotal() << endl;
  cout << "Part 2 Ans: eval(): " << p->eval() << endl;

  return p->eval();
}

int main(void) {

  string line;
  getline(cin, line);

  cout << solve(line) << endl;
}

const char* hexChar2Bin(char c) {
    switch(toupper(c))
    {
        case '0': return "0000";
        case '1': return "0001";
        case '2': return "0010";
        case '3': return "0011";
        case '4': return "0100";
        case '5': return "0101";
        case '6': return "0110";
        case '7': return "0111";
        case '8': return "1000";
        case '9': return "1001";
        case 'A': return "1010";
        case 'B': return "1011";
        case 'C': return "1100";
        case 'D': return "1101";
        case 'E': return "1110";
        case 'F': return "1111";
        default: return "XXXX";
    }
}

string hex2Bin(const string& hex) {
    string bin;
    for (const char c : hex) {
      bin += hexChar2Bin(c);
    }
    return bin;
}

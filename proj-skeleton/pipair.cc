#include <string>
#include <iostream>
#include <istream>
#include <ostream>
#include <iterator>
#include <cstdlib>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <map>
#include <set>
#include <utility>

using namespace std;

typedef map<long, set<long> > CallGraph;
typedef map<long, int> SupportTable;
typedef map<set<long,long>, int> SupportPairTable;

static SupportTable supportTable;
static SupportPairTable supportPairTable;
static map<string, long> id_map;
static map<long, string> reverse_id_map;
static map<long, int> support_map;

string toString(long id) {
  if(reverse_id_map.count(id)) {
    return reverse_id_map[id];
  } else {
    return "";
  }
}

int toId(string s) {
  static int count = 0;
  if (id_map.count(s)) {
    return id_map[s];
  }
  id_map[s] = count;
  reverse_id_map[count] = s;
  count++;
}


void trim(string & s) {
  size_t first = s.find_first_not_of(' ');
  size_t last = s.find_last_not_of(' ');
  s = s.substr(first, (last-first+1));
}

vector<string> &split(const string &s, char delim, vector<string> &elems) {
  stringstream ss(s);
  string item;
  while (std::getline(ss, item, delim)) {
    if (item != "") {
      elems.push_back(item);
    }
  }
  return elems;
}

vector<string> split(const string &s, char delim) {
  vector<string> elems;
  split(s, delim, elems);
  return elems;
}

void extractQuotes(string& s) {
  size_t first = s.find_first_of('\'');
  size_t last = s.find_last_of('\'');
  s = s.substr(first+1, last-1);
}

void calculateSupport(CallGraph graph) {
  // calculate support for the call graph
}

int main (int argc, char* argv[]) {
  int support = atoi(argv[1]);
  int confidence = atoi(argv[2]);

  string line;
  CallGraph callGraph;
  string node = "";
  
  while (getline(cin, line)) { // string parsing
    if(line == "") {
      continue;
    }
    trim(line);
    vector<string> tokens = split(line, ' ');
    if (tokens.size() == 7) {
      node = tokens[5];
      extractQuotes(node);
      cout << node << endl;
    } else if (tokens.size() == 4) {
        if(tokens[2] == "external" && tokens[3] == "node") {
          continue;
        }
        if (tokens[1] == "Root") {
          continue;
        }
        if (node == "") {
          continue;
        }
        string leaf = tokens[3];
        extractQuotes(leaf);
        cout << "\t" << leaf << endl;
        callGraph[toId(node)].insert(toId(leaf));
    }
  }
  calculateSupport(callGraph);
  return 0;
}



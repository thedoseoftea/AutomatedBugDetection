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
typedef map<pair<long,long>, int> SupportPairTable;

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
  static long count = 0;
  if (id_map.count(s)) {
    return id_map[s];
  }
  id_map[s] = count;
  reverse_id_map[count] = s;
  return count++;
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
  s = s.substr(first+1, last-first-1);
}

void calculateSupport(set<long> scope) {
  for(set<long>::iterator it = scope.begin();it!=scope.end();++it) {
    supportTable[*it]++;
  }
}

void calculatePairSupport(set<long> scope) {
  for(set<long>::iterator i = scope.begin();i!=scope.end();++i) {
    long first = *i;
    for(set<long>::iterator j = i; j!=scope.end();++j) {
      long second = *j;
      if (first != second) {
        pair<long,long> p;
        if (first < second) {
          p = make_pair(first, second);
        } else {
          p = make_pair(second, first);
        }
        supportPairTable[p]++;
      }
    }
  }
}

void printBug(int existing, int scope, pair<long,long> p, int support, double confidence) {
  cout << "bug: " << toString(existing)
    <<" in " << toString(scope)
    << ", pair: " << "(" << toString(p.first) << ", " << toString(p.second) << "), "
    << "support: " << support <<", "
    << "confidence: " << confidence << endl;
    // bug: %s in %s, pair: (%s, %s), support: %d, confidence: %.2f%%\n
}

void findBug(CallGraph & graph, long existing, long missing, double pairSupport, double bugConfidence, double confidence) {
  if(bugConfidence >= confidence) {
    for(CallGraph::iterator it  = graph.begin();it!=graph.end();++it) {
      set<long> scopeMethods = it->second;
      if (find(scopeMethods.begin(), scopeMethods.end(), existing) != scopeMethods.end() &&
          find(scopeMethods.begin(), scopeMethods.end(), missing) == scopeMethods.end()) {
        printBug(existing, it->first, make_pair(existing, missing), pairSupport, bugConfidence);
      }
    }
  }
}

void findBugs(CallGraph & graph, int support, int confidence) {
  for(SupportPairTable::iterator it = supportPairTable.begin(); it!= supportPairTable.end(); ++it) {
    pair<long, long> p = it->first;
    int pairSupport = it->second;
    if (pairSupport >= support) {
      long first = p.first;
      long second = p.second;
      int firstSupport = supportTable[first];
      int secondSupport = supportTable[second];
      findBug(graph, first, second, pairSupport, (double)pairSupport/firstSupport, (double)confidence/100);
      findBug(graph, second, first, pairSupport, (double)pairSupport/secondSupport, (double)confidence/100);
    }
  }
}

void processGraph(CallGraph & graph, int support, int confidence) {
  for(CallGraph::iterator it = graph.begin(); it!=graph.end(); ++it) {
    calculateSupport(it->second); 
    calculatePairSupport(it->second);
  }
  findBugs(graph, support, confidence); 
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
        callGraph[toId(node)].insert(toId(leaf));
    }
  }
  cout << "after parsing" << endl;
  processGraph(callGraph, support, confidence);
  return 0;
}



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
#include <stdio.h>

using namespace std;

// map of scope => methods in scope
typedef map<long, set<long> > CallGraph;

// support for each individual method in the call graph
typedef map<long, int> SupportTable;

// support for each pair method in the call graph
typedef map<pair<long,long>, int> SupportPairTable;

static SupportTable supportTable;
static SupportPairTable supportPairTable;

//  string (function name) <=> integer (id)
static map<string, long> id_map;
static map<long, string> reverse_id_map;

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

// get string between single quotes - eg. 'se465' => se465
void extractQuotes(string& s) {
  size_t first = s.find_first_of('\'');
  size_t last = s.find_last_of('\'');
  s = s.substr(first+1, last-first-1);
}

// calculate support for each method in a scope
void calculateSupport(set<long> scope) {
  for(set<long>::iterator it = scope.begin();it!=scope.end();++it) {
    supportTable[*it]++;
  }
}

// calculate suport for each pair in a scope
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

// print bug in format specified in the assignment
void printBug(int existing, int scope, pair<long,long> p, int support, double confidence) {
    string first = toString(p.first);
    string second = toString(p.second);

    // sort pair alphabetically for output
    if (first > second) {
      string temp = first;
      first = second;
      second = temp;
    }

    printf("bug: %s in %s, pair: (%s, %s), support: %d, confidence: %.2f%%\n", 
        toString(existing).c_str(),
        toString(scope).c_str(),
        first.c_str(),
        second.c_str(),
        support,
        confidence*100);
}

// iterate though each scope, a bug occurs when "existing" is found in the scope
// and "missing" is not found in the scope and the confidence is above a certain threshold
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

// iterate through each pair, we further process pairs that has support above the threshold
void findBugs(CallGraph & graph, int support, int confidence) {
  for(SupportPairTable::iterator it = supportPairTable.begin(); it!= supportPairTable.end(); ++it) {
    pair<long, long> p = it->first;
    int pairSupport = it->second;
    if (pairSupport >= support) {
      long first = p.first;
      long second = p.second;
      int firstSupport = supportTable[first];
      int secondSupport = supportTable[second];
      // calculate confidence for (pair, first_element) and (pair, second_element) and check for bugs
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
  
  // parse callGraph text output
  while (getline(cin, line)) {
    if(line == "") {
      continue;
    }
    trim(line);
    vector<string> tokens = split(line, ' ');
    // this is a SCOPE
    if (tokens.size() == 7) {
      node = tokens[5];
      extractQuotes(node);
    // this is a METHOD in the above scope
    } else if (tokens.size() == 4) {

        // ignore external nodes
        if(tokens[2] == "external" && tokens[3] == "node") {
          continue;
        }
        // ignore root scope
        if (tokens[1] == "Root") {
          continue;
        }
        // ignore root scope's methods
        if (node == "") {
          continue;
        }
        string leaf = tokens[3];
        extractQuotes(leaf);
        // make hashTable representation of callGraph: scope => {methods in scope}
        callGraph[toId(node)].insert(toId(leaf));
    }
  }
  processGraph(callGraph, support, confidence);
  return 0;
}



#include <string>
#include <iostream>
#include <istream>
#include <ostream>
#include <iterator>
#include <cstdlib>

using namespace std;



int main (int argc, char* argv[])
{


  int support = atoi(argv[1]);
  int confidence = atoi(argv[2]);

  string line;
  cout<<"lol"<<endl;
  while (getline(cin, line))
  {
    //do something interesting    
    cout << line << endl;
  }
  return 0;
}
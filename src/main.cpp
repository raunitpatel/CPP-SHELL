#include <iostream>
#include <string>
using namespace std;

int main() {
  // Flush after every std::cout / std:cerr
  cout << unitbuf;
  cerr << unitbuf;

  // Uncomment this block to pass the first stage
  cout<<"$ ";

  string input;
  getline(std::cin, input);
  cout<<input<<": command not found"<<endl;
}

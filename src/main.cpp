#include <iostream>
#include <string>
using namespace std;

int main() {
  // Flush after every std::cout / std:cerr
  while(1){
    cout << unitbuf;
    cerr << unitbuf;

    // Uncomment this block to pass the first stage
    cout<<"$ ";

    string input;
    getline(cin, input);
    if(input == "exit 0"){
      break;
    }
    else{
      cout<<input<<": command not found"<<endl;
    }
  }
  return 0;
}

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
    else if(input.find("echo") != string::npos){
      string temp="";
      bool flag=false;
      for(auto x:input){
        if(x==' ' and !flag){
          flag=true;
        }
        else if(flag){
          temp+=x;
        }
      }
      cout<<temp<<endl;
    }
    else{
      cout<<input<<": command not found"<<endl;
    }
  }
  return 0;
}

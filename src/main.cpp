#include <iostream>
#include <string>
#include <vector>
#include <set>
using namespace std;

pair<string,string> break_command(string command){
  string first_word="";
  string rem_words="";
  bool flag=false;
  for(auto x:command){
    if(x==' ' and !flag){
      flag=true;
    }
    else if(flag){
      rem_words+=x;
    }
    else{
      first_word+=x;
    }
  }
  return {first_word, rem_words};

}


int main() {
  // Flush after every std::cout / std:cerr
  set<string> commands = {"exit", "echo", "type"};
  while(1){
    cout << unitbuf;
    cerr << unitbuf;

    // Uncomment this block to pass the first stage
    cout<<"$ ";

    string input;
    getline(cin, input);

    auto [command_name, rem_command] = break_command(input);
    
    if(commands.find(command_name) == commands.end()){
      cout<<input<<": command not found"<<endl;
      continue;
    }

    if(command_name == "exit"){
      if(rem_command == "0"){
        break;
      }
    }
    else if(command_name == "echo"){
      cout<<rem_command<<endl;
    }
    else if(command_name == "type"){
      if(commands.find(rem_command) != commands.end()){
        cout<<rem_command<<" is a shell builtin"<<endl;
      }
      else{
        cout<<rem_command<< ": not found"<<endl;
      }
    }
  }
  return 0;
}

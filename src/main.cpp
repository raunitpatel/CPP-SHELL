#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <filesystem>
#include <cstdlib>
#include <unistd.h>  

using namespace std;
namespace fs = std::filesystem;



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

vector<string> get_folders_paths(string str){
  vector<string> ans;
  string folder="";
  for(auto x:str){
    if(x==':'){
      ans.push_back(folder);
      folder="";
    }
    else{
      folder+=x;
    }
  }
  if (!folder.empty()) {
      ans.push_back(folder); 
  }
  return ans;
}


int main() {
  // Flush after every std::cout / std:cerr
  const char *path_env = std::getenv("PATH");
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

      vector<string> folders_paths=get_folders_paths(path_env);
      
      if(commands.find(rem_command) != commands.end()){
        cout<<rem_command<<" is a shell builtin"<<endl;
      }
      else{
        string target_filename = rem_command;
        bool flag=false;

        for(auto dir:folders_paths){
          if(fs::exists(dir) == false) continue;

          fs::path filepath = fs::path(dir) / target_filename;
          if (fs::exists(filepath) && access(filepath.c_str(), X_OK) == 0) {
            cout<<target_filename<<" is "<<filepath.string()<<endl;
            flag=true;
            break;
          }
        }

        if(!flag) cout<<rem_command<< ": not found"<<endl;
      }
    }
  }
  return 0;
}

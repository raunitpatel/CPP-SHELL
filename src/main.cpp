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

string get_executable_path(string target_filename){
  string path_env = "";
  path_env = getenv("PATH");
  vector<string> folders;
  string folder="";
  string ans="";
  for(auto x:path_env){
    if(x==':'){
      folders.push_back(folder);
      folder="";
    }
    else{
      folder+=x;
    }
  }
  if (!folder.empty()) {
      folders.push_back(folder); 
  }

  for(auto dir:folders){
    if(fs::exists(dir) == false) continue;

    fs::path filepath = fs::path(dir) / target_filename;
    if (fs::exists(filepath) && access(filepath.c_str(), X_OK) == 0) {
      ans = filepath.string();
      return ans;
    }
  }
  return ans;
}


int main() {
  // Flush after every std::cout / std:cerr
  
  set<string> commands = {"exit", "echo", "type", "pwd", "cd"};
  fs::path curr_dir = fs::current_path();


  while(1){
    cout << unitbuf;
    cerr << unitbuf;

    // Uncomment this block to pass the first stage
    cout<<"$ ";

    string input;
    getline(cin, input);

    auto [command_name, rem_command] = break_command(input);

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
        string filepath=get_executable_path(rem_command);
        if(filepath != ""){
          cout<<rem_command<<" is "<< filepath<<endl;
        }
        else cout<<rem_command<< ": not found"<<endl;
      }
    }
    else if(get_executable_path(command_name) != ""){
      string filepath = get_executable_path(command_name);
      system(input.c_str());
    }
    else if(command_name == "pwd"){
      cout<<curr_dir.string()<<endl;
    }
    else if(command_name == "cd"){
      fs::path abs_dir_path = rem_command;
      if(fs::exists(abs_dir_path) and fs::is_directory(abs_dir_path)){
        fs::current_path(abs_dir_path);
        curr_dir = fs::current_path();
      }
      else{
        cout << "cd: " << abs_dir_path.string() << ": No such file or directory"<<endl;
      }

    }
    else{
      cout<<input<<": command not found"<<endl;
    }
  }
  return 0;
}

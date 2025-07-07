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
  string PATH_ENV = "";
  PATH_ENV = getenv("PATH");
  vector<string> folders;
  string folder="";
  string ans="";
  for(auto x:PATH_ENV){
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
      string output="";
      bool flag=false;
      int len= rem_command.size();
      if(rem_command.find('\"') == string::npos){
        for(int i=0;i<len;i++){
          if(rem_command[i]=='\''){
            flag=!flag;
          }
          else if(!flag and i!=0 and rem_command[i]==' ' and rem_command[i-1]==' '){
            continue;
          }
          else output+=rem_command[i];
        }
      }
      else{
        for(int i=0;i<len;i++){
          if(rem_command[i]=='\"'){
            flag=!flag;
          }
          else if(!flag and i!=0 and rem_command[i]==' ' and rem_command[i-1]==' '){
            continue;
          }
          else output+=rem_command[i];
        }
      }
      
      cout<<output<<endl;
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
      string HOME_ENV = getenv("HOME");
      fs::path abs_dir_path = rem_command;
      if(rem_command != "" and rem_command[0] == '~'){
        if(HOME_ENV != ""){
          string suffix = rem_command.substr(1);
          abs_dir_path = HOME_ENV + suffix;
        }
      }
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

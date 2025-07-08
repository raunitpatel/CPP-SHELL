#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <filesystem>
#include <cstdlib>
#include <unistd.h> 
#include <algorithm> 
#include <fstream>

using namespace std;
namespace fs = std::filesystem;
vector<string> get_cleaned_text(string rem_command) {
  vector<string> tokens;
  string output = "";

  bool is_single_quote = false, is_double_quote = false;
  bool escaped = false;

  int len = rem_command.size();
  for (int i = 0; i < len; ++i) {
    char ch = rem_command[i];

    if (escaped) {
      output += ch;
      escaped = false;
      continue;
    }

    if (ch == '\\') {
      if (is_single_quote) {
        output += ch;
      } 
      else if (is_double_quote) {
        if (i + 1 < len) {
          char next_char = rem_command[i + 1];
          if (next_char == '\\' || next_char == '"' || next_char == '$' || next_char == '\n') {
            escaped = true; 
            continue;
          }
           else {
            output += ch;
          }
        } 
        else {
          output += ch;
        }
      } 
      else {
        escaped = true;
        continue;
      }
    } 
    else if (ch == '"' && !is_single_quote) {
      is_double_quote = !is_double_quote;
    } 
    else if (ch == '\'' && !is_double_quote) {
      is_single_quote = !is_single_quote;
    } 
    else if (ch == ' ' && !is_single_quote && !is_double_quote) {
      if(!output.empty()){
        tokens.push_back(output);
        output.clear();
      }
    } else {
      output += ch;
    }
  }

  if (!output.empty()) {
    tokens.push_back(output);
  }

  return tokens;
}

string escapeShellArg(string arg)
{
  if (arg.find_first_of(" \t\n'\"\\") == string::npos)
  {
    return arg;
  }

  if (arg.find('\'') != string::npos)
  {
    return "\"" + arg + "\"";
  }

  return "'" + arg + "'";
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
    vector<string> tokens = get_cleaned_text(input);
    string command_name = tokens[0];
    vector<string> args;
    for(int i=1;i<tokens.size();i++){
      args.push_back(tokens[i]);
    } 

    if(command_name == "exit" ){
      if(args.size() > 0 and args[0] == "0"){
        break;
      }
    }

    else if(command_name == "echo"){

      bool has_stdout_redirect = false;
      bool has_stderr_redirect = false;
      bool has_stdout_append_redirect = false;
      bool has_stderr_append_redirect = false;

      string stdout_file = "";
      string stderr_file = "";

      string content = "";

      for(int i=0;i<args.size();i++){
        if((args[i] == "1>" or args[i] == ">") and i+1 < args.size()){
          stdout_file = args[i+1];
          has_stdout_redirect = true;
          i++;
        }
        else if((args[i] == "1>>" or args[i] == ">>") and i+1 < args.size()){
          stdout_file = args[i+1];
          has_stdout_append_redirect = true;
          i++;
        }
        else if(args[i] == "2>"  and i+1 < args.size()){
          stderr_file = args[i+1];
          has_stderr_redirect = true;
          i++;
        }
        else if(args[i] == "2>>" and i+1 < args.size()){
          stderr_file = args[i+1];
          has_stderr_append_redirect = true;
          i++;
        }
        else{
          content += args[i] + " ";
        }
      }

      if(!content.empty()){
        content.pop_back();
      }
      if(has_stdout_append_redirect){
        ofstream out(stdout_file, ios::app); // append
        if (out.is_open()) {
            out << content << endl;
            out.close();
        }
      }
      else if(has_stdout_redirect){
        ofstream out(stdout_file);
        if(out.is_open()){
          out<<content<<endl;
          out.close();
        }
      }
      else{
        std::cout << content << std::endl;
      }

      if(has_stderr_redirect){
        ofstream err(stderr_file);
        if(err.is_open()){
          err.close();
        }
      }
    }

    else if(command_name == "type"){
      if(args.size() > 0 and commands.find(args[0]) != commands.end()){
        cout<<args[0]<<" is a shell builtin"<<endl;
      }
      else if(args.size() > 0 ){
        string filepath=get_executable_path(args[0]);
        if(filepath != ""){
          cout<<args[0]<<" is "<< filepath<<endl;
        }
        else cout<<args[0]<< ": not found"<<endl;
      }
    }

    else if(get_executable_path(command_name) != ""){
      string fullCommand = escapeShellArg(command_name);
      for(auto arg:args){
        fullCommand += " " + escapeShellArg(arg);
      }
      system(fullCommand.c_str());
    }

    else if(command_name == "pwd"){
      cout<<curr_dir.string()<<endl;
    }

    else if(command_name == "cd"){
      string HOME_ENV = getenv("HOME");
      string path_to_go;
      if(args.empty()){
          if(HOME_ENV != ""){
            path_to_go = HOME_ENV;
          }
          else{
            path_to_go = "";
          }
              
      } 
      else{
          path_to_go = args[0];
      }
      if (!args.empty() && !path_to_go.empty() && path_to_go[0] == '~') {
        std::string suffix = path_to_go.substr(1);
        if(!HOME_ENV.empty()){
            path_to_go = std::string(HOME_ENV) + suffix;
        }
      }
      fs::path abs_dir_path(path_to_go);
      if (fs::exists(abs_dir_path) && fs::is_directory(abs_dir_path)) {
          fs::current_path(abs_dir_path);
          curr_dir = fs::current_path();
      } else {
          std::cout << "cd: " << abs_dir_path.string() << ": No such file or directory" << std::endl;
      }

    }
    
    else{
      cout<<input<<": command not found"<<endl;
    }
  }
  return 0;
}

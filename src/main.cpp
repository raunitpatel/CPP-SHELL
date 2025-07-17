#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <readline/readline.h>
#include <readline/history.h>
#include <filesystem>
#include <sys/wait.h>
#include <cstdlib>
#include <unistd.h> 
#include <algorithm> 
#include <fstream>

using namespace std;
namespace fs = std::filesystem;

set<string> commands = {"exit", "echo", "type", "pwd", "cd", "history"};

char* command_generator(const char* text, int state) {
  static vector<string> matches;
  static size_t match_index;

  if (state == 0) {
    // Reset for a new completion attempt
    matches.clear();
    match_index = 0;

    set<string> all_candidates;

    // 1. Add built-in commands
    for (const auto& cmd : commands) {
      all_candidates.insert(cmd);
    }

    // 2. Add external commands from PATH
    string PATH_ENV = "";
    PATH_ENV = getenv("PATH");
    if(PATH_ENV != " "){
      vector<string> folders;
      string folder="";
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

        for(auto file : fs::directory_iterator(dir)){
          if (file.is_regular_file() && access(file.path().c_str(), X_OK) == 0) {
            all_candidates.insert(file.path().filename().string());
          }
        }
      }
    }

    for(auto candidate:all_candidates){
      if(candidate.find(text) == 0){
        matches.push_back(candidate);
      }
    }
  }

  if(match_index < matches.size()){
    return strdup(matches[match_index++].c_str());
  } 
  else{
    return nullptr;
  }
}


char** command_completion(const char* text, int start, int end) {
  // rl_attempted_completion_over = 1; // prevent readline from filename completion
  return rl_completion_matches(text, command_generator);
}

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

void execute_pipeline(vector<vector<string>> &pipe_commands){
  int num_cmds = pipe_commands.size();
  int pipe_fds[2];
  int prev_fd = -1;
  for(int i=0;i<num_cmds;i++){
    if (i < num_cmds - 1 && pipe(pipe_fds) == -1) {
      perror("pipe");
      return;
    }
    pid_t pid = fork();
    if (pid < 0) {
      perror("fork");
      return;
    }
    if(pid == 0){

      if(i> 0){
        dup2(prev_fd, STDIN_FILENO);
        close(prev_fd);
      }
      if(i < (num_cmds-1)){
        close(pipe_fds[0]);
        dup2(pipe_fds[1], STDOUT_FILENO);
        close(pipe_fds[1]);
      }

      vector<string>& tokens = pipe_commands[i];
      string command_name = tokens[0];
      vector<string> args(tokens.begin() + 1, tokens.end());

      if(command_name == "echo"){

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
        exit(0);
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
        exit(0);
      }
      else if(get_executable_path(command_name) != ""){
        string fullCommand = escapeShellArg(command_name);
        for(auto arg:args){
          fullCommand += " " + escapeShellArg(arg);
        }
        system(fullCommand.c_str());
        exit(0);
      }
      else{
        string input = "";
        for(auto x: pipe_commands[i]){
          input+=x;
          input+=' ';
        }
        if(!input.empty()) input.pop_back();
        cout<<input<<": command not found"<<endl;
        exit(127);
      }
    }
    else{
      if(prev_fd != -1) close(prev_fd);
      if(i<num_cmds -1){
        close(pipe_fds[1]);
        prev_fd = pipe_fds[0];
      }
    }
  }
  while(wait(nullptr) > 0 );
}

class History{
  private:
    vector<string> historic_commands;
    int curr_pos;
    int sz;
  public:
    History(){
      curr_pos = 0;
      sz = 0;
    }
    void put_commands_in_history(string command){
      historic_commands.push_back(command);
      sz++;

      return;
    }
    void print_history(){
      for(int i=0;i<sz;i++){
        cout<<i+1<<" "<<historic_commands[i]<<endl;
      }
      return;
    }
};

int main() {
  // Flush after every std::cout / std:cerr
  
  rl_attempted_completion_function = command_completion;
  
  fs::path curr_dir = fs::current_path();

  History cmd_history;


  while(1){
    cout << unitbuf;
    cerr << unitbuf;

    // Uncomment this block to pass the first stage
    char* input_cstr = readline("$ ");
    string input = input_cstr;
    free(input_cstr);
    vector<string> tokens = get_cleaned_text(input);
    if(tokens.empty()){
      continue;
    }
    
    cmd_history.put_commands_in_history(input);
    
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

    else if(command_name == "pwd"){
      cout<<curr_dir.string()<<endl;
      continue;
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
      
      if(!args.empty() && !path_to_go.empty() && path_to_go[0] == '~'){
        std::string suffix = path_to_go.substr(1);
        if(!HOME_ENV.empty()){
            path_to_go = std::string(HOME_ENV) + suffix;
        }
      }

      fs::path abs_dir_path(path_to_go);
      if(fs::exists(abs_dir_path) && fs::is_directory(abs_dir_path)){
          fs::current_path(abs_dir_path);
          curr_dir = fs::current_path();
      }
      else{
          std::cout << "cd: " << abs_dir_path.string() << ": No such file or directory" << std::endl;
      }
      continue;
    }
    else if(command_name == "history"){
      cmd_history.print_history();
      continue;
    }

    vector<vector<string>> pipe_commands;
    vector<string> temp_tokens;
    for(auto token:tokens){
      if(token == "|"){
        pipe_commands.push_back(temp_tokens);
        temp_tokens.clear();
      }
      else{
        temp_tokens.push_back(token);
      }
    }
    if(!temp_tokens.empty())pipe_commands.push_back(temp_tokens);

    execute_pipeline(pipe_commands);
    
    
    
  }
  return 0;
}

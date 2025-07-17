#ifndef SHELL_H
#define SHELL_H

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

// Global variables
extern set<string> commands;

// Function declarations
vector<string> get_cleaned_text(string rem_command);
string escapeShellArg(string arg);
string get_executable_path(string target_filename);
void execute_pipeline(vector<vector<string>> &pipe_commands);

// Completion system functions
char* command_generator(const char* text, int state);
char** command_completion(const char* text, int start, int end);

// History class declaration
class History {
private:
    vector<string> historic_commands;
    int curr_pos;
    int sz;
    int last_appended_index;

public:
    History();
    void write_history_to_file(string filename);
    void append_history_to_file(string filename);
    void append_commands_to_history_from_file(string filename);
    void put_commands_in_history(string command);
    void print_history(int x);
};

#endif // SHELL_H
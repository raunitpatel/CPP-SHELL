#include "shell.h"
#include <readline/readline.h>
#include <readline/history.h>
#include <cstdlib>
#include <unistd.h>

// Global variable definition
set<string> commands = {"exit", "echo", "type", "pwd", "cd", "history"};

int main() {
    // Flush after every std::cout / std:cerr
    cout << unitbuf;
    cerr << unitbuf;

    rl_attempted_completion_function = command_completion;

    fs::path curr_dir = fs::current_path();
    History cmd_history;

    string HISTFILE_ENV = "";
    const char* histfile_env = getenv("HISTFILE");
    if (histfile_env != nullptr) {
        HISTFILE_ENV = histfile_env;
        cmd_history.append_commands_to_history_from_file(HISTFILE_ENV);
    }

    while (1) {
        char* input_cstr = readline("$ ");
        if (input_cstr == nullptr) {
            // Handle EOF (Ctrl+D)
            break;
        }

        string input = input_cstr;
        free(input_cstr);

        vector<string> tokens = get_cleaned_text(input);
        if (tokens.empty()) {
            continue;
        }

        cmd_history.put_commands_in_history(input);

        string command_name = tokens[0];
        vector<string> args;
        for (int i = 1; i < tokens.size(); i++) {
            args.push_back(tokens[i]);
        }

        if (command_name == "exit") {
            if (args.size() > 0 and args[0] == "0") {
                if (histfile_env != nullptr) {
                    HISTFILE_ENV = histfile_env;
                    cmd_history.write_history_to_file(HISTFILE_ENV);
                }
                break;
            }
        } else if (command_name == "pwd") {
            cout << curr_dir.string() << endl;
            continue;
        } else if (command_name == "cd") {
            const char* home_env = getenv("HOME");
            string HOME_ENV = (home_env != nullptr) ? home_env : "";
            string path_to_go;
            
            if (args.empty()) {
                if (HOME_ENV != "") {
                    path_to_go = HOME_ENV;
                } else {
                    path_to_go = "";
                }
            } else {
                path_to_go = args[0];
            }

            if (!args.empty() && !path_to_go.empty() && path_to_go[0] == '~') {
                std::string suffix = path_to_go.substr(1);
                if (!HOME_ENV.empty()) {
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
            continue;
        } else if (command_name == "history") {
            if (args.empty()) {
                cmd_history.print_history(-1);
            } else if (args[0] == "-r") {
                cmd_history.append_commands_to_history_from_file(args[1]);
            } else if (args[0] == "-a") {
                cmd_history.append_history_to_file(args[1]);
            } else if (args[0] == "-w") {
                cmd_history.write_history_to_file(args[1]);
            } else {
                int pos = stoi(args[0]);
                cmd_history.print_history(pos);
            }
            continue;
        }

        // Handle pipelines
        vector<vector<string>> pipe_commands;
        vector<string> temp_tokens;
        for (auto token : tokens) {
            if (token == "|") {
                pipe_commands.push_back(temp_tokens);
                temp_tokens.clear();
            } else {
                temp_tokens.push_back(token);
            }
        }
        if (!temp_tokens.empty()) {
            pipe_commands.push_back(temp_tokens);
        }

        execute_pipeline(pipe_commands);
    }
    return 0;
}
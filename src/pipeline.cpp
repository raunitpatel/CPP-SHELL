#include "shell.h"
#include <sys/wait.h>
#include <unistd.h>
#include <cstdlib>
#include <fstream>

void execute_pipeline(vector<vector<string>> &pipe_commands) {
    int num_cmds = pipe_commands.size();
    int pipe_fds[2];
    int prev_fd = -1;
    
    for (int i = 0; i < num_cmds; i++) {
        if (i < num_cmds - 1 && pipe(pipe_fds) == -1) {
            perror("pipe");
            return;
        }
        
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            return;
        }
        
        if (pid == 0) {
            if (i > 0) {
                dup2(prev_fd, STDIN_FILENO);
                close(prev_fd);
            }
            if (i < (num_cmds - 1)) {
                close(pipe_fds[0]);
                dup2(pipe_fds[1], STDOUT_FILENO);
                close(pipe_fds[1]);
            }

            vector<string>& tokens = pipe_commands[i];
            string command_name = tokens[0];
            vector<string> args(tokens.begin() + 1, tokens.end());

            if (command_name == "echo") {
                bool has_stdout_redirect = false;
                bool has_stderr_redirect = false;
                bool has_stdout_append_redirect = false;
                bool has_stderr_append_redirect = false;

                string stdout_file = "";
                string stderr_file = "";
                string content = "";

                for (int i = 0; i < args.size(); i++) {
                    if ((args[i] == "1>" or args[i] == ">") and i + 1 < args.size()) {
                        stdout_file = args[i + 1];
                        has_stdout_redirect = true;
                        i++;
                    } else if ((args[i] == "1>>" or args[i] == ">>") and i + 1 < args.size()) {
                        stdout_file = args[i + 1];
                        has_stdout_append_redirect = true;
                        i++;
                    } else if (args[i] == "2>" and i + 1 < args.size()) {
                        stderr_file = args[i + 1];
                        has_stderr_redirect = true;
                        i++;
                    } else if (args[i] == "2>>" and i + 1 < args.size()) {
                        stderr_file = args[i + 1];
                        has_stderr_append_redirect = true;
                        i++;
                    } else {
                        content += args[i] + " ";
                    }
                }

                if (!content.empty()) {
                    content.pop_back();
                }
                
                if (has_stdout_append_redirect) {
                    ofstream out(stdout_file, ios::app);
                    if (out.is_open()) {
                        out << content << endl;
                        out.close();
                    }
                } else if (has_stdout_redirect) {
                    ofstream out(stdout_file);
                    if (out.is_open()) {
                        out << content << endl;
                        out.close();
                    }
                } else {
                    cout << content << endl;
                }

                if (has_stderr_redirect) {
                    ofstream err(stderr_file);
                    if (err.is_open()) {
                        err.close();
                    }
                }
                exit(0);
            } else if (command_name == "type") {
                if (args.size() > 0 and commands.find(args[0]) != commands.end()) {
                    cout << args[0] << " is a shell builtin" << endl;
                } else if (args.size() > 0) {
                    string filepath = get_executable_path(args[0]);
                    if (filepath != "") {
                        cout << args[0] << " is " << filepath << endl;
                    } else {
                        cout << args[0] << ": not found" << endl;
                    }
                }
                exit(0);
            } else if (get_executable_path(command_name) != "") {
                string fullCommand = escapeShellArg(command_name);
                for (auto arg : args) {
                    fullCommand += " " + escapeShellArg(arg);
                }
                system(fullCommand.c_str());
                exit(0);
            } else {
                string input = "";
                for (auto x : pipe_commands[i]) {
                    input += x;
                    input += ' ';
                }
                if (!input.empty()) input.pop_back();
                cout << input << ": command not found" << endl;
                exit(127);
            }
        } else {
            if (prev_fd != -1) close(prev_fd);
            if (i < num_cmds - 1) {
                close(pipe_fds[1]);
                prev_fd = pipe_fds[0];
            }
        }
    }
    while (wait(nullptr) > 0);
}
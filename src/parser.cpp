#include "shell.h"
#include <cstdlib>
#include <unistd.h>
#include <algorithm>

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
            } else if (is_double_quote) {
                if (i + 1 < len) {
                    char next_char = rem_command[i + 1];
                    if (next_char == '\\' || next_char == '"' || next_char == '$' || next_char == '\n') {
                        escaped = true;
                        continue;
                    } else {
                        output += ch;
                    }
                } else {
                    output += ch;
                }
            } else {
                escaped = true;
                continue;
            }
        } else if (ch == '"' && !is_single_quote) {
            is_double_quote = !is_double_quote;
        } else if (ch == '\'' && !is_double_quote) {
            is_single_quote = !is_single_quote;
        } else if (ch == ' ' && !is_single_quote && !is_double_quote) {
            if (!output.empty()) {
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

string escapeShellArg(string arg) {
    if (arg.find_first_of(" \t\n'\"\\") == string::npos) {
        return arg;
    }

    if (arg.find('\'') != string::npos) {
        return "\"" + arg + "\"";
    }

    return "'" + arg + "'";
}

string get_executable_path(string target_filename) {
    string PATH_ENV = "";
    const char* path_env = getenv("PATH");
    if (path_env == nullptr) return "";

    PATH_ENV = path_env;
    vector<string> folders;
    string folder = "";
    string ans = "";
    for (auto x : PATH_ENV) {
        if (x == ':') {
            folders.push_back(folder);
            folder = "";
        } else {
            folder += x;
        }
    }
    if (!folder.empty()) {
        folders.push_back(folder);
    }

    for (auto dir : folders) {
        if (fs::exists(dir) == false) continue;

        fs::path filepath = fs::path(dir) / target_filename;
        if (fs::exists(filepath) && access(filepath.c_str(), X_OK) == 0) {
            ans = filepath.string();
            return ans;
        }
    }
    return ans;
}
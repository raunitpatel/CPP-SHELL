#include "shell.h"
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include <cstdlib>

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
        const char* path_env = getenv("PATH");
        if (path_env != nullptr) {
            PATH_ENV = path_env;
            vector<string> folders;
            string folder = "";
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

                for (auto file : fs::directory_iterator(dir)) {
                    if (file.is_regular_file() && access(file.path().c_str(), X_OK) == 0) {
                        all_candidates.insert(file.path().filename().string());
                    }
                }
            }
        }

        for (auto candidate : all_candidates) {
            if (candidate.find(text) == 0) {
                matches.push_back(candidate);
            }
        }
    }

    if (match_index < matches.size()) {
        return strdup(matches[match_index++].c_str());
    } else {
        return nullptr;
    }
}

char** command_completion(const char* text, int start, int end) {
    // rl_attempted_completion_over = 1; // prevent readline from filename completion
    return rl_completion_matches(text, command_generator);
}
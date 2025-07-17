#include "shell.h"
#include <readline/history.h>
#include <fstream>

History::History() {
    curr_pos = 0;
    sz = 0;
    last_appended_index = 0;
}

void History::write_history_to_file(string filename) {
    ofstream outfile(filename);
    for (auto &cmd : historic_commands) {
        outfile << cmd << endl;
    }
    outfile.close();
}

void History::append_history_to_file(string filename) {
    ofstream outfile(filename, ios::app);
    for (int i = last_appended_index; i < sz; ++i) {
        outfile << historic_commands[i] << endl;
    }
    last_appended_index = sz;
    outfile.close();
}

void History::append_commands_to_history_from_file(string filename) {
    ifstream infile(filename);
    string line;
    while (getline(infile, line)) {
        if (!line.empty()) {
            historic_commands.push_back(line);
            add_history(line.c_str());
            sz++;
        }
    }
    infile.close();
}

void History::put_commands_in_history(string command) {
    historic_commands.push_back(command);
    sz++;
    add_history(command.c_str());
}

void History::print_history(int x) {
    if (sz == 0) {
        return;
    } else if (x != -1) {
        for (int i = sz - x; i < sz; i++) {
            cout << i + 1 << " " << historic_commands[i] << endl;
        }
    } else {
        for (int i = 0; i < sz; i++) {
            cout << i + 1 << " " << historic_commands[i] << endl;
        }
    }
}
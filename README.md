# CPP SHELL

A feature-rich, Unix-like shell implementation written in C++ with support for command execution, pipelines, history management, and tab completion.

## Features

### Core Shell Features
- **Interactive Command Line**: Full readline support with command history navigation
- **Built-in Commands**: `exit`, `echo`, `type`, `pwd`, `cd`, `history`
- **External Command Execution**: Run any executable from PATH
- **Pipeline Support**: Chain commands with `|` operator
- **Tab Completion**: Auto-complete commands and executables from PATH
- **Quote Handling**: Support for single quotes, double quotes, and escape sequences
- **I/O Redirection**: Basic output redirection support in echo command

### Advanced Features
- **Command History**: Persistent command history with file support
- **Path Resolution**: Automatic executable discovery from PATH environment variable
- **Home Directory Support**: `~` expansion in cd command
- **Signal Handling**: Proper EOF (Ctrl+D) handling
- **Error Handling**: Comprehensive error messages and exit codes

## Project Structure

```
src/
├── shell.h          # Main header file with declarations
├── main.cpp         # Main shell loop and built-in commands
├── completion.cpp   # Tab completion system
├── parser.cpp       # Command parsing and text processing
├── executor.cpp     # Command execution and pipeline handling
├── history.cpp      # History management implementation
```

## Prerequisites

### System Requirements
- Linux/Unix-like operating system
- C++17 compatible compiler (GCC 7+ or Clang 5+)
- GNU Readline library

### Installing Dependencies

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install build-essential libreadline-dev
```


## Building and Running

### Quick Start
```bash
# Clone or download the project
git clone <repository-url>
cd CPP-SHELL

# Build and run the code
chmod +x your_program.sh
./your_program.sh
```

## Usage Examples

### Basic Commands
```bash
$ pwd
/home/user/shell

$ echo "Hello World"
Hello World

$ type ls
ls is /bin/ls

$ cd /tmp
$ pwd
/tmp

$ cd ~
$ pwd
/home/user
```

### Pipeline Examples
```bash
$ ls | grep .cpp
main.cpp
completion.cpp
parser.cpp

$ echo "test" | cat
test
```

### History Management
```bash
$ history          # Show all history
$ history 5        # Show last 5 commands
$ history -w file  # Write history to file
$ history -r file  # Read history from file
$ history -a file  # Append history to file
```

### Tab Completion
- Press `Tab` to auto-complete commands
- Works with both built-in commands and external executables
- Supports partial command matching

## Built-in Commands

| Command | Description | Usage |
|---------|-------------|--------|
| `exit [0]` | Exit the shell | `exit` or `exit 0` |
| `pwd` | Print working directory | `pwd` |
| `cd [path]` | Change directory | `cd`, `cd /path`, `cd ~` |
| `echo [args]` | Print arguments | `echo Hello World` |
| `type [cmd]` | Show command type/location | `type ls` |
| `history [n]` | Show command history | `history`, `history 10` |

### History Command Options
- `history -w file` - Write current history to file
- `history -r file` - Read history from file
- `history -a file` - Append new history entries to file

## Environment Variables

The shell respects the following environment variables:
- `PATH` - Used for executable discovery and tab completion
- `HOME` - Used for `~` expansion in cd command
- `HISTFILE` - Persistent history file location

## Configuration

### Setting Up History Persistence
```bash
# In your .bashrc or .profile
export HISTFILE=~/.shell_history

# Then run the shell
./your_program.sh
```

## Limitations

- No background job control (`&`, `jobs`, `fg`, `bg`)
- Limited I/O redirection (only basic output redirection in echo)
- No command substitution or variable expansion
- No wildcard/glob expansion
- No complex control structures (if, while, for)

## Development

### Code Organization
- **shell.h**: All function declarations and class definitions
- **main.cpp**: Main shell loop and built-in command implementations
- **completion.cpp**: Tab completion system using readline
- **parser.cpp**: Command parsing, tokenization, and utility functions
- **pipeline.cpp**: Command execution, pipeline handling, and process management
- **history.cpp**: History class implementation with file I/O

### Adding New Built-in Commands
1. Add command name to the `commands` set in main.cpp
2. Add command handling logic in the main loop
3. Update tab completion in completion.cpp if needed

## Future Enhancements

- [ ] Job control support
- [ ] Variable expansion and substitution
- [ ] Wildcard/glob pattern matching
- [ ] Configuration file support
- [ ] Syntax highlighting
- [ ] Command aliases

---


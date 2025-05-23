
# Simple-C-Shell

A lightweight, custom Unix shell implementation written in C with a range of built-in commands and features.

## Overview

This shell provides a command-line interface with standard shell functionality plus several custom commands. It supports command execution, background processes, process management, I/O redirection, piping, and more.

## Features

- **Custom prompt** displaying username, hostname, and current directory
- **Command history** with log functionality
- **Background process** execution and management
- **Signal handling** for Ctrl+C (SIGINT) and Ctrl+Z (SIGTSTP)
- **I/O redirection** and command piping
- **Process management** with fg/bg commands and activities display
- **File system navigation** with custom commands

## key components:

    Command Parsing – Tokenizing user input to identify commands and arguments.
    Execution – Handling built-in and external commands using system calls (fork, exec, wait).
    Redirection – Supporting input (<) and output (>) redirection.
    Pipelines – Implementing command chaining using pipes (|).
    Background Execution – Running processes in the background using &.
    Error Handling – Managing invalid commands and execution failures.
    Modular Code Structure – Separating functionalities into multiple C files with proper header files.

## Commands

| Command | Description | Usage |
|---------|-------------|-------|
| `hop` | Change directory with enhanced features | `hop [directory]` |
| `reveal` | List directory contents with options | `reveal [-a] [-l] [path]` |
| `log` | Display, execute, or purge command history | `log`, `log execute <index>`, `log purge` |
| `proclore` | Display process information | `proclore [pid]` |
| `seek` | Search for files/directories | `seek [-d/-f/-e] <name> [directory]` |
| `activities` | List all background processes | `activities` |
| `ping` | Send a signal to a process | `ping <pid> <signal_number>` |
| `fg` | Bring background process to foreground | `fg <pid>` |
| `bg` | Resume a stopped background process | `bg <pid>` |
| `neonate` | Monitor the most recent process ID | `neonate [-n <time>]` |
| `iMan` | Fetch man pages from online source | `iMan <command>` |

## Command Details

### hop
Navigate between directories with support for special paths:
- `~` for home directory
- `-` for previous directory
- `.` for current directory
- `..` for parent directory

### reveal
List files and directories with options:
- `-a` to show hidden files
- `-l` to show detailed information (permissions, size, etc.)

### log
Command history management:
- Without arguments: displays command history
- `log execute <index>`: executes a command from history
- `log purge`: clears command history

### proclore
Display detailed information about a process:
- Process ID
- Process status
- Process group
- Virtual memory usage
- Executable path

### seek
Search for files and directories:
- `-d` to search for directories only
- `-f` to search for files only
- `-e` to execute/open the first result

## Signal Handling

- **Ctrl+C (SIGINT)**: Terminates the foreground process
- **Ctrl+Z (SIGTSTP)**: Stops the foreground process and moves it to background
- **Ctrl+D**: Exits the shell

## I/O Redirection and Piping

The shell supports:
- Input redirection using `<`
- Output redirection using `>`
- Output appending using `>>`
- Command piping using `|`

## Compilation and Usage

To compile the shell:
```bash
make
```

To run the shell:
```bash
./a.out
```

## Exit

To exit the shell, type `exit`.

## Implementation Notes

- The shell maintains a persistent command history in `command_log.txt`
- Background processes are tracked and their status is updated upon completion
- The shell handles multiple commands separated by semicolons
- Home directory is represented as `~` in the prompt

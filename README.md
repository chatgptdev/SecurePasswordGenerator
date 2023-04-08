# Secure Password Generator

Secure Password Generator is a command-line tool for generating secure passwords. The program supports customization of password length, the inclusion of special characters, and the ability to generate multiple passwords at once. Additionally, it allows users to write the generated passwords to a file or append them to an existing file, and copy the passwords to the clipboard, providing flexible output options for different use cases.

## Features

- Generate secure passwords with a specified length (default length: 20)
- Option to include special characters
- Option to generate multiple passwords at once
- Command-line switches for controlling password length, special character inclusion, and quiet mode
- Supports writing generated passwords to a file or appending them to an existing file
- Supports copying generated passwords to the clipboard on Windows, Linux, and macOS

## Implementation

The program is implemented in C++ and uses the C++11 random number library for generating random characters. It uses a cryptographically secure pseudo-random number generator to ensure the generated passwords are secure.

## Building the Program

### Prerequisites

- C++11 compatible compiler
- CMake 3.10 or higher

### Building on Windows, Linux, and macOS

1. Clone the repository:

```sh
git clone https://github.com/chatgptdev/SecurePasswordGenerator.git
cd SecurePasswordGenerator
```

2. Create a build directory:

```sh
mkdir build && cd build
```

3. Run CMake to generate build files:

```sh
cmake ..
```

4. Build the project:

```sh
cmake --build .
```

The secure_password_generator binary will be created in the build directory.

## Usage

```
Usage: secure_password_generator [OPTIONS]
Generate secure passwords with optional constraints.

Options:
  -l LENGTH    Set the password length (default: 20, minimum: 6)
  -s           Require at least one special character
  -n NUM       Generate NUM passwords (default: 1)
  -q           Quiet mode - only print passwords
  -f FILE      Write passwords to FILE (-a to append, otherwise overwrite)
  -a           Append passwords to the file specified with -f
  -c           Copy generated passwords to the clipboard (no console outputin this case)
  -h           Display this help message and exit
```

## Author

ChatGPT 4.0 based on prompts by @chatgptdev (chatgptdev@tuta.io)

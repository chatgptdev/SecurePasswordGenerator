/*
 * Secure Password Generator
 * 
 * Purpose:
 *   This program generates secure passwords with optional constraints
 *   such as length, the presence of special characters, and the number
 *   of passwords to generate.
 *
 * Copyright (c) 2023 chatgptdev
 *
 * This software was written mostly by ChatGPT 4.0 using instructions by
 * @chatgptdev. It is provided under the Apache License, Version 2.0 
 * (the "License"); you may not use this software except in compliance with
 * the License. You may obtain a copy of the License at:
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifdef _WIN32
#include <Windows.h>
#endif
#include <iostream>
#include <fstream>
#include <ctime>
#include <memory>
#include <random>
#include <string>
#include <algorithm>
#include <cstring>

#define APP_VERSION "1.1.0"

struct secure_deleter {
    void operator()(char *ptr) const {
        if (ptr) {
            std::fill(ptr, ptr + std::strlen(ptr), 0);
            delete[] ptr;
        }
    }
};

std::unique_ptr<char[], secure_deleter> generate_password(int length, bool hasSymbols, bool hasSpecial) {
    const std::string uppercase_letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const std::string lowercase_letters = "abcdefghijklmnopqrstuvwxyz";
    const std::string digits = "0123456789";
    const std::string symbols = "+-/*";
    const std::string special_chars = "!@#$%^&*()_= [{]}\\|;:'\",<.>?`~";

    std::unique_ptr<char[], secure_deleter> password(new char[length + 1]);

    std::random_device rd;
    auto gen = [&rd](int min, int max) {
        std::uniform_int_distribution<> dist(min, max);
        return dist(rd);
    };

    password[0] = uppercase_letters[gen(0, uppercase_letters.length() - 1)];
    password[1] = lowercase_letters[gen(0, lowercase_letters.length() - 1)];
    password[2] = digits[gen(0, digits.length() - 1)];

    if (hasSymbols) {
        password[3] = symbols[gen(0, symbols.length() - 1)];
    }

    int special_count = hasSpecial ? gen(1, length / 3) : 0;
    int current_special_count = 0;

    for (int i = 3 + (hasSymbols ? 1 : 0); i < length; ++i) {
        if (current_special_count < special_count && i >= length - special_count) {
            password[i] = special_chars[gen(0, special_chars.length() - 1)];
            current_special_count++;
        } else {
            int random_selector;
            if (hasSymbols) {
                random_selector = gen(0, 3);
            } else {
                random_selector = gen(0, 2);
            }

            switch (random_selector) {
                case 0:
                    password[i] = uppercase_letters[gen(0, uppercase_letters.length() - 1)];
                    break;
                case 1:
                    password[i] = lowercase_letters[gen(0, lowercase_letters.length() - 1)];
                    break;
                case 2:
                    password[i] = digits[gen(0, digits.length() - 1)];
                    break;
                case 3:
                    if (hasSymbols) {
                        password[i] = symbols[gen(0, symbols.length() - 1)];
                    }
                    break;
            }
        }
    }

    std::shuffle(password.get(), password.get() + length, rd);
    password[length] = '\0';

    return password;
}

bool copy_to_clipboard(const std::string& text) {
#ifdef _WIN32
    if (!OpenClipboard(nullptr)) {
        return false;
    }

    if (!EmptyClipboard()) {
        CloseClipboard();
        return false;
    }

    HGLOBAL hGlob = GlobalAlloc(GMEM_FIXED, text.size() + 1);
    if (!hGlob) {
        CloseClipboard();
        return false;
    }

    memcpy(GlobalLock(hGlob), text.c_str(), text.size() + 1);
    GlobalUnlock(hGlob);

    if (SetClipboardData(CF_TEXT, hGlob) == nullptr) {
        CloseClipboard();
        return false;
    }

    CloseClipboard();
    return true;
#elif defined(__APPLE__) || defined(__linux__)
    FILE* pipe;
    const char* cmd;

    #ifdef __APPLE__
       cmd = "pbcopy";
    #elif defined(__linux__)
    // First, check if xclip is available
    if (system("command -v xclip > /dev/null") == 0) {
        // Use xclip
        cmd = "xclip -selection clipboard -i";
    }
    // If xclip is not available, check if xsel is available
    else if (system("command -v xsel > /dev/null") == 0) {
        // Use xsel
        cmd = "xsel -ib", "w";
    } else {
        std::cerr << "Error: Could not find xclip or xsel. Please install one of them to copy passwords to clipboard." << std::endl;
        return false;
    }
    //    cmd = "xclip -selection clipboard";
    #endif

    pipe = popen(cmd, "w");
    if (!pipe) {
        return false;
    }

    fwrite(text.c_str(), 1, text.size(), pipe);
    fclose(pipe);
    return true;
#else
    return false;
#endif
}

void print_help() {
    std::cout << "Usage: secure_password_generator [OPTIONS]" << std::endl;
    std::cout << "Generate secure passwords with optional constraints." << std::endl;
    std::cout << "Version: " << APP_VERSION << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -l LENGTH\tSet the password length (default: 20, minimum: 6)" << std::endl;
    std::cout << "  -b\t\tRequire at least one symbol character('+','-','/' or '*')" << std::endl;
    std::cout << "  -s\t\tRequire at least one special character" << std::endl;
    std::cout << "  -n NUM\tGenerate NUM passwords (default: 1)" << std::endl;
    std::cout << "  -q\t\tQuiet mode - only print passwords" << std::endl;
    std::cout << "  -f FILE\tWrite passwords to FILE (-a to append, otherwise overwrite)" << std::endl;
    std::cout << "  -a\t\tAppend passwords to the file specified with -f" << std::endl;
    std::cout << "  -c\t\tCopy generated password(s) to clipboard without printing them." << std::endl;
    std::cout << "  -h\t\tDisplay this help message and exit" << std::endl;
}

int main(int argc, char *argv[]) {
    int password_length = 20;
    bool has_symbol = false;
    bool has_special = false;
    int num_passwords = 1;
    bool quiet_mode = false;
    bool append = false;
    bool clipboard = false;
    std::string file_path;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-l") == 0) {
            if (i + 1 < argc) {
                int input_length = std::stoi(argv[++i]);
                if (input_length >= 6) {
                    password_length = input_length;
                } else {
                    std::cerr << "Error: Password length must be at least 6." << std::endl;
                    print_help();
                    return -1;
                }
            } else {
                std::cerr << "Error: Missing value for -l switch." << std::endl;
                print_help();
                return -1;
            }
        } else if (strcmp(argv[i], "-s") == 0) {
            has_special = true;
        } else if (strcmp(argv[i], "-b") == 0) {
            has_symbol = true;
        } else if (strcmp(argv[i], "-n") == 0) {
            if (i + 1 < argc) {
                num_passwords = std::stoi(argv[++i]);
            } else {
                std::cerr << "Error: Missing value for -n switch." << std::endl;
                print_help();
                return -1;
            }
        } else if (strcmp(argv[i], "-q") == 0) {
            quiet_mode = true;
        } else if (strcmp(argv[i], "-f") == 0) {
            if (i + 1 < argc) {
                file_path = argv[++i];
            } else {
                std::cerr << "Error: Missing value for -f switch." << std::endl;
                print_help();
                return -1;
            }
        } else if (strcmp(argv[i], "-a") == 0) {
            append = true;
        } else if (strcmp(argv[i], "-c") == 0) {
            clipboard = true;
            quiet_mode = true;
      } else if (strcmp(argv[i], "-h") == 0) {
          print_help();
          return 0;
      } else {
          std::cerr << "Error: Unknown switch: " << argv[i] << std::endl;
          print_help();
          return -1;
      }
  }

  std::ofstream output_file;
  if (!file_path.empty()) {
      output_file.open(file_path, append ? std::ios::app : std::ios::trunc);
      if (!output_file.is_open()) {
          std::cerr << "Error: Cannot open output file." << std::endl;
          return -1;
      }
  }

  if (!quiet_mode && file_path.empty()) {
      if (num_passwords == 1) {
          std::cout << "Generated secure password: ";
      } else {
          std::cout << "Generated secure passwords:" << std::endl;
      }
  }

  std::vector<std::unique_ptr<char[], secure_deleter>> clipboard_passwords;
  int clipboard_buffer_length = 0;
  int ret = 0;

  for (int i = 0; i < num_passwords; ++i) {
      std::unique_ptr<char[], secure_deleter> secure_password = generate_password(password_length, has_symbol, has_special);
      if (!file_path.empty()) {
          output_file.write(secure_password.get(), password_length);
          output_file << std::endl;
      } else if (!clipboard) {
          std::cout << secure_password.get() << std::endl;
      }

      if (clipboard) {
          clipboard_passwords.push_back(std::move(secure_password));
          clipboard_buffer_length += password_length + 1;
      }
  }

  if (clipboard && !clipboard_passwords.empty()) {
      std::unique_ptr<char[], secure_deleter> clipboard_buffer(new char[clipboard_buffer_length]);
      int buffer_position = 0;

      for (const auto &password : clipboard_passwords) {
          std::memcpy(clipboard_buffer.get() + buffer_position, password.get(), password_length);
          buffer_position += password_length;
          clipboard_buffer[buffer_position++] = '\n';
      }
      clipboard_buffer[buffer_position - 1] = '\0';

      if (!copy_to_clipboard(std::string(clipboard_buffer.get()))) {
          std::cerr << "Error: Failed to copy password(s) to clipboard." << std::endl;
          ret = -1;
      }
  }

  if (output_file.is_open()) {
      output_file.close();
  }

  return ret;
}


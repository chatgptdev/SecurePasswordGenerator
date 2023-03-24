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

#include <iostream>
#include <fstream>
#include <ctime>
#include <random>
#include <string>
#include <algorithm>
#include <cstring>

std::string generate_password(int length, bool hasSpecial) {
    const std::string uppercase_letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const std::string lowercase_letters = "abcdefghijklmnopqrstuvwxyz";
    const std::string digits = "0123456789";
    const std::string symbols = "+-/*";
    const std::string special_chars = "!@#$%^&*()_= [{]}\\|;:'\",<.>?`~";

    std::string password = "";
    password.reserve(length);

    std::random_device rd;
    auto gen = [&rd](int min, int max) {
        std::uniform_int_distribution<> dist(min, max);
        return dist(rd);
    };

    password.push_back(uppercase_letters[gen(0, uppercase_letters.length() - 1)]);
    password.push_back(lowercase_letters[gen(0, lowercase_letters.length() - 1)]);
    password.push_back(digits[gen(0, digits.length() - 1)]);
    password.push_back(symbols[gen(0, symbols.length() - 1)]);

    int special_count = hasSpecial ? gen(1, length / 3) : 0;
    int current_special_count = 0;

    for (int i = 4; i < length; ++i) {
        if (current_special_count < special_count && i >= length - special_count) {
            password.push_back(special_chars[gen(0, special_chars.length() - 1)]);
            current_special_count++;
        } else {
            int random_selector = gen(0, 4);
            switch (random_selector) {
                case 0:
                    password.push_back(uppercase_letters[gen(0, uppercase_letters.length() - 1)]);
                    break;
                case 1:
                    password.push_back(lowercase_letters[gen(0, lowercase_letters.length() - 1)]);
                    break;
                case 2:
                    password.push_back(digits[gen(0, digits.length() - 1)]);
                    break;
                case 3:
                    password.push_back(symbols[gen(0, symbols.length() - 1)]);
                    break;
                case 4:
                    if (current_special_count < special_count) {
                        password.push_back(special_chars[gen(0, special_chars.length() - 1)]);
                        current_special_count++;
                    } else {
                        i--; // Decrement i to repeat this iteration with a new random_selector value
                    }
                    break;
            }
        }
    }

    std::shuffle(password.begin(), password.end(), rd);
    return password;
}

void print_help() {
    std::cout << "Usage: secure_password_generator [OPTIONS]" << std::endl;
    std::cout << "Generate secure passwords with optional constraints." << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -l LENGTH\tSet the password length (default: 20, minimum: 6)" << std::endl;
    std::cout << "  -s\t\tRequire at least one special character" << std::endl;
    std::cout << "  -n NUM\tGenerate NUM passwords (default: 1)" << std::endl;
    std::cout << "  -q\t\tQuiet mode - only print passwords" << std::endl;
    std::cout << "  -f FILE\tWrite passwords to FILE (-a to append, otherwise overwrite)" << std::endl;
    std::cout << "  -a\t\tAppend passwords to the file specified with -f" << std::endl;
    std::cout << "  -h\t\tDisplay this help message and exit" << std::endl;
}

int main(int argc, char *argv[]) {
    int password_length = 20;
    bool has_special = false;
    int num_passwords = 1;
    bool quiet_mode = false;
    bool append = false;
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

  for (int i = 0; i < num_passwords; ++i) {
      std::string secure_password = generate_password(password_length, has_special);
      if (!file_path.empty()) {
          output_file << secure_password << std::endl;
      } else {
          std::cout << secure_password << std::endl;
      }
  }

  if (output_file.is_open()) {
      output_file.close();
  }

  return 0;
}

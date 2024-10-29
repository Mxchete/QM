#include "IO/blif_reader.hpp"
#include <cstdlib>
#include <iostream>
void print_help() {
  std::cout << "Usage:" << std::endl;
  std::cout << "Help: -h" << std::endl;
}
int main(int argc, char **argv) {
  std::cout << argc << std::endl;
  if (argc == 2 && argv[1] == "-h") {
    print_help();
  }
  return EXIT_SUCCESS;
}

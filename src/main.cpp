#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <memory>
#include "IO/blif_reader.hpp"
#include "QM/minterm_map.hpp"

char* get_arg(char** begin, char** end, const std::string& option)
{
  char** itr = std::find(begin, end, option);
  if (itr != end && ++itr != end)
  {
    return *itr;
  }
  return 0;
}

bool arg_exists(char** begin, char** end, const std::string& option)
{
  return std::find(begin, end, option) != end;
}

void print_help()
{
  std::cout << "Usage:" << std::endl;
  std::cout << "Help: -h" << std::endl;
}
int main(int argc, char** argv)
{
  if (arg_exists(argv, argv + argc, "-h"))
  {
    print_help();
  }

  char* filename = get_arg(argv, argv + argc, "-f");

  if (filename)
  {
    std::string file_name = argv[2];
    IO::File::BlifReader file_reader(false);
    file_reader.read_file(file_name);

    std::shared_ptr<QM::MintermMap> map = file_reader.get_map();
    return EXIT_SUCCESS;
  }
}

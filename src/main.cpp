#include <cstdlib>
#include <iostream>
#include <memory>
#include "IO/blif_reader.hpp"
#include "QM/literal_map.hpp"

void print_help()
{
  std::cout << "Usage:" << std::endl;
  std::cout << "Help: -h" << std::endl;
}
int main(int argc, char **argv)
{
  if (argc == 2)
  {
    print_help();
  }

  std::string file_name = argv[1];
  IO::File::BlifReader file_reader(false);
  file_reader.read_file(file_name);

  std::shared_ptr<QM::LiteralMap> map = file_reader.get_map();
  return EXIT_SUCCESS;
}

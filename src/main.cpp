#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <memory>
#include "IO/file_reader.hpp"
#include "IO/file_reader_factory.hpp"
#include "QM/minterm_and_dc_map.hpp"
#include "QM/minterm_map.hpp"
#include "QM/qm_process_handler.hpp"

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
    auto file_reader = IO::File::FileReaderFactory<QM::MintermDCMap>::create(file_name);

    std::cout << "Created file" << std::endl;

    QM::MintermDCMap map(file_reader->read_file());
    QM::QMProcessHandler qm_processor(map);

    QM::MintermMap final_product(qm_processor.process());
    return EXIT_SUCCESS;
  }
}

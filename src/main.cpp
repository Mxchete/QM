#include <getopt.h>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <memory>
#include "IO/file_reader.hpp"
#include "IO/file_reader_factory.hpp"
#include "IO/file_writer.hpp"
#include "IO/logger.hpp"
#include "IO/pla_writer.hpp"
#include "QM/minterm_and_dc_map.hpp"
#include "QM/minterm_map.hpp"
#include "QM/qm_process_handler.hpp"

const struct option longopts[] = {
    {"version", no_argument, 0, 'v'},
    {"help", no_argument, 0, 'h'},
    {"filepath", required_argument, 0, 'f'},
    {"outputpath", required_argument, 0, 'o'},
    {"loglevel", required_argument, 0, 'l'},
    {"logpath", required_argument, 0, 'i'},
    {0, 0, 0, 0},
};

void print_help()
{
  std::cout << std::endl << "Command    : Argument         | Explanation" << std::endl;
  std::cout << "Help       : --help, -h       | Prints this message then exits" << std::endl;
  std::cout << "Version    : --version, -v    | Prints build information then exits" << std::endl;
  std::cout
      << "Input File : --filepath, -f   | !Required! Specifies the input filepath (must be in "
         "PLA format)"
      << std::endl;
  std::cout << "Ouput File : --outputpath, -o | Specifies the output file path, otherwise prints "
               "output to stdout only"
            << std::endl;
  std::cout << "Log level  : --loglevel, -l   | Specifies the level of logs that will be printed "
               "(TRACE, DEBUG, INFO, WARN, ERROR, FATAL) Default=INFO"
            << std::endl;
  std::cout << "Log path   : --logpath, -p    | Specifies output path for logs, otherwise "
               "prints logs to "
               "stdout only"
            << std::endl;
}

int main(int argc, char** argv)
{
  // Program name & build information
  std::cout << ">>>> QM SIMPLIFIER <<<<" << std::endl;
  std::cout << "Built on: " << __DATE__ << " " << __TIME__ << std::endl;
  int idx;
  int iarg = 0;
  char* filename = nullptr;
  char* output_file = nullptr;
  std::string log_level = "INFO";
  std::string log_path = "";

  // turn off getopt error message
  opterr = 1;

  // iterate through all arguments
  while (iarg != -1)
  {
    iarg = getopt_long(argc, argv, "vhf:o:l:p:", longopts, &idx);

    switch (iarg)
    {
      case 'h':
        print_help();
        return EXIT_SUCCESS;

      case 'v':
        return EXIT_SUCCESS;

      case 'f':
        filename = optarg;
        break;

      case 'o':
        output_file = optarg;
        break;

      case 'l':
        log_level = optarg;
        break;

      case 'p':
        log_path = optarg;
        break;
    }
  }

  // only run the program if input file was received
  if (filename)
  {
    // create our logger object
    IO::Logger::LogLevel lvl = IO::Logger::get_log_level(log_level);
    std::shared_ptr<IO::Logger> logger;
    if (log_path != "")
    {
      logger = std::make_shared<IO::Logger>(log_path, lvl);
    }
    else
    {
      logger = std::make_shared<IO::Logger>(lvl);
    }
    logger->info("Main::Logger successfully created");
    // create file reader
    auto file_reader = IO::File::FileReaderFactory<QM::sBooleanFunction>::create(filename, logger);

    logger->info("File reader object created");

    // read file & get output as map of minterm & dc terms
    QM::sBooleanFunction map = file_reader->read_file();

    // create object to handle processing
    QM::QMProcessHandler qm_processor(map, logger);

    // process the map & create simplified map as output
    QM::sMintermMap final_product = qm_processor.process();
    // create our writer output object
    IO::File::PlaWriter out_file =
        (output_file) ? IO::File::PlaWriter(output_file, logger) : IO::File::PlaWriter(logger);
    // write out our simplified map
    out_file.write_file(final_product);
    logger->info("Program has successfully finished");
    return EXIT_SUCCESS;
  }

  // return failure if no input filename was supplied
  return EXIT_FAILURE;
}

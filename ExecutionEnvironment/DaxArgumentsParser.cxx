/*=========================================================================

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

===========================================================================*/
#include "DaxArgumentsParser.h"

#include <iostream>
#include <boost/program_options.hpp>
namespace po = boost::program_options;

inline unsigned int max(unsigned int a, unsigned int b)
{
  return a > b ? a : b;
}

//-----------------------------------------------------------------------------
DaxArgumentsParser::DaxArgumentsParser():
  MaxWarpSize(128),
  MaxGridSize(32768),
  ProblemSize(128)
{
}

//-----------------------------------------------------------------------------
DaxArgumentsParser::~DaxArgumentsParser()
{
}

//-----------------------------------------------------------------------------
bool DaxArgumentsParser::ParseArguments(int argc, char* argv[])
{
  po::options_description desc("Allowed options");
  desc.add_options()
    ("max-grid", po::value<unsigned int>(), "Maximum grid size (default:32768)")
    ("max-warp", po::value<unsigned int>(), "Maximum warp size (default:128)")
    ("size", po::value<unsigned int>(), "Problem size (default: 128)")
    ("help", "Generate this help message");

  po::variables_map variables;
  po::store(po::parse_command_line(argc, argv, desc), variables);
  po::notify(variables);
  if (variables.count("help") != 0)
    {
    std::cout << desc << std::endl;
    return false;
    }
  if (variables.count("max-grid") == 1)
    {
    this->MaxGridSize = max(1, variables["max-grid"].as<unsigned int>());
    }
  if (variables.count("max-warp") == 1)
    {
    this->MaxWarpSize = max(1, variables["max-warp"].as<unsigned int>());
    }
  if (variables.count("size") == 1)
    {
    this->ProblemSize = max(1, variables["size"].as<unsigned int>());
    }
  return true;
}

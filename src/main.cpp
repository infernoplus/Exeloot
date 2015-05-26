//Wrote this tool with no use of smart pointers or vectors.
//I wanted to learn more about the way raw pointers/arrays worked in c++
//TODO: This program probably needs some serious inspection for memory leaks bad pointer use and error catching/reporting

#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <sstream>
#include <fstream>

#include <string.h>
#include <math.h>

#include "model/import.hpp"

#define uint unsigned int

int main(int argc, char** argv) {
  std::cout << "\n-- Exetool v0.1 --\n";

  if(argc < 2) {
	  std::cout << "No command given.\nUse \'exeloot help\' for a list of commands.\n";
	  return -1;
  }

  std::string cmd = std::string(argv[1]);

  if(cmd == "obj2gls") {
	  if(argc != 4) {
		  std::cout << "Incorrect number of parameters for command \'obj2gls\'. <" << argc-2 << "> \n";
		  return -1;
	  }
	  return model::importModel(std::string(argv[2]), std::string(argv[3]));
  }
  if(cmd == "help") {
	  std::cout << "List of valid commands:\n obj2gls <input-obj> <output-gls>\n help\n";
	  return 0;
  }
  else {
	  std::cout << "Unknown command \'" << argv[1] << "\'.\nUse \'exeloot help\' for a list of commands.\n";
	  return -1;
  }
    
  return 0;
}

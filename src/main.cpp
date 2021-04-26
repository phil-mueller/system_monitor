#include "ncurses_display.h"
#include "system.h"
// The latter two are include just for testing purposes, delete later!
#include "linux_parser.h"
#include <iostream>
#include <vector>
#include <string>

int main() {
  std::cout << LinuxParser::TotalProcesses() << std::endl;
  //System system;
  //NCursesDisplay::Display(system);
}
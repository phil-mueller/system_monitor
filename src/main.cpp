#include "ncurses_display.h"
#include "system.h"
// The latter two are include just for testing purposes, delete later!
#include "linux_parser.h"
#include <iostream>
#include <vector>
#include <string>

int main() {
  int i=1810;
  std::cout << LinuxParser::UpTime(i)<< std::endl;
  //System system;
  //NCursesDisplay::Display(system);
}
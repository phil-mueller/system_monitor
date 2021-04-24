#include "ncurses_display.h"
#include "system.h"
// The latter two are include just for testing purposes, delete later!
#include "linux_parser.h"
#include <iostream>

int main() {  
  std::cout << LinuxParser::Jiffies() << std::endl;
  //System system;
  //NCursesDisplay::Display(system);
}
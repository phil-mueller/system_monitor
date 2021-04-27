#include "ncurses_display.h"
#include "system.h"

// Just for testing
#include "processor.h"
#include "linux_parser.h"
#include <iostream>
#include <string>
#include <vector>

int main() {
  System system;
  NCursesDisplay::Display(system);
}
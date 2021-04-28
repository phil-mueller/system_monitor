#include <chrono>
#include <thread>
#include "ncurses_display.h"
#include "system.h"

int main() {
  System system;
  std::this_thread::sleep_for(std::chrono::seconds(1));
  NCursesDisplay::Display(system);
}
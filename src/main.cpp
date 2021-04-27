#include "ncurses_display.h"
#include "system.h"
#include <chrono>
#include <thread>

int main() {
  System system;
  std::this_thread::sleep_for(std::chrono::seconds(1));
  NCursesDisplay::Display(system);
}
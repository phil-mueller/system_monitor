#include <sstream>
#include <string>

#include "format.h"

using std::string;

// Formatting Helper Function. INPUT: Long int measuring seconds. OUTPUT: String
// "HH:MM:SS"
string Format::ElapsedTime(long seconds) {
  long minutes = seconds / 60;
  seconds = seconds % 60;
  long hours = minutes / 60;
  minutes = minutes % 60;
  // Create Output String based on number of hours/minutes/seconds digits
  // To Do for future: rework using std::chrono library
  std::ostringstream oss;
  if (hours < 10) {
    oss << "0" << hours << ":";
  } else {
    oss << hours << ":";
  }
  if (minutes < 10) {
    oss << "0" << minutes << ":";
  } else {
    oss << minutes << ":";
  }
  if (seconds < 10) {
    oss << "0" << seconds;
  } else {
    oss << seconds;
  }
  return oss.str();
}
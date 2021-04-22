#include <string>
#include <sstream>

#include "format.h"

using std::string;

// DONE: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
    long minutes = seconds/60;
    seconds = seconds % 60;
    long hours = minutes / 60;
    minutes = minutes % 60;
    std::ostringstream oss;
    oss << hours << ":" << minutes << ":" << seconds;
    return oss.str(); 
}
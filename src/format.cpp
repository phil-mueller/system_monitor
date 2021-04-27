#include <string>
#include <sstream>

#include "format.h"

using std::string;

// DONE: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
    long minutes = seconds/60;
    seconds = seconds % 60;
    long hours = minutes / 60;
    minutes = minutes % 60;
    std::ostringstream oss;
    if(hours<10){
        oss << "0" << hours << ":";
    }
    else{
        oss << hours << ":";
    }
    if(minutes<10){
        oss << "0" << minutes << ":";
    }
    else{
        oss << minutes << ":";
    }
    if(seconds<10){
        oss << "0" << seconds;
    }
    else{
        oss << seconds;
    }
    return oss.str(); 
}
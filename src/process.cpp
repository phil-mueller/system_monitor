#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// Added Constructor to initiate Process with PID
Process::Process(int p) : pid_(p){}

// DONE: Return this process's ID
int Process::Pid() { return pid_; }

// DONE: Return this process's CPU utilization
float Process::CpuUtilization() const {
    float utilization;
    long activejiffies = LinuxParser::ActiveJiffies(pid_);
    long seconds = LinuxParser::UpTime(pid_);
    utilization = ((float(activejiffies)/sysconf(_SC_CLK_TCK))/float(seconds));
    return utilization;
}

// DONE: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(pid_); }

// DONE: Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(pid_); }

// DONE: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid_); }

// DONE: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

// TODO: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const { 
    return (a.CpuUtilization() < this->CpuUtilization());
}
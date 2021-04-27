#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"

using std::string;
using std::to_string;
using std::vector;

// Constructor to initiate Process with PID
Process::Process(int p) : pid_(p) {}

/* Process Attribute Getter Function are Pass-Throughs
to the Linux Parser Namespace using the PID as Argument */

// Return this process's ID
int Process::Pid() { return pid_; }

// Return this process's CPU utilization
float Process::CpuUtilization() const {
  float utilization;
  long activejiffies = LinuxParser::ActiveJiffies(pid_);
  long seconds = LinuxParser::UpTime(pid_);
  utilization =
      ((float(activejiffies) / sysconf(_SC_CLK_TCK)) / float(seconds));
  return utilization;
}

// Return the command that generated this process
string Process::Command() { return LinuxParser::Command(pid_); }

// Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(pid_); }

// Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid_); }

// Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

// Overload of the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
  return (a.CpuUtilization() < this->CpuUtilization());
}
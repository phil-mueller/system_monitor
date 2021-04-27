#include <dirent.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;
using std::stol;

// Read Operating System Name from File
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// Read Kernel Name
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// For Later: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Read System Memory Utilization
float LinuxParser::MemoryUtilization() {
  float utilization;
  float memtotal, memfree, buffers;
  string key, value, ending;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    string line;
    for (int i = 1; i < 5; i++) {
      std::getline(stream, line);
      std::istringstream linestream(line);
      linestream >> key >> value >> ending;
      if (key == "MemTotal:") {
        memtotal = std::stof(value);
      } else if (key == "MemFree:") {
        memfree = std::stof(value);
      } else if (key == "Buffer:") {
        buffers = stof(value);
      }
    }
  }
  utilization = 1.0 - (memfree / (memtotal - buffers));
  return utilization;
}

// Read System Uptime
long LinuxParser::UpTime() {
  long uptime{0};
  string uptime_string, idletime_string;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime_string >> idletime_string;
  }
  uptime = stol(uptime_string);
  return uptime;
}

// Read Number of Total Jiffies for the System
long LinuxParser::Jiffies() {
  vector<string> v = LinuxParser::CpuUtilization();
  long jiffies = stol(v[1]) + stol(v[1]) + stol(v[2]) + stol(v[3]) +
                 stol(v[4]) + stol(v[5]) + stol(v[6]) + stol(v[7]) +
                 stol(v[8]) + stol(v[9]);
  return jiffies;
}

// Read Number of Active Jiffies for a Process
long LinuxParser::ActiveJiffies(int pid) {
  long jiffies;
  std::ostringstream path;
  path << kProcDirectory << "/" << pid << kStatFilename;
  std::ifstream stream(path.str());
  if (stream.is_open()) {
    string pid, comm, state, ppid, pgrp, session, tty_nr, tpgid, flags, minflt,
        cminflt, majflt, cmajflt;
    long utime, stime, cutime, cstime;
    string line;
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr >>
        tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt >> utime >>
        stime >> cutime >> cstime;
    jiffies = utime + stime + cutime + cstime;
  }
  return jiffies;
}

// Read Number of Active Jiffies for the System
long LinuxParser::ActiveJiffies() {
  vector<string> v = LinuxParser::CpuUtilization();
  long jiffies = stol(v[0]) + stol(v[1]) + stol(v[2]);
  return jiffies;
}

// Read Number of Ifle Jiffies for the System
long LinuxParser::IdleJiffies() {
  vector<string> v = LinuxParser::CpuUtilization();
  long jiffies = stol(v[3]) + stol(v[4]);
  return jiffies;
}

// Read CPU Utilization by Jiffies into Vector
vector<string> LinuxParser::CpuUtilization() {
  string user, nice, system, idle, iowait, irq, softirq, steal, guest,
      guest_nice;
  vector<string> cpu_util;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    string line;
    string cpu;
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cpu >> user >> nice >> system >> idle >> iowait >> irq >>
        softirq >> steal >> guest >> guest_nice;
    cpu_util = {user, nice,    system, idle,  iowait,
                irq,  softirq, steal,  guest, guest_nice};
  }
  return cpu_util;
}

// Read Total Number of Processes
int LinuxParser::TotalProcesses() {
  /* Note: As opposed to the number of forks since boot which was suggested in
  the Knowledge
  Forum, in this Code the total number of processes is defined as the number of
  active
  processes. This is believed to be more representative of the sytem state */
  int total_procs;
  vector<int> v = LinuxParser::Pids();
  total_procs = v.size();
  return total_procs;
}

// Read Number of Running Processes
int LinuxParser::RunningProcesses() {
  int procs_running;
  string line, key, value;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "procs_running") {
        procs_running = stoi(value);
        break;
      }
    }
  }
  return procs_running;
}

// Read Command that started Process by PID
string LinuxParser::Command(int pid) {
  string comm;
  std::ostringstream path;
  path << kProcDirectory << "/" << pid << kCmdlineFilename;
  std::ifstream stream(path.str());
  if (stream.is_open()) {
    std::getline(stream, comm);
  }
  return comm;
}

// Read Memory Consumption of a Process by PID
string LinuxParser::Ram(int pid) {
  string memory="";
  std::ostringstream path;
  path << kProcDirectory << "/" << pid << kStatusFilename;
  std::ifstream stream(path.str());
  if (stream.is_open()) {
    string key, value, line;
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "VmSize:") {
        memory = value;
        break;
      }
    }
  }
  if(memory!=""){
    int memint = stoi(memory);
    memint = memint / 1000;
    memory = to_string(memint);
  }
  return memory;
}

// Read the User ID associated with a Process
string LinuxParser::Uid(int pid) {
  string uid;
  std::ostringstream path;
  path << kProcDirectory << "/" << pid << kStatusFilename;
  std::ifstream stream(path.str());
  if (stream.is_open()) {
    string key, value, line;
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "Uid:") {
        uid = value;
        break;
      }
    }
  }
  return uid;
}

// Read the User associated with a Process
string LinuxParser::User(int pid) {
  string username;
  string userid = LinuxParser::Uid(pid);
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    string name, x, uid, line;
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> name >> x >> uid;
      if (uid == userid) {
        username = name;
        break;
      }
    }
  }
  return username;
}

// Read the Uptime of a Process
long LinuxParser::UpTime(int pid) {
  long uptime;
  long systemjiffies = LinuxParser::Jiffies();
  long processjiffies;
  std::ostringstream path;
  path << kProcDirectory << "/" << pid << kStatFilename;
  std::ifstream stream(path.str());
  if (stream.is_open()) {
    string placeholder, line;
    std::getline(stream, line);
    std::istringstream linestream(line);
    for (int i = 0; i <= 21; i++) {
      if (i == 21) {
        linestream >> processjiffies;
      } else {
        linestream >> placeholder;
      }
    }
  }
  uptime = (systemjiffies - processjiffies) / sysconf(_SC_CLK_TCK);
  return uptime;
}
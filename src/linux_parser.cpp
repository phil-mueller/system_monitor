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
using std::stod;

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
        if (key == filterOSNameString) {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
    filestream.close();
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
    stream.close();
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
double LinuxParser::MemoryUtilization() {
  double utilization;
  double memTotal, memFree, buffers;
  string key, value, ending;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    string line;
    for (int i = 1; i < 5; i++) {
      std::getline(stream, line);
      std::istringstream linestream(line);
      linestream >> key >> value >> ending;
      if (key == filterMemTotalString) {
        memTotal = std::stod(value);
      } else if (key == filterMemFreeString) {
        memFree = std::stod(value);
      } else if (key == filterBufferString) {
        buffers = stod(value);
      }
    }
    stream.close();
  }
  utilization = 1.0 - (memFree / (memTotal - buffers));
  return utilization;
}

// Read System Uptime
long LinuxParser::UpTime() {
  long uptime{0};
  string uptimeString, idletimeString;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptimeString >> idletimeString;
    stream.close();
  }
  uptime = stol(uptimeString);
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
    string pid, comm, state, ppid, pgrp, session, ttynr, tpgid, flags, minflt,
        cminflt, majflt, cmajflt;
    long utime, stime, cutime, cstime;
    string line;
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> pid >> comm >> state >> ppid >> pgrp >> session >> ttynr >>
        tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt >> utime >>
        stime >> cutime >> cstime;
    jiffies = utime + stime + cutime + cstime;
    stream.close();
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
    stream.close();
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
  int totalProcs;
  vector<int> v = LinuxParser::Pids();
  totalProcs = v.size();
  return totalProcs;
}

// Read Number of Running Processes
int LinuxParser::RunningProcesses() {
  int procsRunning;
  string line, key, value;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == filterRunningProcesses) {
        procsRunning = stoi(value);
        break;
      }
    }
    stream.close();
  }
  return procsRunning;
}

// Read Command that started Process by PID
string LinuxParser::Command(int pid) {
  string comm;
  std::ostringstream path;
  path << kProcDirectory << "/" << pid << kCmdlineFilename;
  std::ifstream stream(path.str());
  if (stream.is_open()) {
    std::getline(stream, comm);
    stream.close();
  }
  return comm;
}

// Read Memory Consumption of a Process by PID
string LinuxParser::Ram(int pid) {
  string memory = "";
  std::ostringstream path;
  path << kProcDirectory << "/" << pid << kStatusFilename;
  std::ifstream stream(path.str());
  if (stream.is_open()) {
    string key, value, line;
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == filterProcessMemory) {
        memory = value;
        break;
      }
    }
    stream.close();
  }
  if (memory != "") {
    int memInt = stoi(memory);
    memInt = memInt / 1000;
    memory = to_string(memInt);
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
      if (key == filterUID) {
        uid = value;
        break;
      }
    }
    stream.close();
  }
  return uid;
}

// Read the User associated with a Process
string LinuxParser::User(int pid) {
  string userName;
  string userId = LinuxParser::Uid(pid);
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    string name, x, uid, line;
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> name >> x >> uid;
      if (uid == userId) {
        userName = name;
        break;
      }
    }
    stream.close();
  }
  return userName;
}

// Read the Uptime of a Process
long LinuxParser::UpTime(int pid) {
  long upTime;
  long processJiffies;
  std::ostringstream path;
  path << kProcDirectory << "/" << pid << kStatFilename;
  std::ifstream stream(path.str());
  if (stream.is_open()) {
    string placeholder, line;
    std::getline(stream, line);
    std::istringstream linestream(line);
    for (int i = 0; i <= 21; i++) {
      if (i == 21) {
        linestream >> processJiffies;
      } else {
        linestream >> placeholder;
      }
    }
    stream.close();
  }
  upTime = LinuxParser::UpTime() - (processJiffies / sysconf(_SC_CLK_TCK));
  return upTime;
}
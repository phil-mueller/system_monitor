#include "processor.h"
#include <string>
#include <vector>
#include "linux_parser.h"

using std::stol;

// Return the CPU utilization
float Processor::Utilization() {
  float utilization;
  std::vector<std::string> v = LinuxParser::CpuUtilization();
  // Usage Calculation based on StackOverflow from htop source code
  const long PrevIdle = previdle + prevoiwait;
  const long Idle = stol(v[3]) + stol(v[4]);
  const long PrevNonIdle =
      prevuser + prevnice + prevsystem + previrq + prevsoftirq + prevsteal;
  const long NonIdle = stol(v[0]) + stol(v[1]) + stol(v[2]) + stol(v[5]) +
                       stol(v[6]) + stol(v[7]);
  const long PrevTotal = PrevIdle + PrevNonIdle;
  const long Total = Idle + NonIdle;
  const long totald = Total - PrevTotal;
  const long idled = Idle - PrevIdle;
  utilization = (static_cast<float>(totald) - static_cast<float>(idled)) /
                static_cast<float>(totald);
  // After Calculation, safe current datapoints for next iteration
  prevuser = stol(v[0]);
  prevnice = stol(v[1]);
  previdle = stol(v[3]);
  prevoiwait = stol(v[4]);
  prevsystem = stol(v[2]);
  previrq = stol(v[5]);
  prevsoftirq = stol(v[6]);
  prevsteal = stol(v[7]);
  return utilization;
}
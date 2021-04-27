#include "processor.h"
#include "linux_parser.h"
#include <string>
#include <vector>

using std::stol;

// DONE: Return the aggregate CPU utilization
float Processor::Utilization() {
    float utilization;
    std::vector<std::string> v = LinuxParser::CpuUtilization();
    // Usage Calculation based on StackOverflow from htop source code
    long PrevIdle = previdle + prevoiwait;
    long Idle = stol(v[3]) + stol(v[4]);
    long PrevNonIdle = prevuser + prevnice + prevsystem + previrq + prevsoftirq + prevsteal;
    long NonIdle = stol(v[0])+stol(v[1])+stol(v[2])+stol(v[5])+stol(v[6])+stol(v[7]);
    long PrevTotal = PrevIdle + PrevNonIdle;
    long Total = Idle + NonIdle;
    long totald = Total - PrevTotal;
    long idled = Idle - PrevIdle;
    utilization = (float(totald)-float(idled))/float(totald);
    // After Calculating Usage, safe datapoints for next iteration
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
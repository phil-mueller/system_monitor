#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <string>

class Processor {
 public:
  double Utilization();

 private:
  long previdle = 0;
  long prevoiwait = 0;
  long prevuser = 0;
  long prevnice = 0;
  long prevsystem = 0;
  long previrq = 0;
  long prevsoftirq = 0;
  long prevsteal = 0;
};

#endif
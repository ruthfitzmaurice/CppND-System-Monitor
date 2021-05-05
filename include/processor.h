#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  // DONE: See src/processor.cpp

  // DONE: Declare any necessary private members
 private:
 float old_idle_jiffies = 0.0;           // initialize old CPU Jiffies to 0 for active and idle
 float old_active_jiffies = 0.0;

};

#endif
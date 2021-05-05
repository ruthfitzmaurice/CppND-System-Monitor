#include "processor.h"
#include "linux_parser.h"
#include <iostream>

using namespace LinuxParser;

// DONE: Return the aggregate CPU utilization
// Using the method from here: https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux/23376195#23376195

float Processor::Utilization() { 
    
    float cpu_percentage;
    float active_jiffies = (float)LinuxParser::ActiveJiffies();     //Active jiffies of the system
    float idle_jiffies = (float)LinuxParser::IdleJiffies();         //idle jiffies of the system
    float total = active_jiffies + idle_jiffies;
    float old_total = old_idle_jiffies + old_active_jiffies;       // Values initialised in header

    float total_difference = total - old_total;
    float idle_difference = idle_jiffies - old_idle_jiffies;

    cpu_percentage = (total_difference - idle_difference)/total_difference;

    old_idle_jiffies = idle_jiffies;
    old_active_jiffies = active_jiffies;


    return cpu_percentage; }


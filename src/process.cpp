#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

using namespace LinuxParser;

Process::Process(int pid){                  // Initialiser
    pid_ = pid;
    //command_ = LinuxParser::Command(pid);
    //ram_ = LinuxParser::Ram(pid);
    user_ = LinuxParser::User(pid);
    //uptime_ = LinuxParser::UpTime(pid);
}


// DONE: Return this process's ID
// Return private variable
int Process::Pid() { return pid_; }

// DONE: Return this process's CPU utilization
// Course notes says the CPU utilisation should be calculated as the average utilisation of the process 
// since the process launched, this implies the calculation should be:
// CPUUTIL of the process = active jiffies of the process / uptime of the process 

float Process::CpuUtilization() { 
    float active_jiffies = (LinuxParser::ActiveJiffies(pid_))/(sysconf(_SC_CLK_TCK));   // Now in Seconds not jiffies
    //float cpuutil_ =  (float)active_jiffies/(float)LinuxParser::UpTime(pid_);
    float pid_uptime = LinuxParser::UpTime(pid_);
    float cpuutil_ =  active_jiffies/pid_uptime;
    return cpuutil_;
     }

// DONE: Return the command that generated this process
string Process::Command() { 
 
    return LinuxParser::Command(pid_); }

// DONE: Return this process's memory utilization
string Process::Ram() { 

    
    return LinuxParser::Ram(pid_); }

// DONE: Return the user (name) that generated this process
string Process::User() { 

    return user_;
 }

// DONE: Return the age of this process (in seconds)
long int Process::UpTime() { 
    
    return LinuxParser::UpTime(pid_); }
    //return uptime_; }

// DONE: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const { 
    
    return cpuutil_ > a.cpuutil_;
}

#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <iostream>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;


// DONE: Return the system's CPU
Processor& System::Cpu() { 
    
    return cpu_; 
    
    }

// DONE: Return a container composed of the system's processes
vector<Process>& System::Processes() { 
    processes_.clear();
    vector<int> system_pids = LinuxParser::Pids();  // Calling Pids function from LinuxParser namespace

    for(int i : system_pids){        // Iterating through each pid in system_pids
        Process process(i);             // creating a new process for each
        if (!LinuxParser::Ram(i).empty() || !LinuxParser::Command(i).empty()) {
            processes_.push_back(process); // Add to processes list
        }
    }
    sort(processes_.begin(),processes_.end());

    return processes_; 
    
    }

// DONE: Return the system's kernel identifier (string)
std::string System::Kernel() { 
    
    return LinuxParser::Kernel();     // Calling Kernel function from LinuxParser namespace
    
    }

// DONE: Return the system's memory utilization
float System::MemoryUtilization() { 
    return LinuxParser::MemoryUtilization();     // Calling MemoryUtilization function from LinuxParser namespace
    
    }

// DONE: Return the operating system name
std::string System::OperatingSystem() { 
    
    return LinuxParser::OperatingSystem();

     }

// DONE: Return the number of processes actively running on the system
int System::RunningProcesses() { 
    
    return LinuxParser::RunningProcesses();
 }

// DONE: Return the total number of processes on the system
int System::TotalProcesses() { 

    return LinuxParser::TotalProcesses();
 }

// DONE: Return the number of seconds since the system started running
long int System::UpTime() {

    return LinuxParser::UpTime();
 }
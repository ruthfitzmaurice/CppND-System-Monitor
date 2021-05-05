#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <cctype>
#include <algorithm>

#include "linux_parser.h"

using std::stof;
using std::stoi;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);                      // kOSPath = /etc/os-release from linuxparser.h
  if (filestream.is_open()) {                             // Open the file
    while (std::getline(filestream, line)) {              // as long as there is a line, keep reading
      std::replace(line.begin(), line.end(), ' ', '_');   // remove any odd formating from each line in file (replace with empyty)
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);               // Start a string stream
      while (linestream >> key >> value) {               // Chunk off values : key value
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');   // Reformat empty values in string
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
// Command that gives info about kernel (and more) cat /proc/version
// Linux version 4.15.0-1080-gcp ... last section is kernel 
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);     // Input file stream /proc/version
  if (stream.is_open()) {                                      // Try to open the file stream
    std::getline(stream, line);                                // Read the entire line
    std::istringstream linestream(line);                       // Start a string stream to segment the line          
    linestream >> os >> version >> kernel;                     // We know that the 1st element = 'Linux', 2nd = 'version', 3rd = 'kernel'
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
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

// DONE: Read and return the system memory utilization
// Using formula from here: https://knowledge.udacity.com/questions/285770

float LinuxParser::MemoryUtilization() { 
  
  string line;
  string key;
  string value_in_kb;
  float mem_free;
  float mem_total;
  float mem_util_percent;

  std::ifstream filestream(kProcDirectory + kMeminfoFilename);        // Input file stream /proc/meminfo
  if (filestream.is_open()) {                                         // Open the file
    while (std::getline(filestream, line)) {                          // as long as there is a line, keep reading
      std::istringstream linestream(line);                            // Start a string stream
      while (linestream >> key >> value_in_kb) {                      // Chunk off values : key value_in_kb
        if (key == "MemTotal:") {                                     // Assign the correct variables values
          mem_total = stof(value_in_kb);  
        }
        if (key == "MemFree:") {
          mem_free = stof(value_in_kb);          
        }
      }
    }
    mem_util_percent = (mem_total - mem_free )/mem_total;

    
    return mem_util_percent;
  }
  return 0.0; 
  }

// DONE: Read and return the system uptime
// Using as reference: https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux/6/html/deployment_guide/s2-proc-uptime
// The first value logged is the time in seconds the system has been up
// The second value logged is the sum of how much time each core has been idel 
long LinuxParser::UpTime() { 
  long uptime;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);      // Input file stream /proc/uptime
  if (stream.is_open()) {                                      // Try to open the file stream
    std::getline(stream, line);                                // Read the entire line
    std::istringstream linestream(line);                       // Start a string stream to segment the line          
    linestream >> uptime;                                      // We know that the 1st element = 'Uptime' and dont care about the rest
    return uptime;
    }
  return 0;
  }


// DONE: Read and return the number of jiffies for the system
// Using as reference: https://askubuntu.com/questions/737079/monitoring-cpu-time-with-proc-stat
// Total number of jiffies is sum of 7 values in the first line
// For testing, total jiffies = 96193 + 8010 + 26406 + 595478 + 2213 + 0 + 847 + 0 + 0 + 0 = 729147
long LinuxParser::Jiffies() { 
  long total_jiffies;
  string line, key;
  long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;                // In order printed
  std::ifstream filestream(kProcDirectory + kStatFilename);           // Input file stream /proc/stat
  if (filestream.is_open()) {                                         // Open the file
    while (std::getline(filestream, line)) {                          // as long as there is a line, keep reading
      std::istringstream linestream(line);                            // Start a string stream
      while (linestream >> key) {                                     // Chunk off values : key value_in_kb
        if (key == "cpu") {                                          // Assign the correct variables values
              linestream >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;                      // Chunk off values : key value_in_kb
              total_jiffies = user + nice + system + idle + iowait + irq + softirq + steal + guest + guest_nice;
              return total_jiffies;
        }
      }
    }
  }
  return 0; 
}

// DONE: Read and return the number of active jiffies for a PID

long LinuxParser::ActiveJiffies(int pid) {
  long active_jiffies = 0;
  string line, element;
  vector<string> pid_jiffies_container;

  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);           // Input file stream /proc/stat
  if (filestream.is_open()) {                                         // Open the file
    while (std::getline(filestream, line)) {                          // as long as there is a line, keep reading
      std::istringstream linestream(line);                            // Start a string stream
      while (linestream >> element){
        pid_jiffies_container.push_back(element);
      }
    }
  }

  //active_jiffies = long(stoi(pid_jiffies_container[14]) + stoi(pid_jiffies_container[15]) + stoi(pid_jiffies_container[16]) + stoi(pid_jiffies_container[17]));
  active_jiffies = long(stoi(pid_jiffies_container[13]) + stoi(pid_jiffies_container[14]) + stoi(pid_jiffies_container[15]) + stoi(pid_jiffies_container[16]));
  return active_jiffies; 
  }



// function to return the fill list of CPU jiffies
// /proc/stat commands returns info on all CPUS plus an aggregate CPU value
// This is the first line 'cpu' which we deal with
// The CpuJiffies function will return a list of all values from the CPU line
vector<long> LinuxParser::CpuUtilization() {
  vector<long> active_jiffies_list(10);
  string line, key;
  long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;                // In order printed
  std::ifstream filestream(kProcDirectory + kStatFilename);           
  if (filestream.is_open()) {                                         
    while (std::getline(filestream, line)) {                          
      std::istringstream linestream(line);                            
      while (linestream >> key){
        if (key == "cpu") {                                          // Assign the correct variables values
              linestream >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;                      // Chunk off values : key value_in_kb
              active_jiffies_list.emplace_back(user);
              active_jiffies_list.emplace_back(nice);
              active_jiffies_list.emplace_back(system);
              active_jiffies_list.emplace_back(idle);
              active_jiffies_list.emplace_back(iowait);
              active_jiffies_list.emplace_back(irq);
              active_jiffies_list.emplace_back(softirq);
              active_jiffies_list.emplace_back(steal);
              active_jiffies_list.emplace_back(guest);
              active_jiffies_list.emplace_back(guest_nice);    
        }
      }
    }
  }
  return active_jiffies_list; 
  }

// DONE: Read and return the number of active jiffies for the system
/*
We know from previous Jiffies() method: 
The output of command cat /proc/stat gives values for the system (first line called cpu)
There are 10 output values for 'cpu', in order left to right they are: 
user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice.
We can break these attributes into two type : idle and non idle.
Idle jiffies = idle + iowait
Non idle jiffies (ie active jiffies) = user + nice + system.

*/

long LinuxParser::ActiveJiffies() { 

  long active_jiffies = 0;
  vector<long> all_jiffies = CpuUtilization();                // Returns a list of all the cpu jiffie values for the system
  int s = all_jiffies.size();                             // s is the length of the list, needed for indexing specific jiffies
  active_jiffies = all_jiffies[s-10] + all_jiffies[s-9] + all_jiffies[s-8];    // We know the active jiffies from comments above function def
                                                                              // Since emplace_back was used, user is actually last element in list  
  return active_jiffies; 
}
  


// DONE: Read and return the number of idle jiffies for the system
// Using logic defined in ActiveJiffies comments
// Remembering: idle jiffies = idle + iowait
long LinuxParser::IdleJiffies() {
                                                                            
  long idle_jiffies = 0;
  vector<long> all_jiffies = CpuUtilization();                // Returns a list of all the cpu jiffie values for the system
  int s = all_jiffies.size();                             // s is the length of the list, needed for indexing specific jiffies
  idle_jiffies = all_jiffies[s-6] + all_jiffies[s-7];    // We know the active jiffies from comments above function def
                                                                              // Since emplace_back was used, user is actually last element in list  

  return idle_jiffies; 
}


// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 

  string line, key;
  int process_number;
  std::ifstream filestream(kProcDirectory + kStatFilename);           
  if (filestream.is_open()) {                                         
    while (std::getline(filestream, line)) {                          
      std::istringstream linestream(line);                            
      while (linestream >> key){
        if (key == "processes") {                                          // Assign the correct variables values
              linestream >> process_number;
              return process_number;
        }
      }
    }
  }
return 0;
 }


// DONE: Read and return the number of running processes

int LinuxParser::RunningProcesses() { 
  string line, key;
  int proc_running;
  std::ifstream filestream(kProcDirectory + kStatFilename);           
  if (filestream.is_open()) {                                         
    while (std::getline(filestream, line)) {                          
      std::istringstream linestream(line);                            
      while (linestream >> key){
        if (key == "procs_running") {                                          // Assign the correct variables values
              linestream >> proc_running;
              return proc_running;
        }
      }
    }
  }
  return 0;
  }
 
// DONE: Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
  string line, cmd;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);          
  if (filestream.is_open()) {                                         // Open the file
    while (std::getline(filestream, line)) {                          // as long as there is a line, keep reading
      std::istringstream linestream(line);                            // Start a string stream
      linestream >> cmd;
      return cmd;
    }
  }
  return string();
  }



// DONE: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 
  string line, key;
  string Ram;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);           
  if (filestream.is_open()) {                                         
    while (std::getline(filestream, line)) {                          
      std::istringstream linestream(line);                            
      while (linestream >> key){
        if (key == "VmSize:") {                                          // Assign the correct variables values
              linestream >> Ram;
              float ram_in_mb = stof(Ram)/1000;                           // Change units to MB
              return to_string(ram_in_mb);
        }
      }
    }
  }
  return string(); 
  
  }

// DONE: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) { 
  string line, key;
  string Uid;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);           
  if (filestream.is_open()) {                                         
    while (std::getline(filestream, line)) {                          
      std::istringstream linestream(line);                            
      while (linestream >> key){
        if (key == "Uid:") {                                          // Assign the correct variables values
              linestream >> Uid;
              return Uid;
        }
      }
    }
  }
  return string(); 
  }

// DONE: Read and return the user associated with a process
// From course notes (13. Process Data), the output of cat /etc/passwd which we care about is:
// david: x: 1000 : 1000: David Silver ,,, : /home/david: bin/bash 
// Where : User = david, x = x, UID = 1000
// Use the same linestream chunks for function
string LinuxParser::User(int pid) { 
  string line, key;
  string user, x, u_id;
  std::ifstream filestream(kPasswordPath);  
  if (filestream.is_open()) {                                         
    while (std::getline(filestream, line)) { 
        std::replace(line.begin(), line.end(), ':', ' ');   // remove any odd formating from each line in file (replace with whitespace)
        std::istringstream linestream(line);               // Start a string stream (string now has spaces)
        linestream >> user >> x >> u_id;
        if (u_id == Uid(pid)){
          return user;
        }
    }
  }
  return string();
 }

// DONE: Read and return the uptime of a process
// Starttime of a process: >> cat /proc/<pid>/stat and is the 22nd value 
// Value is in clock ticks so needs to be divided by sysconf(_SC_CLK_TCK)
// To get the uptime of a PID you need to subtract PID start time from Uptime of the system 
// From a previously answered question: https://knowledge.udacity.com/questions/155622


long LinuxParser::UpTime(int pid) {
  long pid_start_time;
  string line, start_time;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);  
  if (filestream.is_open()) {                                         
    while (std::getline(filestream, line)) {                          
      std::istringstream linestream(line);                            // string stream is separated by spaces                       
      int counter = 0;                                                // Counter will keep track of how many values we have chunked off
      while (linestream >> start_time){
        counter = counter + 1;                                        // update counter by 1 everytime we chunk off a value from linestream 
        if (counter == 22) {                                           // We know that start time is the 22nd value
          pid_start_time = stol(start_time)/sysconf(_SC_CLK_TCK);
          return (UpTime() - pid_start_time);
        }
      }
    }
  }
  return 0;
}

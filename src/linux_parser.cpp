#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <unistd.h>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
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

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
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

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  string key, value, unit;
  string line;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  float totalMemory = 0.0;
  float freeMemory = 0.0;
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value >> unit;
      if (key == "MemTotal:")
        totalMemory = stof(value);
      if (key == "MemFree:")
        freeMemory = stof(value);
    }
    return totalMemory - freeMemory;
  }
  return 0.0; 
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  string timeSpend;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::replace(line.begin(), line.end(), '.', ' ');
    std::istringstream linestream(line);
    linestream >> timeSpend;
    //std::cout << timeSpend << "\n";
  }
  return stol(timeSpend);
  //return 0;
 }

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies(); }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  long user, nice, system, irq, softirq, steal;
  std::string line, str;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    for (int count = 0; count <= 8; count++){
        linestream >> str;
        if(count == 1) user = stol(str);
        if(count == 2) nice = stol(str);
        if(count == 3) system = stol(str);
        if(count == 6) irq = stol(str);
        if(count == 7) softirq = stol(str);
        if(count == 8) steal = stol(str);
    }
    //std::cout << "active" << user + nice + system + irq + softirq + steal << "\n";
    return user + nice + system + irq + softirq + steal;
  }
  return 0; 
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {   
  long idle, iowait;
  std::string line, str;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    for (int count = 0; count <= 5; count++){
        linestream >> str;
        if(count == 4) idle = stol(str);
        if(count == 5) iowait = stol(str);
    }
    //std::cout << "idle" << idle + iowait << "\n";
    return idle + iowait;
  }
  return 0; 
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization(int pid) { 
  std::string line, key;
  std::vector<string> strVect;
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> key){
     // std::cout << key << "\n";
      strVect.push_back(key);
    }
  }
  return strVect; 
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string key, value;
  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "processes")
        return stoi(value);
     }
  }
  return 0; 
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string key, value;
  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "procs_running")
        return stoi(value);
     }
  }
  return 0; 
}
// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid[[maybe_unused]]) {   
  std::string line, value;
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    std::replace(line.begin(), line.end(), '-', ' ');
    linestream >> value;
    return value;
    
  }
  return string(); 
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid[[maybe_unused]]) { 
  std::string line, key, value;
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "VmSize:"){
        return value;
      }
    }
  }  
  return string(); }

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  std::string line, key, value;
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "Uid:"){
        return value;
      }
    }
  }
  return string(); 
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
std::string line(""), x, uid;
std::string name("");
std::ifstream stream(kPasswordPath);
if (stream.is_open()) {
  while (std::getline(stream, line)) {
    std::replace(line.begin(), line.end(), ':', ' ');
    std::istringstream linestream(line);
    linestream >> name >> x >> uid;
    if (uid == Uid(pid)){
      return name;
    }
  }
}
  return string(); 
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
  std::string line, value;
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    for (int i=0; i <= 21; i++){ 
      linestream >> value; 
    }
    return stol(value)/sysconf(_SC_CLK_TCK);
  }
  return 0; 
}
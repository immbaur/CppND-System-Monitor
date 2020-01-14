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
  std::string line;
  std::string key;
  std::string value;
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
          filestream.close();
          return value;
        }
      }
    }
  }
  return string();
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  std::string os, version, kernel;
  std::string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
    stream.close();
    return kernel;
  }
  return string();
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

// Done: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  std::string key;
  std::string value;
  std::string unit;
  std::string line;
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
    stream.close();
    return freeMemory/totalMemory;
  }
  return 0.0; 
}

// Reads and returns the system uptime
long LinuxParser::UpTime() { 
  std::string timeSpend;
  std::string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::replace(line.begin(), line.end(), '.', ' ');
    std::istringstream linestream(line);
    linestream >> timeSpend;
    //std::cout << timeSpend << "\n";
    stream.close();
  }
  return stol(timeSpend);
  //return 0;
 }

// Reads and returns the number of jiffies for the system
long LinuxParser::Jiffies() { return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies(); }

//Reads and returns the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

//Reads and returns the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  long user; 
  long nice;
  long system; 
  long irq;
  long softirq; 
  long steal;
  std::string line; 
  std::string str;
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
    stream.close();
    return user + nice + system + irq + softirq + steal;
  }
  return 0; 
}

//Reads and returns the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {   
  long idle; 
  long iowait;
  std::string line;
  std::string str;
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
    stream.close();
    return idle + iowait;
  }
  return 0; 
}

//Reads and returns CPU utilization
vector<string> LinuxParser::CpuUtilization(int pid) { 
  std::string line; 
  std::string key;
  std::vector<string> strVect;
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> key){
     // std::cout << key << "\n";
      strVect.push_back(key);
    }
    stream.close();
    return strVect;
  }
  return std::vector<string>(); 
}

//Reads and returns the total number of processes
int LinuxParser::TotalProcesses() { 
  std::string key;
  std::string value;
  std::string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "processes"){
        stream.close();
        return stoi(value);
      }
    }
    stream.close();
  }
  return 0; 
}

//Reads and returns the number of running processes
int LinuxParser::RunningProcesses() { 
  std::string key;
  std::string value;
  std::string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "procs_running"){
        stream.close();
        return stoi(value);
      }
    }
    stream.close();
  }
  return 0; 
}
//Reads and returns the command associated with a process
string LinuxParser::Command(int pid[[maybe_unused]]) {   
  std::string line; 
  std::string value;
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    std::replace(line.begin(), line.end(), '-', ' ');
    linestream >> value;
    stream.close();
    return value;
    
  }
  return string(); 
}

//Reads and returns the memory used by a process
string LinuxParser::Ram(int pid[[maybe_unused]]) { 
  std::string line; 
  std::string key; 
  std::string value;
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "VmData:"){
        stream.close();
        return to_string(stoi(value)/1024);
      }
    }
    stream.close();
  }  
  return string(); }

//Reads and returns the user ID associated with a process
string LinuxParser::Uid(int pid) { 
  std::string line; 
  std::string key; 
  std::string value;
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "Uid:"){
        stream.close();
        return value;
      }
    }
    stream.close();
  }
  return string(); 
}

//Reads and returns the user associated with a process
string LinuxParser::User(int pid) { 
  std::string line; 
  std::string x; 
  std::string uid;
  std::string name;
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> name >> x >> uid;
      if (uid == Uid(pid)){
        stream.close();
        return name;
      }
    }
    stream.close();
  }
  return string(); 
}

// Reads and returns the uptime of a process
long LinuxParser::UpTime(int pid) { 
  std::string line;
  std::string value;
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    for (int i=0; i <= 21; i++){ 
      linestream >> value; 
    }
    stream.close();
    return stol(value)/sysconf(_SC_CLK_TCK);
  }
  return 0; 
}
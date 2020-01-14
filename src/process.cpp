#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <unistd.h>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// Done: Return this process's ID
int Process::Pid() const { return pid_; }

Process::Process(int pid) {
    pid_ = pid;

    // calculate cpu
    int utime, stime, cutime, cstime, total_time;
    float seconds;
    
    vector<string> strVec = LinuxParser::CpuUtilization(pid);
    int count = 0;
    for (string str : strVec){
        count++;
        //std::cout << str << "\n";
        if(count == 14) utime = stoi(str);
        if(count == 15) stime = stoi(str);
        if(count == 16) cutime = stoi(str);
        if(count == 17) cstime = stoi(str);
    }
    total_time = (utime + stime + cutime + cstime) / sysconf(_SC_CLK_TCK); 
    seconds = LinuxParser::UpTime() - LinuxParser::UpTime(pid);
  //  if (seconds < 0.001){
  //    seconds = 0.001;
  //  }   
    //std::cout << 100 * ((total_time/100)/seconds) << "\n";
    cpu_ = ((total_time)/seconds);
}

// Done: Return this process's CPU utilization
float Process::CpuUtilization() const { 
    
    return cpu_; 
}

// Done: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(Pid()); }

// Done: Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(Pid()); }

// Done: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(Pid()); }

// Done: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(Pid()); }

// Done: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const { 
    return CpuUtilization() < a.CpuUtilization(); 
}

bool Process::operator>(Process const& a) const { 
    return CpuUtilization() > a.CpuUtilization(); 
}
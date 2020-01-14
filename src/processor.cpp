#include "processor.h"
#include "linux_parser.h"
#include <iostream>
// Done: Return the aggregate CPU utilization
float Processor::Utilization() { 
    //std::cout << "CPU util: " << LinuxParser::ActiveJiffies() / LinuxParser::Jiffies();
    return float(LinuxParser::ActiveJiffies()) / float(LinuxParser::Jiffies()); 
}
#include <string>
#include <sstream>
#include <iomanip>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
    
    long sec = seconds % 60;
    long hours = seconds /3600;
    long minutes = (seconds % 3600) / 60;

    std::stringstream buffer;
    buffer << std::setw(2) << std::setfill('0') << hours << ":" << std::setw(2) << std::setfill('0') << minutes << ":" << std::setw(2) << std::setfill('0') << sec;
    return buffer.str(); 

}
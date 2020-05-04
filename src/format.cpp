#include <string>
#include <sstream>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
     long time = seconds;
    std::stringstream ss;

    long h = time/3600;
    time = time%3600;
    long m = time/60;
    time = time%60;
    long s = time;

    ss << h << ":" << m << ":" << s;
    return ss.str();
}
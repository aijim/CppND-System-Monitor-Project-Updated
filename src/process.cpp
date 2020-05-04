#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <unistd.h>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid)
{
  pid_ = pid;
  user_ = LinuxParser::User(pid);
  command_ = LinuxParser::Command(pid);
}

// TODO: Return this process's ID
int Process::Pid() { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() const 
{
    long uptime = LinuxParser::UpTime();
    long starttime = LinuxParser::UpTime(pid_);
    long seconds = uptime - starttime;

    long active_jiffies = LinuxParser::ActiveJiffies(pid_);

    return (float)active_jiffies/sysconf(_SC_CLK_TCK)/seconds;
}

// TODO: Return the command that generated this process
string Process::Command() { return command_; }

// TODO: Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(pid_); }

// TODO: Return the user (name) that generated this process
string Process::User() { return user_; }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const { return (*this).CpuUtilization() < a.CpuUtilization(); }
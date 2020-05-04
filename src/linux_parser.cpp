#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <fstream>
#include <unistd.h>
#include <iostream>

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
float LinuxParser::MemoryUtilization() 
{
  int memTotal = 0;
  int memFree = 0;
  std::string key;
  float value = 0; 
  std::string line;

  std::ifstream fileStream(kProcDirectory + kMeminfoFilename);
  if(fileStream.is_open())
  {
    while(std::getline(fileStream, line))
    {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if(key == "MemTotal:")
      {
        memTotal = value;
      }else if(key == "MemFree:")
      {
        memFree = value;
      }
    }
  }
  if(memTotal == 0)
  {
    return 0.0;
  }else{
    return float(memTotal-memFree)/memTotal;
  } 
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  long uptime = 0;
  std::string line;

  std::ifstream fileStream(kProcDirectory + kUptimeFilename);
  if(fileStream.is_open())
  {
    std::getline(fileStream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  return uptime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  std::string line;
  std::string cpu;
  long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
  long jiffies = 0;

  std::ifstream fileStream(kProcDirectory + kStatFilename);
  if(fileStream.is_open())
  {
    std::getline(fileStream,line);
    std::istringstream linestream(line);
    linestream >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;

    jiffies = user + nice + system + idle + iowait + irq + softirq + steal;
  }

  return jiffies;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) 
{
  std::string line,col;
  long utime,stime,cutime,cstime;
  std::string starttime;
  
  std::ifstream fileStream(kProcDirectory + to_string(pid) + kStatFilename);
  if(fileStream.is_open())
  {
    getline(fileStream,line);
    std::stringstream linestream(line);
    for(int i=0; i<13; i++)
    {
      linestream >> col;
    }
    linestream >> utime >> stime >> cutime >> cstime;
    return utime + stime + cutime + cstime;
  }

  return 0;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  std::string line;
  std::string cpu;
  long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
  long active_jiffies = 0;

  std::ifstream fileStream(kProcDirectory + kStatFilename);
  if(fileStream.is_open())
  {
    std::getline(fileStream,line);
    std::istringstream linestream(line);
    linestream >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;

    active_jiffies = user + nice + system + irq + softirq + steal;
  }

  return active_jiffies;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  std::string line;
  std::string cpu;
  long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
  long idle_jiffies = 0;

  std::ifstream fileStream(kProcDirectory + kStatFilename);
  if(fileStream.is_open())
  {
    std::getline(fileStream,line);
    std::istringstream linestream(line);
    linestream >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;

    idle_jiffies = idle + iowait;
  }

  return idle_jiffies;
}

// TODO: Read and return CPU utilization
float LinuxParser::CpuUtilization() 
{
  float utilization;
  static long pre_jiffies = 0;
  static long pre_active_jiffies = 0;
  
  long jiffies = Jiffies();
  long active_jiffies = ActiveJiffies();

  if(jiffies-pre_jiffies != 0)
  {
    utilization = (float)(active_jiffies-pre_active_jiffies)/(jiffies-pre_jiffies);
  }else{
    utilization = 0.0;
  }

  pre_jiffies = jiffies;
  pre_active_jiffies = active_jiffies;

  return utilization;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  std::string line;
  int num = 0;

  std::ifstream fileStream(kProcDirectory + kStatFilename);
  if(fileStream.is_open())
  {
    while(std::getline(fileStream, line))
    {
      std::string col1;
      std::istringstream linestream(line);
      linestream >> col1;
      if(col1 == "processes")
      {
        linestream >> num;
        break;
      }
    }
  }
  return num;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  std::string line;
  int num = 0;

  std::ifstream fileStream(kProcDirectory + kStatFilename);
  if(fileStream.is_open())
  {
    while(std::getline(fileStream, line))
    {
      std::string col1;
      std::istringstream linestream(line);
      linestream >> col1;
      if(col1 == "procs_running")
      {
        linestream >> num;
        break;
      }
    }
  }
  return num;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) 
{
  std::string line;
  
  std::ifstream fileStream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if(fileStream.is_open())
  {
    getline(fileStream,line);
  }

  return line;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) 
{
  std::string line;
  std::string col1;
  long col2;
  
  std::ifstream fileStream(kProcDirectory + to_string(pid) + kStatusFilename);
  if(fileStream.is_open())
  {
    while(getline(fileStream,line))
    {
      std::stringstream linestream(line);
      linestream >> col1;
      if(col1 == "VmSize:")
      {
        linestream >> col2;
        break;
      }
    }
  }
  return to_string(col2/1000);
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) 
{
  std::string line;
  std::string col1, col2;
  
  std::ifstream fileStream(kProcDirectory + to_string(pid) + kStatusFilename);
  if(fileStream.is_open())
  {
    while(getline(fileStream,line))
    {
      std::stringstream linestream(line);
      linestream >> col1;
      if(col1 == "Uid:")
      {
        linestream >> col2;
        break;
      }
    }
  }

  return col2;
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) 
{
  std::string line;
  std::string col1, col2, col3;
  std::string user;

  std::string uid = Uid(pid);
  
  std::ifstream fileStream(kPasswordPath);
  if(fileStream.is_open())
  {
    while(getline(fileStream,line))
    {
      std::replace(line.begin(),line.end(),':',' ');
      std::stringstream linestream(line);
      linestream >> col1 >> col2 >> col3;
      if(col3 == uid)
      {
        user = col1;
        break;
      }
    }
  }

  return user;
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) 
{
  std::string line;
  std::string col;
  long starttime;
  
  std::ifstream fileStream(kProcDirectory + to_string(pid) + kStatFilename);
  if(fileStream.is_open())
  {
    getline(fileStream,line);
    std::stringstream linestream(line);
    for(int i=0; i<21; i++)
    {
      linestream>> col;
    }
    linestream >> starttime;
  }

  return starttime/sysconf(_SC_CLK_TCK);
}
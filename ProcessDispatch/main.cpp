//
//  main.cpp
//  ProcessDispatch
//
//  Created by Nero Zuo on 16/11/9.
//  Copyright © 2016年 Nero. All rights reserved.
//

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <fstream>
#define PROCESS_NAME_LEN 32


#include <chrono>
#include <thread>

using namespace std;

class Process {
public:
    string name;
    int arrive;
    int duration;
    int priority;
    int waitingTime;
    int turnoverTime;
    
    Process(vector<string> processInfo) {
        name = processInfo[0];
        arrive = stoi(processInfo[1]);
        duration = stoi(processInfo[2]);
        priority = stoi(processInfo[3]);
        waitingTime = 0;
        turnoverTime = 0;
    }
};

bool operator==(Process const &p1 ,Process const & p2) {
    return p1.name == p2.name;
}


bool sjfCpmpare(const Process &p1, const Process &p2) {
    return p1.duration < p2.duration;
}

bool sjfCpmparePoint(const Process *p1, const Process *p2) {
    return p1->duration < p2->duration;
}


bool roundRobinComparePoint(const Process *p1, const Process *p2) {
    return p1->arrive < p2->arrive;
}

bool priorityComparePoint(const Process *p1, const Process *p2) {
    return p1->priority > p2->priority;
}


class ProcessesManager {
private:
    
    int timeNow;
public:
    vector<Process> processes;
    
    vector<Process> finishedProcesses;
    ProcessesManager(vector<Process> processes) {
        this->processes = processes;
        this->finishedProcesses = {};
        timeNow = 0;
    }
    
    void run(Process *process, int duration) {
        if (process->duration - duration < 0) {
            duration = process->duration;
        }
        process->duration = process->duration - duration;
        process->waitingTime += (timeNow-process->arrive);
        process->turnoverTime += (timeNow-process->arrive + duration);
        timeNow += duration;
        process->arrive = timeNow;
        Process p = *process;
        if(process->duration <= 0) {
            finishedProcesses.push_back(p);
            remove(processes.begin(), processes.end(), *process);
            processes.pop_back();
        }
        cout << "run " + p.name + " form " + to_string(timeNow-duration) + " to " + to_string(timeNow) + " for " + to_string(duration) << endl;
    }
    
    vector<Process *> rightNowProcesses() {
        vector<Process *> rightNowProcess;
        for (std::vector<Process>::size_type iter = 0; iter!=processes.size(); iter++) {
            
            if (processes[iter].arrive <= timeNow) {
                rightNowProcess.push_back(&processes[iter]);
            }
        }
        
        return rightNowProcess;
    }
    
    
    void sjf() {
        while (processes.size() != 0) {
            vector<Process *> toRunProcess = rightNowProcesses();
            if (toRunProcess.size() == 0) {
                timeNow += 1;
                continue;
            }
            sort(toRunProcess.begin(), toRunProcess.end(), sjfCpmparePoint);
            run(toRunProcess[0], toRunProcess[0]->duration);
        }
        for (std::vector<Process>::size_type iter = 0; iter!=finishedProcesses.size(); iter++) {
            Process process = finishedProcesses[iter];
            cout << process.name + " waiting time " + to_string(process.waitingTime) + "| turnover time " + to_string(process.turnoverTime) << endl;
        }
    }
    
    void roundRobin() {
        vector<int> timeSlice = {1,2,3};
        int timeSliceIter = 0;
        while (processes.size() != 0) {
            vector<Process *> toRunProcess = rightNowProcesses();
            int timeToRun = timeSlice[timeSliceIter];
//            this_thread::sleep_for(std::chrono::milliseconds(timeToRun*1000));
            timeSliceIter = (timeSliceIter + 1)%timeSlice.size();
            if (toRunProcess.size() == 0) {
                timeNow += 1;
                continue;
            }
            sort(toRunProcess.begin(), toRunProcess.end(), roundRobinComparePoint);
            run(toRunProcess[0], timeToRun);
        }
        for (std::vector<Process>::size_type iter = 0; iter!=finishedProcesses.size(); iter++) {
            Process process = finishedProcesses[iter];
            cout << process.name + " waiting time " + to_string(process.waitingTime) + "| turnover time " + to_string(process.turnoverTime) << endl;
        }
    }
    
    void highPriorityFirst() {
        while (processes.size() != 0) {
            vector<Process *> toRunProcess = rightNowProcesses();
            if (toRunProcess.size() == 0) {
                timeNow += 1;
                continue;
            }
            sort(toRunProcess.begin(), toRunProcess.end(), priorityComparePoint);
            run(toRunProcess[0], toRunProcess[0]->duration);
        }
        for (std::vector<Process>::size_type iter = 0; iter!=finishedProcesses.size(); iter++) {
            Process process = finishedProcesses[iter];
            cout << process.name + " waiting time " + to_string(process.waitingTime) + "| turnover time " + to_string(process.turnoverTime) << endl;
        }
    }
    
};


void splitString(const std::string& s, std::vector<std::string>& v, const std::string& c)
{
    std::string::size_type pos1, pos2;
    pos2 = s.find(c);
    pos1 = 0;
    while(std::string::npos != pos2)
    {
        v.push_back(s.substr(pos1, pos2-pos1));
        
        pos1 = pos2 + c.size();
        pos2 = s.find(c, pos1);
    }
    if(pos1 != s.length())
        v.push_back(s.substr(pos1));
}


vector<Process> readFile() {
    ifstream file;
    string line;
    vector<Process> processes = {};
    file.open("/Users/Nero/Desktop/process.txt");
    if (file.is_open()) {
        while (getline(file, line)) {
            vector<string> processInfo;
            splitString(line, processInfo, ",");
            Process process = Process(processInfo);
            processes.push_back(process);
        }
    }
    file.close();
    return processes;
}

int main(int argc, const char * argv[]) {
    
//    vector<Process> processes = {{"p1", 0, 7, 5, 0, 0}, {"p2", 1, 1, 1, 0, 0}, {"p3", 1, 3, 4, 0 ,0}, {"p4", 2, 5, 3, 0, 0}, {"p5", 4, 4, 2, 0, 0}};
    
    vector<std::string> str;
    splitString("h\ne\nl\nl\no", str, "\n");
    
    vector<Process> processes = readFile();
    ProcessesManager manager = *new ProcessesManager(processes);
    manager.highPriorityFirst();
    return 0;
}

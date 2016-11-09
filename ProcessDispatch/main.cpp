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
//        cout << "run "  + " form " + to_string(timeNow-duration) + " to " + to_string(timeNow) + " for " + to_string(duration) << endl;
        
        cout << "-" + to_string(timeNow-duration) + " ";
        for (int i = 0; i<duration; i++) {
            cout <<  "-";
        }
        cout << p.name << ":" + to_string(duration);
        for (int i = 0; i<duration; i++) {
            cout <<  "-";
        }
        cout << " " + to_string(timeNow) + "-|";
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
        printResult();
    }
    
    void roundRobin() {
        vector<int> timeSlice = {1,2,3};
        int timeSliceIter = 0;
        while (processes.size() != 0) {
            vector<Process *> toRunProcess = rightNowProcesses();
            int timeToRun = timeSlice[timeSliceIter];
            timeSliceIter = (timeSliceIter + 1)%timeSlice.size();
            if (toRunProcess.size() == 0) {
                timeNow += 1;
                continue;
            }
            sort(toRunProcess.begin(), toRunProcess.end(), roundRobinComparePoint);
            run(toRunProcess[0], timeToRun);
        }
        printResult();
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
        
        printResult();
    }
    
    void printResult() {
        cout << endl;
        int totoalTurnoverTime = 0;
        for (std::vector<Process>::size_type iter = 0; iter!=finishedProcesses.size(); iter++) {
            Process process = finishedProcesses[iter];
            cout << process.name + " waiting time " + to_string(process.waitingTime) + "| turnover time " + to_string(process.turnoverTime) << endl;
            totoalTurnoverTime += process.turnoverTime;
        }
        cout << "average turnover time: " + to_string(totoalTurnoverTime/finishedProcesses.size()) << endl;
        cout << "----------------------------------" << endl;
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


vector<Process> readFile(string filepath) {
    ifstream file;
    string line;
    vector<Process> processes = {};
    file.open(filepath);
    if (file.is_open()) {
        while (getline(file, line)) {
            vector<string> processInfo;
            splitString(line, processInfo, ",");
            Process process = Process(processInfo);
            processes.push_back(process);
        }
    }else {
        cout << "Open file failed" << endl;
        exit(EXIT_FAILURE);
    }
    file.close();
    return processes;
}

int main(int argc, const char * argv[]) {
    
    string filePahth;
    cout << "Please input file path:" << endl;
    cin >> filePahth;
    
    vector<Process> processes = readFile(filePahth);
    ProcessesManager manager0 = *new ProcessesManager(processes);
    manager0.sjf();
    
    ProcessesManager manager1 = *new ProcessesManager(processes);
    manager1.roundRobin();
    
    ProcessesManager manager2 = *new ProcessesManager(processes);
    manager2.highPriorityFirst();
    return 0;
}

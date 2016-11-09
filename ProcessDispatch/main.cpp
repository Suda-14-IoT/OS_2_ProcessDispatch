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

int main(int argc, const char * argv[]) {
    
    vector<Process> processes = {{"p1", 0, 7, 5, 0, 0}, {"p2", 1, 1, 1, 0, 0}, {"p3", 1, 3, 4, 0 ,0}, {"p4", 2, 5, 3, 0, 0}, {"p5", 4, 4, 2, 0, 0}};
    ProcessesManager manager = *new ProcessesManager(processes);
//    manager.run(&manager.processes[0], 10);
//    cout << manager.processes[4].name << endl;
//    manager.rightNowProcesses();
    
    manager.highPriorityFirst();
    return 0;
}

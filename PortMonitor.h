#ifndef PORTMONITOR_H
#define PORTMONITOR_H

#include <ctime>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include "IP.h"
using namespace std;


class PortMonitor {
public:
    PortMonitor(const vector<int>& node) : node(node) {
        startTime = time(nullptr);
        endTime = time(nullptr);
        ip = new vector<IP>[node.size()];
    }

    PortMonitor(const vector<int>& node, time_t startTime, time_t endTime)
               : startTime(startTime), endTime(endTime), node(node) {
        ip = new vector<IP>[node.size()];
    }

    virtual ~PortMonitor() {
        delete[] ip;
    }

    void setEndTime() {
        endTime = time(nullptr);;
    }

    time_t getStartTime() const {
        return startTime;
    }

    time_t getEndTime() const {
        return endTime;
    }

    void recode() {
        int nodeSize = node.size();
        for (int i = 0; i < nodeSize; ++i) {
            string nodeNum = to_string(node[i]);
            string tmpFile = "tmp" + nodeNum;
            string command = "netstat -n | grep :" + nodeNum + " >> " + tmpFile;
            system(command.data());

            ifstream fin(tmpFile);
            string tmp;
            while (!fin.eof()) { // Sample: tcp6       0      0 SFO2-Ubuntu18:8932      118.114.165.46:52558    ESTABLISHED
                getline(fin >> ws, tmp);
                int pos = tmp.find(nodeNum) + 5; // max length of node is 5 digits
                if (pos == string::npos)
                    continue;
                pos = tmp.find_first_not_of(' ', pos); // skip space
                if (pos == string::npos)
                    continue;
                if (!isdigit(tmp[pos])) // check the first digit of ip is a number
                    continue;
                int end = tmp.find_first_of(':', pos);
                string ipAddr = tmp.substr(pos, end - pos);
                IP ipNum(ipAddr);

                if (find(this->ip[i].begin(), this->ip[i].end(), ipNum) == this->ip[i].end()
                   && ipNum.isValid()) {
                    this->ip[i].push_back(ipNum);
                }
            }
            fin.close();
            string rm = "rm " + tmpFile;
            system(rm.data());
        }
    }

    void addRecord(const PortMonitor& hourRec) {
        if (this->node != hourRec.node) {
            return;
        }
        int nodeSize = node.size();
        for (int i = 0; i < nodeSize; ++i) {
            for (auto&& recIP : hourRec.ip[i]) { // check whether IP from hourly record in daily record
                if (find(this->ip[i].begin(), this->ip[i].end(), recIP) == this->ip[i].end()) {
                    ip[i].push_back(recIP);
                }
            }
        }
    }

    friend ostream& operator<<(ostream& os, const PortMonitor& monitor) {
        struct tm* t;
        t = localtime(&monitor.startTime);
        os << t->tm_year + 1900 << '-' << t->tm_mon + 1<< '-' << t->tm_mday << ' ' << t->tm_hour << ':' << t->tm_min << " ~ ";
        t = localtime(&monitor.endTime);
        os << t->tm_year + 1900 << '-' << t->tm_mon + 1<< '-' << t->tm_mday << ' ' << t->tm_hour << ':' << t->tm_min << endl;

        int nodeSize = monitor.node.size();
        bool empty = true;
        for (int i = 0; i < nodeSize; ++i) {
            if (!monitor.ip[i].empty()) {
                empty = false;
                string nodeInfo = "  " + to_string(monitor.node[i]) + ": ";
                os.width(9); // 2 spaces + 5 digits port + ": "
                os << left << nodeInfo;
                os << monitor.ip[i][0] << endl;
                for (int j = 1; j < monitor.ip[i].size(); ++j) {
                    os << "         " << monitor.ip[i][j] << endl;
                }
            }
        }
        if (empty) {
            os << "  <None>\n";
        }
        os << endl;
        return os;
    }

private:
    time_t startTime;
    time_t endTime;
    vector<IP>* ip; // Array of vector
    const vector<int>& node;
};


#endif //PORTMONITOR_H

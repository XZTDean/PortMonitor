#ifndef PORTMONITOR_H
#define PORTMONITOR_H

#include <ctime>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
using namespace std;


class PortMonitor {
public:
    PortMonitor(const vector<int>& node) : node(node) {
        startTime = time(nullptr);
        endTime = time(nullptr);
        ip = new vector<string>[node.size()];
    }

    virtual ~PortMonitor() {
        delete[] ip;
    }

    void setEndTime() {
        endTime = time(nullptr);;
    }

    void recode() {
        int nodeSize = node.size();
        for (int i = 0; i < nodeSize; ++i) {
            string nodeNum = to_string(node[i]);
            string command = "netstat -n | grep :" + nodeNum + " >> tmp";
            system(command.data());

            ifstream fin("tmp");
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

                if (find(this->ip[i].begin(), this->ip[i].end(), ipAddr) == this->ip[i].end()) {
                    this->ip[i].push_back(ipAddr);
                }
            }
            fin.close();
            system("rm tmp");
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
    vector<string>* ip; // Array of vector
    const vector<int>& node;
};


#endif //PORTMONITOR_H

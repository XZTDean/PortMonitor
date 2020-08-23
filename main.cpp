#include <iostream>
#include <unistd.h>
#include <sstream>
#include "PortMonitor.h"
#include "IP.h"
using namespace std;

vector<int> getNode() {
    vector<int> node;
    ifstream fin("node.txt");
    int tmp;
    while (fin >> tmp) {
        node.push_back(tmp);
    }
    fin.close();
    return node;
}

string generateDate() {
    time_t startTime = time(nullptr);
    struct tm* t;
    t = localtime(&startTime);
    stringstream ss;
    ss << t->tm_year + 1900 << '-';
    ss.width(2);
    ss.fill('0');
    ss << t->tm_mon + 1 << '-';
    ss.width(2);
    ss.fill('0');
    ss << t->tm_mday;
    return ss.str();
}

PortMonitor* oneHourMonitor(const vector<int>& node) {
    PortMonitor* monitor = new PortMonitor(node);
    for (int i = 0; i < 12; ++i) {
        monitor->recode();
        sleep(300);
    }
    monitor->setEndTime();
    return monitor;
}

void dayMonitor(const vector<int>& node) {
    string date = generateDate();
    PortMonitor* monitor[24];
    for (auto&& mo : monitor) {
        mo = oneHourMonitor(node);
        ofstream fout("log/" + date + ".log", ios::app);
        fout << *mo;
        fout.close();
    }
    PortMonitor dailyRec(node, monitor[0]->getStartTime(), monitor[23]->getEndTime());
    for (auto&& mo : monitor) {
        dailyRec.addRecord(*mo);
    }
    ofstream fout("log/" + date + ".log");
    fout << "********** Daily Record **********\n";
    fout << dailyRec;
    fout << "********** Hourly Record **********\n";
    for (auto&& mo : monitor) {
        fout << *mo;
        delete mo;
    }
    fout.close();
    cout << date << " generate.\n";
}

int main() {
    vector<int> node = getNode();

    while (true) {
        dayMonitor(node);
    }

    return 0;
}
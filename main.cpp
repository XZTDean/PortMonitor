#include <iostream>
#include <unistd.h>
#include <sstream>
#include "NodeMonitor.h"
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

NodeMonitor* oneHourMonitor(const vector<int>& node) {
    NodeMonitor* monitor = new NodeMonitor(node);
    for (int i = 0; i < 12; ++i) {
        monitor->recode();
        sleep(300);
    }
    monitor->setEndTime();
    return monitor;
}

void dayMonitor(const vector<int>& node) {
    string date = generateDate();
    NodeMonitor* monitor[24];
    for (auto& mo : monitor) {
        mo = oneHourMonitor(node);
        ofstream fout("log/" + date + ".log", ios::app);
        fout << *mo;
        fout.close();
    }
    cout << date << " generate.\n";
    for (auto& mo : monitor) {
        delete mo;
    }
}

int main() {
    vector<int> node = getNode();

    while (true) {
        dayMonitor(node);
    }

    return 0;
}
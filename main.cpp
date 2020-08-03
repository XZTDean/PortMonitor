#include <iostream>
#include <unistd.h>
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
    time_t startTime = time(nullptr);
    struct tm* t;
    t = localtime(&startTime);
    string date = to_string(t->tm_year) + '-' + to_string(t->tm_mon) + '-' + to_string(t->tm_mday);
    NodeMonitor* monitor[24];
    for (auto& i : monitor) {
        i = oneHourMonitor(node);
        ofstream fout("log/" + date + ".log", ios::app);
    }
}

int main() {
    vector<int> node = getNode();

    return 0;
}
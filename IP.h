#ifndef PORTMONITOR_IP_H
#define PORTMONITOR_IP_H

#include <string>
#include <stdexcept>
#include <ostream>

using namespace std;

class IP {
public:
    IP(const string& dotDecimal) {
        ip = dotDecimalToInt(dotDecimal);
    }

    static unsigned int dotDecimalToInt(const string& dotDecimal) {
        short dot1 = dotDecimal.find_first_of('.');
        if (dot1 == string::npos) {
            return -1;
        }
        short dot2 = dotDecimal.find_first_of('.', dot1 + 1);
        if (dot2 == string::npos) {
            return -1;
        }
        short dot3 = dotDecimal.find_first_of('.', dot2 + 1);
        if (dot3 == string::npos) {
            return -1;
        }
        unsigned int ipPart[4];
        try {
            ipPart[0] = stoul(dotDecimal.substr(0, dot1));
            ipPart[1] = stoul(dotDecimal.substr(dot1 + 1, dot2 - dot1 - 1));
            ipPart[2] = stoul(dotDecimal.substr(dot2 + 1, dot3 - dot2 - 1));
            ipPart[3] = stoul(dotDecimal.substr(dot3 + 1));
        } catch (const invalid_argument& e) {
            return -1;
        }
        for (int i = 0; i < 4; ++i) {
            if (ipPart[i] > 255) {
                return -1;
            }
        }
        unsigned int ip = ipPart[0];
        for (int i = 1; i < 4; ++i) {
            ip = (ip << 8) + ipPart[i];
        }
        return ip;
    }

    string getIpStr() const {
        unsigned int ipPart[4];
        unsigned int ipTmp = ip;
        ipPart[0] = ipTmp & 0xFF;
        for (int i = 1; i < 4; ++i) {
            ipTmp = ipTmp >> 8;
            ipPart[i] = ipTmp & 0xFF;
        }
        string ipStr = to_string(ipPart[0]);
        for (int i = 1; i < 4; ++i) {
            ipStr = to_string(ipPart[i]) + '.' + ipStr;
        }
        return ipStr;
    }

    bool isValid() const {
        return ip != -1;
    }

    friend ostream& operator<<(ostream& os, const IP& ip) {
        if (ip.isValid()) {
            os << ip.getIpStr();
        }
        return os;
    }

private:
    unsigned int ip;
};


#endif //PORTMONITOR_IP_H

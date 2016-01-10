#ifndef LOGS_H
#define LOGS_H

#include <iostream>

#define DEBUG_MODE      1
#define RELEASE_MODE    0

#define MODE            DEBUG_MODE
#define LOG_PATH        "./logs.txt"

// Thanks to: http://stackoverflow.com/questions/8415890/c-chaining-of-the-operator-for-stdcout-like-usage

//TODO: configure log file ostream

class Logs
{
public:
    std::ostream &m_file;
    Logs(std::ostream &o = std::cout):m_file(o){};

    template <typename T>
    Logs &operator<<(const T &a) {
        m_file<<a;
        return *this;
    }

    Logs &operator<<(std::ostream& (*pf) (std::ostream&)){
        m_file<<pf;
        return *this;
    }
};

static Logs logs;


#endif // LOGS_H

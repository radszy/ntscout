#ifndef VERSION_H
#define VERSION_H

#include <QString>

struct Version {
    static QString toString() {
        return QString("%1.%2.%3").arg(major).arg(minor).arg(fix);
    }

    static const int major = 7;
    static const int minor = 4;
    static const int fix   = 1;
};

#endif // VERSION_H


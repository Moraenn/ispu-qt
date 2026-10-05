#ifndef STAMP_H
#define STAMP_H

#include <QString>

struct Stamp
{
    QString title;
    QString country;
    int year = 1840;
    int circulation = 1;
};

#endif // STAMP_H

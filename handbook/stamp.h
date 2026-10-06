#ifndef STAMP_H
#define STAMP_H
#include <QString>
struct Stamp{
    QString title;
    QString country;
    int year = 1840;
    int circulation = 1;
};
inline bool validStamp(const Stamp &stamp){
    return !stamp.title.trimmed().isEmpty() && stamp.title.size() <= 200 &&
           !stamp.country.trimmed().isEmpty() && stamp.country.size() <= 200 &&
           stamp.year >= 1840 && stamp.year <= 2100 &&
           stamp.circulation >= 1 && stamp.circulation <= 1000000000;
}
#endif

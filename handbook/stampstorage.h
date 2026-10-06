#ifndef STAMPSTORAGE_H
#define STAMPSTORAGE_H
#include <QDataStream>
#include <QVector>
#include "stamp.h"
QDataStream &operator<<(QDataStream &stream, const Stamp &stamp);
QDataStream &operator>>(QDataStream &stream, Stamp &stamp);
class StampStorage{
public:
    static bool save(const QString &path, const QVector<Stamp> &stamps, QString &error);
    static bool load(const QString &path, QVector<Stamp> &stamps, QString &error);
};
#endif

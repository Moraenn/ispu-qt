#include "stampstorage.h"
#include <QFile>
#include <QSaveFile>
namespace{
constexpr quint32 Magic = 0x53544D50;
constexpr quint16 FormatVersion = 1;
constexpr quint32 MaxRecords = 10000;
constexpr qint64 MaxFileSize = 32 * 1024 * 1024;
void writeString(QDataStream &s, const QString &value){
    const QByteArray bytes = value.toUtf8();
    s << quint32(bytes.size());
    if (s.writeRawData(bytes.constData(), int(bytes.size())) != bytes.size()) s.setStatus(QDataStream::WriteFailed);
}
QString readString(QDataStream &s){
    quint32 size = 0;
    s >> size;
    if (s.status() != QDataStream::Ok || size > 1024){
        s.setStatus(QDataStream::ReadCorruptData);
        return {};
    }
    QByteArray bytes(int(size), '\0');
    if (s.readRawData(bytes.data(), int(size)) != int(size)){
        s.setStatus(QDataStream::ReadPastEnd);
        return {};
    }
    const QString text = QString::fromUtf8(bytes);
    if (text.toUtf8() != bytes) s.setStatus(QDataStream::ReadCorruptData);
    return text;
}
void configure(QDataStream &s){
    s.setVersion(QDataStream::Qt_5_15);
    s.setByteOrder(QDataStream::LittleEndian);
}
}
QDataStream &operator<<(QDataStream &s, const Stamp &stamp){
    writeString(s, stamp.title);
    writeString(s, stamp.country);
    return s << qint32(stamp.year) << qint32(stamp.circulation);
}
QDataStream &operator>>(QDataStream &s, Stamp &stamp){
    Stamp value;
    value.title = readString(s);
    value.country = readString(s);
    qint32 year = 0, circulation = 0;
    s >> year >> circulation;
    value.year = year;
    value.circulation = circulation;
    if (s.status() == QDataStream::Ok && validStamp(value)) stamp = value;
    else s.setStatus(QDataStream::ReadCorruptData);
    return s;
}
bool StampStorage::save(const QString &path, const QVector<Stamp> &stamps, QString &error){
    error.clear();
    if (stamps.size() > MaxRecords){ error = "Слишком много записей."; return false; }
    for (const Stamp &stamp : stamps){
        if (!validStamp(stamp)){ error = "Некорректная запись в коллекции."; return false; }
    }
    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly)){ error = file.errorString(); return false; }
    QDataStream stream(&file);
    configure(stream);
    stream << Magic << FormatVersion << quint32(stamps.size());
    for (const Stamp &stamp : stamps) stream << stamp;
    if (stream.status() != QDataStream::Ok){
        file.cancelWriting();
        error = "Ошибка записи данных.";
        return false;
    }
    if (!file.commit()){ error = file.errorString(); return false; }
    return true;
}
bool StampStorage::load(const QString &path, QVector<Stamp> &stamps, QString &error){
    error.clear();
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)){ error = file.errorString(); return false; }
    if (file.size() < 10 || file.size() > MaxFileSize){ error = "Недопустимый размер файла."; return false; }
    QDataStream stream(&file);
    configure(stream);
    quint32 magic = 0, count = 0;
    quint16 version = 0;
    stream >> magic >> version >> count;
    if (magic != Magic || version != FormatVersion || count > MaxRecords){
        error = "Неверный формат или версия файла.";
        return false;
    }
    QVector<Stamp> candidate;
    candidate.reserve(int(count));
    for (quint32 i = 0; i < count; ++i){
        Stamp stamp;
        stream >> stamp;
        if (stream.status() != QDataStream::Ok){ error = "Файл повреждён или содержит некорректные данные."; return false; }
        candidate.append(stamp);
    }
    if (stream.status() != QDataStream::Ok || !file.atEnd()){
        error = "Структура файла нарушена.";
        return false;
    }
    // Старую коллекцию заменяем только после успешного чтения всего файла.
    stamps.swap(candidate);
    return true;
}

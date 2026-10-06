#include <QStringList>
#include "stampmodel.h"
#include <QLocale>
StampModel::StampModel(QObject *parent) : QAbstractTableModel(parent){
    // Учебные записи для демонстрации интерфейса, не исторический каталог.
    m_stamps = {{"Пенни Блэк", "Великобритания", 1840, 68000000},
                {"Голубой Маврикий", "Маврикий", 1847, 500},
                {"Чёрный грош", "Пруссия", 1850, 100000},
                {"Почта СССР. Космос", "СССР", 1961, 3000000}};
}
int StampModel::rowCount(const QModelIndex &parent) const { return parent.isValid() ? 0 : int(m_stamps.size()); }
int StampModel::columnCount(const QModelIndex &parent) const { return parent.isValid() ? 0 : ColumnCount; }
bool StampModel::validIndex(const QModelIndex &i) const{
    return i.isValid() && i.model() == this && i.row() >= 0 && i.row() < m_stamps.size() && i.column() >= 0 && i.column() < ColumnCount;
}
QVariant StampModel::data(const QModelIndex &i, int role) const{
    if (!validIndex(i)) return {};
    const Stamp &s = m_stamps.at(i.row());
    if (role == Qt::TextAlignmentRole && i.column() >= YearColumn) return int(Qt::AlignCenter);
    if (role != Qt::DisplayRole && role != Qt::EditRole) return {};
    switch (i.column()){
    case TitleColumn: return s.title;
    case CountryColumn: return s.country;
    case YearColumn: return s.year;
    case CirculationColumn:
        if (role == Qt::DisplayRole) return QLocale().toString(s.circulation);
        return s.circulation;
    default: return {};
    }
}
QVariant StampModel::headerData(int section, Qt::Orientation orientation, int role) const{
    if (role != Qt::DisplayRole) return {};
    if (orientation == Qt::Vertical) return section >= 0 && section < rowCount() ? QVariant(section + 1) : QVariant();
    const QStringList headers = {"Название марки", "Страна", "Год выпуска", "Тираж"};
    return section >= 0 && section < headers.size() ? QVariant(headers.at(section)) : QVariant();
}
Qt::ItemFlags StampModel::flags(const QModelIndex &i) const{
    return validIndex(i) ? Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable : Qt::NoItemFlags;
}
bool StampModel::setData(const QModelIndex &i, const QVariant &value, int role){
    if (!validIndex(i) || role != Qt::EditRole) return false;
    Stamp s = m_stamps.at(i.row());
    bool ok = true;
    switch (i.column()){
    case TitleColumn: s.title = value.toString().trimmed(); break;
    case CountryColumn: s.country = value.toString().trimmed(); break;
    case YearColumn: s.year = value.toInt(&ok); break;
    case CirculationColumn: s.circulation = value.toInt(&ok); break;
    default: return false;
    }
    if (!ok) return false;
    return updateStamp(i.row(), s);
}
bool StampModel::addStamp(const Stamp &s){
    if (!validStamp(s) || m_stamps.size() >= 10000) return false;
    const int row = int(m_stamps.size());
    beginInsertRows({}, row, row);
    m_stamps.append(s);
    endInsertRows();
    return true;
}
bool StampModel::updateStamp(int row, const Stamp &s){
    if (row < 0 || row >= m_stamps.size() || !validStamp(s)) return false;
    m_stamps[row] = s;
    emit dataChanged(index(row, 0), index(row, ColumnCount - 1), {Qt::DisplayRole, Qt::EditRole});
    return true;
}
bool StampModel::removeStampAt(int row){
    if (row < 0 || row >= m_stamps.size()) return false;
    beginRemoveRows({}, row, row);
    m_stamps.removeAt(row);
    endRemoveRows();
    return true;
}
Stamp StampModel::stampAt(int row) const { return row >= 0 && row < m_stamps.size() ? m_stamps.at(row) : Stamp(); }
bool StampModel::replaceStamps(const QVector<Stamp> &stamps){
    if (stamps.size() > 10000) return false;
    for (const Stamp &s : stamps) if (!validStamp(s)) return false;
    beginResetModel();
    m_stamps = stamps;
    endResetModel();
    return true;
}

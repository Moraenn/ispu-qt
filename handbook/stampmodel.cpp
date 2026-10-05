#include "stampmodel.h"

#include <QLocale>

StampModel::StampModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    m_stamps.append({
        QStringLiteral("Пенни Блэк"),
        QStringLiteral("Великобритания"),
        1840,
        68000000
    });
    m_stamps.append({
        QStringLiteral("Голубой Маврикий"),
        QStringLiteral("Маврикий"),
        1847,
        500
    });
    m_stamps.append({
        QStringLiteral("Чёрный грош"),
        QStringLiteral("Пруссия"),
        1850,
        100000
    });
    m_stamps.append({
        QStringLiteral("Почта СССР. Космос"),
        QStringLiteral("СССР"),
        1961,
        3000000
    });
}

int StampModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return m_stamps.size();
}

int StampModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return ColumnCount;
}

QVariant StampModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_stamps.size()) {
        return {};
    }

    const Stamp &stamp = m_stamps.at(index.row());

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case TitleColumn:
            return stamp.title;
        case CountryColumn:
            return stamp.country;
        case YearColumn:
            return stamp.year;
        case CirculationColumn:
            return QLocale().toString(stamp.circulation);
        default:
            return {};
        }
    }

    if (role == Qt::EditRole) {
        switch (index.column()) {
        case TitleColumn:
            return stamp.title;
        case CountryColumn:
            return stamp.country;
        case YearColumn:
            return stamp.year;
        case CirculationColumn:
            return stamp.circulation;
        default:
            return {};
        }
    }

    if (role == Qt::TextAlignmentRole && (index.column() == YearColumn || index.column() == CirculationColumn)) {
        return Qt::AlignCenter;
    }

    return {};
}

QVariant StampModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return {};
    }

    if (orientation == Qt::Horizontal) {
        switch (section) {
        case TitleColumn:
            return QStringLiteral("Название марки");
        case CountryColumn:
            return QStringLiteral("Страна");
        case YearColumn:
            return QStringLiteral("Год выпуска");
        case CirculationColumn:
            return QStringLiteral("Тираж");
        default:
            return {};
        }
    }

    return section + 1;
}

Qt::ItemFlags StampModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

bool StampModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || role != Qt::EditRole || index.row() < 0 || index.row() >= m_stamps.size()) {
        return false;
    }

    Stamp &stamp = m_stamps[index.row()];

    switch (index.column()) {
    case TitleColumn:
        stamp.title = value.toString().trimmed();
        break;
    case CountryColumn:
        stamp.country = value.toString().trimmed();
        break;
    case YearColumn:
        stamp.year = value.toInt();
        break;
    case CirculationColumn:
        stamp.circulation = value.toInt();
        break;
    default:
        return false;
    }

    emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});
    return true;
}

void StampModel::addStamp(const Stamp &stamp)
{
    const int row = m_stamps.size();
    beginInsertRows(QModelIndex(), row, row);
    m_stamps.append(stamp);
    endInsertRows();
}

bool StampModel::updateStamp(int row, const Stamp &stamp)
{
    if (row < 0 || row >= m_stamps.size()) {
        return false;
    }

    m_stamps[row] = stamp;
    emit dataChanged(index(row, 0), index(row, ColumnCount - 1), {Qt::DisplayRole, Qt::EditRole});
    return true;
}

bool StampModel::removeStampAt(int row)
{
    if (row < 0 || row >= m_stamps.size()) {
        return false;
    }

    beginRemoveRows(QModelIndex(), row, row);
    m_stamps.removeAt(row);
    endRemoveRows();
    return true;
}

Stamp StampModel::stampAt(int row) const
{
    if (row < 0 || row >= m_stamps.size()) {
        return {};
    }

    return m_stamps.at(row);
}

#ifndef STAMPMODEL_H
#define STAMPMODEL_H
#include <QAbstractTableModel>
#include <QVector>
#include "stamp.h"
class StampModel : public QAbstractTableModel{
    Q_OBJECT
public:
    enum Column { TitleColumn, CountryColumn, YearColumn, CirculationColumn, ColumnCount };
    explicit StampModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = {}) const override;
    int columnCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    bool addStamp(const Stamp &stamp);
    bool updateStamp(int row, const Stamp &stamp);
    bool removeStampAt(int row);
    Stamp stampAt(int row) const;
    const QVector<Stamp> &stamps() const { return m_stamps; }
    bool replaceStamps(const QVector<Stamp> &stamps);
private:
    bool validIndex(const QModelIndex &index) const;
    QVector<Stamp> m_stamps;
};
#endif

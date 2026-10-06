#ifndef STAMPTABLEVIEW_H
#define STAMPTABLEVIEW_H
#include <QTableView>
class StampTableView : public QTableView{
    Q_OBJECT
public:
    explicit StampTableView(QWidget *parent = nullptr);
    int currentRow() const;
};
#endif

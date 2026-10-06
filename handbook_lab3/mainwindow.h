#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QWidget>
class StampModel;
class StampTableView;
class QPushButton;
class MainWindow : public QWidget{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
private slots:
    void addStamp();
    void editStamp();
    void removeStamp();
    void updateActions();
private:
    void selectRow(int row);
    StampModel *m_model;
    StampTableView *m_view;
    QPushButton *m_editButton;
    QPushButton *m_removeButton;
};
#endif

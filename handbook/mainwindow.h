#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>

class QPushButton;

class StampDelegate;
class StampModel;
class StampTableView;

class MainWindow : public QWidget
{
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
    StampDelegate *m_delegate;
    QPushButton *m_addButton;
    QPushButton *m_editButton;
    QPushButton *m_removeButton;
    QPushButton *m_closeButton;
};

#endif // MAINWINDOW_H

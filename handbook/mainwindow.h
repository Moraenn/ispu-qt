#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QWidget>
class StampModel;
class StampTableView;
class QPushButton;
class QCloseEvent;
class MainWindow : public QWidget{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
protected:
    void closeEvent(QCloseEvent *event) override;
private slots:
    void newFile();
    void openFile();
    bool saveFile();
    bool saveAs();
    void addStamp();
    void editStamp();
    void removeStamp();
    void updateActions();
private:
    void selectRow(int row);
    bool maybeSave();
    bool saveTo(const QString &path);
    void setDirty(bool dirty);
    QString m_fileName;
    bool m_dirty = false;
    StampModel *m_model;
    StampTableView *m_view;
    QPushButton *m_editButton;
    QPushButton *m_removeButton;
};
#endif

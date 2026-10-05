#ifndef TIMER_H
#define TIMER_H

#include <QObject>

class Timer : public QObject
{
    Q_OBJECT
public:
    explicit Timer(QObject *parent = nullptr);

signals:
    void test_signal();

public slots:
    void test_slot();
};

#endif // TIMER_H

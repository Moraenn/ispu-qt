#ifndef ALARMWINDOW_H
#define ALARMWINDOW_H

#include <QDate>
#include <QWidget>

class QCheckBox;
class QLabel;
class QLCDNumber;
class QSpinBox;
class QTimer;

class AlarmWindow : public QWidget
{
    Q_OBJECT

public:
    explicit AlarmWindow(QWidget *parent = nullptr);

private slots:
    void updateCurrentTime();
    void updateAlarmPreview();
    void setAlarmEnabled(bool enabled);

private:
    void setupUi();
    void connectSignals();
    void triggerAlarm();
    void resetTriggeredState();

    QLCDNumber *m_timeDisplay;
    QSpinBox *m_hourSpinBox;
    QSpinBox *m_minuteSpinBox;
    QCheckBox *m_alarmEnabledCheckBox;
    QLabel *m_alarmTimeLabel;
    QLabel *m_statusLabel;
    QTimer *m_clockTimer;
    QDate m_lastTriggeredDate;
    int m_lastTriggeredHour;
    int m_lastTriggeredMinute;
};

#endif // ALARMWINDOW_H

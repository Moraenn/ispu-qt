#ifndef ALARMWINDOW_H
#define ALARMWINDOW_H
#include <QDateTime>
#include <QWidget>
class QCheckBox;
class QLabel;
class QLCDNumber;
class QSpinBox;
class QTimer;
class AlarmWindow : public QWidget{
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
    void triggerAlarm(const QDateTime &now);
    void resetTriggeredState();
    QLCDNumber *m_timeDisplay;
    QSpinBox *m_hourSpinBox;
    QSpinBox *m_minuteSpinBox;
    QCheckBox *m_alarmEnabledCheckBox;
    QLabel *m_alarmTimeLabel;
    QLabel *m_statusLabel;
    QTimer *m_clockTimer;
    QDate m_lastTriggeredDate;
    int m_lastTriggeredHour = -1;
    int m_lastTriggeredMinute = -1;
};
#endif

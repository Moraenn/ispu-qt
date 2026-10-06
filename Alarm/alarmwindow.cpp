#include "alarmwindow.h"
#include <QApplication>
#include <QCheckBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLCDNumber>
#include <QLabel>
#include <QMessageBox>
#include <QSpinBox>
#include <QTimer>
#include <QVBoxLayout>

AlarmWindow::AlarmWindow(QWidget *parent) : QWidget(parent), m_clockTimer(new QTimer(this)){
    setupUi();
    connectSignals();
    updateAlarmPreview();
    updateCurrentTime();
    m_clockTimer->start(1000);
}
void AlarmWindow::updateCurrentTime(){
    const QDateTime now = QDateTime::currentDateTime();
    const QTime time = now.time();
    m_timeDisplay->display(time.toString("hh:mm:ss"));
    if (!m_alarmEnabledCheckBox->isChecked()) return;
    const bool reached = time.hour() == m_hourSpinBox->value() && time.minute() == m_minuteSpinBox->value();
    const bool fresh = m_lastTriggeredDate != now.date() || m_lastTriggeredHour != time.hour() || m_lastTriggeredMinute != time.minute();
    if (reached && fresh) triggerAlarm(now);
}
void AlarmWindow::updateAlarmPreview(){
    const QTime time(m_hourSpinBox->value(), m_minuteSpinBox->value());
    m_alarmTimeLabel->setText(tr("Установленное время: %1").arg(time.toString("hh:mm")));
    setAlarmEnabled(m_alarmEnabledCheckBox->isChecked());
}
void AlarmWindow::setAlarmEnabled(bool enabled){
    m_statusLabel->setText(enabled ? tr("Будильник включен. Ожидание сигнала.") : tr("Будильник выключен."));
    m_statusLabel->setStyleSheet(enabled ? "color:#1b5e20;font-weight:bold;" : "color:#616161;font-weight:bold;");
    resetTriggeredState();
}
void AlarmWindow::setupUi(){
    setWindowTitle(tr("Будильник"));
    resize(420, 280);
    m_timeDisplay = new QLCDNumber(this);
    m_timeDisplay->setDigitCount(8);
    m_timeDisplay->setSegmentStyle(QLCDNumber::Filled);
    m_timeDisplay->setMinimumHeight(80);
    auto *title = new QLabel(tr("Текущее время"), this);
    title->setStyleSheet("font-size:16px;font-weight:bold;");
    auto *group = new QGroupBox(tr("Настройка будильника"), this);
    auto *settings = new QHBoxLayout(group);
    m_hourSpinBox = new QSpinBox(group);
    m_hourSpinBox->setObjectName("alarmHour");
    m_hourSpinBox->setRange(0, 23);
    m_hourSpinBox->setWrapping(true);
    m_minuteSpinBox = new QSpinBox(group);
    m_minuteSpinBox->setObjectName("alarmMinute");
    m_minuteSpinBox->setRange(0, 59);
    m_minuteSpinBox->setWrapping(true);
    settings->addWidget(new QLabel(tr("Часы:"), group));
    settings->addWidget(m_hourSpinBox);
    settings->addSpacing(16);
    settings->addWidget(new QLabel(tr("Минуты:"), group));
    settings->addWidget(m_minuteSpinBox);
    m_alarmEnabledCheckBox = new QCheckBox(tr("Включить будильник"), this);
    m_alarmEnabledCheckBox->setObjectName("alarmEnabled");
    m_alarmTimeLabel = new QLabel(this);
    m_statusLabel = new QLabel(this);
    m_statusLabel->setWordWrap(true);
    auto *layout = new QVBoxLayout(this);
    layout->addWidget(title);
    layout->addWidget(m_timeDisplay);
    layout->addWidget(group);
    layout->addWidget(m_alarmEnabledCheckBox);
    layout->addWidget(m_alarmTimeLabel);
    layout->addWidget(m_statusLabel);
    const QTime initial = QTime::currentTime().addSecs(60);
    m_hourSpinBox->setValue(initial.hour());
    m_minuteSpinBox->setValue(initial.minute());
}
void AlarmWindow::connectSignals(){
    connect(m_clockTimer, &QTimer::timeout, this, &AlarmWindow::updateCurrentTime);
    connect(m_hourSpinBox, qOverload<int>(&QSpinBox::valueChanged), this, &AlarmWindow::updateAlarmPreview);
    connect(m_minuteSpinBox, qOverload<int>(&QSpinBox::valueChanged), this, &AlarmWindow::updateAlarmPreview);
    connect(m_alarmEnabledCheckBox, &QCheckBox::toggled, this, &AlarmWindow::setAlarmEnabled);
}
void AlarmWindow::triggerAlarm(const QDateTime &now){
    // Отметка ставится до показа модального окна: таймер продолжает работать.
    m_lastTriggeredDate = now.date();
    m_lastTriggeredHour = now.time().hour();
    m_lastTriggeredMinute = now.time().minute();
    m_statusLabel->setText(tr("Сигнал! Пора вставать!"));
    m_statusLabel->setStyleSheet("color:#b71c1c;font-weight:bold;");
    QApplication::beep();
    QMessageBox::information(this, tr("Будильник"), tr("Пора вставать!"));
}
void AlarmWindow::resetTriggeredState(){
    m_lastTriggeredDate = QDate();
    m_lastTriggeredHour = m_lastTriggeredMinute = -1;
}

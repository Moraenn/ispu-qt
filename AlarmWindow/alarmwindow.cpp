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

AlarmWindow::AlarmWindow(QWidget *parent)
    : QWidget(parent)
    , m_timeDisplay(nullptr)
    , m_hourSpinBox(nullptr)
    , m_minuteSpinBox(nullptr)
    , m_alarmEnabledCheckBox(nullptr)
    , m_alarmTimeLabel(nullptr)
    , m_statusLabel(nullptr)
    , m_clockTimer(new QTimer(this))
    , m_lastTriggeredHour(-1)
    , m_lastTriggeredMinute(-1)
{
    setupUi();
    connectSignals();
    updateAlarmPreview();
    updateCurrentTime();
    m_clockTimer->start(1000);
}

void AlarmWindow::updateCurrentTime()
{
    const QTime currentTime = QTime::currentTime();
    m_timeDisplay->display(currentTime.toString("hh:mm:ss"));

    if (!m_alarmEnabledCheckBox->isChecked()) {
        return;
    }

    const bool isAlarmTimeReached =
        currentTime.hour() == m_hourSpinBox->value() &&
        currentTime.minute() == m_minuteSpinBox->value();

    const bool isNewTriggerMoment =
        m_lastTriggeredDate != QDate::currentDate() ||
        m_lastTriggeredHour != currentTime.hour() ||
        m_lastTriggeredMinute != currentTime.minute();

    if (isAlarmTimeReached && isNewTriggerMoment) {
        triggerAlarm();
    }
}

void AlarmWindow::updateAlarmPreview()
{
    const QString timeText = QString("%1:%2")
    .arg(m_hourSpinBox->value())
        .arg(m_minuteSpinBox->value());

    m_alarmTimeLabel->setText(QString("Установленное время: %1").arg(timeText));

    if (m_alarmEnabledCheckBox->isChecked()) {
        m_statusLabel->setText(QString("Будильник включен. Ожидание сигнала."));
        m_statusLabel->setStyleSheet("color: #1b5e20; font-weight: bold;");
    } else {
        m_statusLabel->setText(QString("Будильник выключен."));
        m_statusLabel->setStyleSheet("color: #616161; font-weight: bold;");
    }

    resetTriggeredState();
}

void AlarmWindow::setAlarmEnabled(bool enabled)
{
    if (enabled) {
        m_statusLabel->setText(tr("Будильник включен. Ожидание сигнала."));
        m_statusLabel->setStyleSheet("color: #1b5e20; font-weight: bold;");
    } else {
        m_statusLabel->setText(tr("Будильник выключен."));
        m_statusLabel->setStyleSheet("color: #616161; font-weight: bold;");
    }

    resetTriggeredState();
}

void AlarmWindow::setupUi()
{
    setWindowTitle("Будильник");
    resize(420, 260);

    m_timeDisplay = new QLCDNumber(this);
    m_timeDisplay->setDigitCount(8);
    m_timeDisplay->setSegmentStyle(QLCDNumber::Filled);
    m_timeDisplay->setMinimumHeight(80);

    auto *displayTitle = new QLabel("Текущее время", this);
    displayTitle->setStyleSheet("font-size: 16px; font-weight: bold;");

    auto *settingsGroup = new QGroupBox("Настройка будильника", this);
    auto *settingsLayout = new QHBoxLayout(settingsGroup);

    auto *hourLabel = new QLabel("Часы:", settingsGroup);
    m_hourSpinBox = new QSpinBox(settingsGroup);
    m_hourSpinBox->setRange(0, 23);
    m_hourSpinBox->setWrapping(true);

    auto *minuteLabel = new QLabel("Минуты:", settingsGroup);
    m_minuteSpinBox = new QSpinBox(settingsGroup);
    m_minuteSpinBox->setRange(0, 59);
    m_minuteSpinBox->setWrapping(true);

    settingsLayout->addWidget(hourLabel);
    settingsLayout->addWidget(m_hourSpinBox);
    settingsLayout->addSpacing(16);
    settingsLayout->addWidget(minuteLabel);
    settingsLayout->addWidget(m_minuteSpinBox);

    m_alarmEnabledCheckBox = new QCheckBox("Включить будильник", this);

    m_alarmTimeLabel = new QLabel(this);
    m_statusLabel = new QLabel(this);
    m_statusLabel->setWordWrap(true);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(displayTitle);
    mainLayout->addWidget(m_timeDisplay);
    mainLayout->addWidget(settingsGroup);
    mainLayout->addWidget(m_alarmEnabledCheckBox);
    mainLayout->addWidget(m_alarmTimeLabel);
    mainLayout->addWidget(m_statusLabel);

    setLayout(mainLayout);

    const QTime initialAlarmTime = QTime::currentTime().addSecs(60);
    m_hourSpinBox->setValue(initialAlarmTime.hour());
    m_minuteSpinBox->setValue(initialAlarmTime.minute());
}

void AlarmWindow::connectSignals()
{
    connect(m_clockTimer, &QTimer::timeout, this, &AlarmWindow::updateCurrentTime);
    connect(m_hourSpinBox, &QSpinBox::valueChanged, this, &AlarmWindow::updateAlarmPreview);
    connect(m_minuteSpinBox, &QSpinBox::valueChanged, this, &AlarmWindow::updateAlarmPreview);
    connect(m_alarmEnabledCheckBox, &QCheckBox::toggled, this, &AlarmWindow::setAlarmEnabled);
}

void AlarmWindow::triggerAlarm()
{
    const QTime currentTime = QTime::currentTime();

    m_lastTriggeredDate = QDate::currentDate();
    m_lastTriggeredHour = currentTime.hour();
    m_lastTriggeredMinute = currentTime.minute();

    m_statusLabel->setText("Сигнал! Пора вставать!");
    m_statusLabel->setStyleSheet("color: #b71c1c; font-weight: bold; font-size: 15px;");

    QApplication::beep();
    QMessageBox::information(this, "Будильник", "Пора вставать!");
}

void AlarmWindow::resetTriggeredState()
{
    m_lastTriggeredDate = QDate();
    m_lastTriggeredHour = -1;
    m_lastTriggeredMinute = -1;
}

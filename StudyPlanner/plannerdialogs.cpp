#include "plannerdialogs.h"
#include "qtutils.h"
#include <QComboBox>
#include <QDateEdit>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QSpinBox>
#include <QVBoxLayout>
TaskDialog::TaskDialog(const QStringList &subjects, QWidget *parent) : QDialog(parent){
    setWindowTitle("Задание");
    resize(470, 310);
    m_title = new QLineEdit(this);
    m_title->setMaxLength(200);
    m_subject = new QComboBox(this);
    m_subject->setEditable(true);
    m_subject->addItems(subjects);
    m_subject->lineEdit()->setMaxLength(150);
    m_deadline = new QDateEdit(QDate::currentDate().addDays(7), this);
    m_deadline->setCalendarPopup(true);
    m_deadline->setDateRange(QDate(1900, 1, 1), QDate(2100, 12, 31));
    m_deadline->setDisplayFormat("dd.MM.yyyy");
    m_priority = new QComboBox(this);
    m_status = new QComboBox(this);
    for (int i = 0; i < 3; ++i){
        m_priority->addItem(priorityText(planner::Priority(i)), i);
        m_status->addItem(statusText(planner::Status(i)), i);
    }
    m_priority->setCurrentIndex(1);
    m_minutes = new QSpinBox(this);
    m_minutes->setRange(1, 6000);
    m_minutes->setSingleStep(15);
    m_minutes->setValue(120);
    m_minutes->setSuffix(" мин");
    auto *form = new QFormLayout;
    form->addRow("Предмет:", m_subject);
    form->addRow("Задание:", m_title);
    form->addRow("Срок сдачи:", m_deadline);
    form->addRow("Приоритет:", m_priority);
    form->addRow("Статус:", m_status);
    form->addRow("Плановое время:", m_minutes);
    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    auto *layout = new QVBoxLayout(this);
    layout->addLayout(form);
    layout->addWidget(buttons);
    connect(buttons, &QDialogButtonBox::accepted, this, [this](){
        if (m_title->text().trimmed().isEmpty() || m_subject->currentText().trimmed().isEmpty()){
            QMessageBox::warning(this, "Ошибка ввода", "Укажите предмет и название задания.");
            return;
        }
        accept();
    });
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}
void TaskDialog::setTask(const planner::Task &t){
    m_id = t.id;
    m_title->setText(text(t.title));
    m_subject->setEditText(text(t.subject));
    m_deadline->setDate(QDate::fromString(text(t.deadline), Qt::ISODate));
    m_priority->setCurrentIndex(int(t.priority));
    m_status->setCurrentIndex(int(t.status));
    m_minutes->setValue(t.plannedMinutes);
}
planner::Task TaskDialog::task() const{
    return {m_id, m_subject->currentText().trimmed().toStdString(), m_title->text().trimmed().toStdString(),
            m_deadline->date().toString(Qt::ISODate).toStdString(), planner::Priority(m_priority->currentData().toInt()),
            planner::Status(m_status->currentData().toInt()), m_minutes->value()};
}
SessionDialog::SessionDialog(const std::vector<planner::Task> &tasks, int selectedId, QWidget *parent) : QDialog(parent){
    setWindowTitle("Запись занятия");
    resize(490, 230);
    m_task = new QComboBox(this);
    for (const auto &t : tasks) m_task->addItem(text(t.subject) + " — " + text(t.title), t.id);
    const int index = m_task->findData(selectedId);
    if (index >= 0) m_task->setCurrentIndex(index);
    m_date = new QDateEdit(QDate::currentDate(), this);
    m_date->setCalendarPopup(true);
    m_date->setDateRange(QDate(1900, 1, 1), QDate::currentDate());
    m_date->setDisplayFormat("dd.MM.yyyy");
    m_minutes = new QSpinBox(this);
    m_minutes->setRange(1, 720);
    m_minutes->setSingleStep(15);
    m_minutes->setValue(45);
    m_minutes->setSuffix(" мин");
    m_note = new QLineEdit(this);
    m_note->setMaxLength(500);
    auto *form = new QFormLayout;
    form->addRow("Задание:", m_task);
    form->addRow("Дата занятия:", m_date);
    form->addRow("Длительность:", m_minutes);
    form->addRow("Примечание:", m_note);
    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    auto *layout = new QVBoxLayout(this);
    layout->addLayout(form);
    layout->addWidget(buttons);
    connect(buttons, &QDialogButtonBox::accepted, this, [this](){ if (m_task->currentIndex() >= 0) accept(); });
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}
planner::StudySession SessionDialog::session() const{
    return {0, m_task->currentData().toInt(), m_date->date().toString(Qt::ISODate).toStdString(),
            m_minutes->value(), m_note->text().trimmed().toStdString()};
}

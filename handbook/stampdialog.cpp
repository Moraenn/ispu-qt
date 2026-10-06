#include "stampdialog.h"
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QSpinBox>
#include <QVBoxLayout>
StampDialog::StampDialog(QWidget *parent) : QDialog(parent){
    setWindowTitle("Марка");
    m_titleEdit = new QLineEdit(this);
    m_countryEdit = new QLineEdit(this);
    m_titleEdit->setMaxLength(200);
    m_countryEdit->setMaxLength(200);
    m_yearSpin = new QSpinBox(this);
    m_yearSpin->setRange(1840, 2100);
    m_circulationSpin = new QSpinBox(this);
    m_circulationSpin->setRange(1, 1000000000);
    m_circulationSpin->setGroupSeparatorShown(true);
    auto *form = new QFormLayout;
    form->addRow("Название марки:", m_titleEdit);
    form->addRow("Страна:", m_countryEdit);
    form->addRow("Год выпуска:", m_yearSpin);
    form->addRow("Тираж:", m_circulationSpin);
    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    auto *layout = new QVBoxLayout(this);
    layout->addLayout(form);
    layout->addWidget(buttons);
    connect(buttons, &QDialogButtonBox::accepted, this, [this](){
        if (!validStamp(stamp())){
            QMessageBox::warning(this, "Ошибка ввода", "Заполните название и страну. Проверьте числовые поля.");
            return;
        }
        accept();
    });
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}
void StampDialog::setStamp(const Stamp &s){
    m_titleEdit->setText(s.title);
    m_countryEdit->setText(s.country);
    m_yearSpin->setValue(s.year);
    m_circulationSpin->setValue(s.circulation);
}
Stamp StampDialog::stamp() const{
    return {m_titleEdit->text().trimmed(), m_countryEdit->text().trimmed(), m_yearSpin->value(), m_circulationSpin->value()};
}

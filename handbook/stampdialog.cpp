#include "stampdialog.h"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

StampDialog::StampDialog(QWidget *parent)
    : QDialog(parent)
    , m_titleEdit(new QLineEdit(this))
    , m_countryEdit(new QLineEdit(this))
    , m_yearSpin(new QSpinBox(this))
    , m_circulationSpin(new QSpinBox(this))
    , m_buttonBox(new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this))
{
    setModal(true);
    setWindowTitle(QStringLiteral("Марка"));

    m_yearSpin->setRange(1840, 2100);
    m_circulationSpin->setRange(1, 1000000000);
    m_circulationSpin->setGroupSeparatorShown(true);

    auto *formLayout = new QFormLayout;
    formLayout->addRow(QStringLiteral("Название марки:"), m_titleEdit);
    formLayout->addRow(QStringLiteral("Страна:"), m_countryEdit);
    formLayout->addRow(QStringLiteral("Год выпуска:"), m_yearSpin);
    formLayout->addRow(QStringLiteral("Тираж:"), m_circulationSpin);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(m_buttonBox);

    connect(m_buttonBox, &QDialogButtonBox::accepted, this, [this]() {
        if (m_titleEdit->text().trimmed().isEmpty()) {
            QMessageBox::warning(this,
                                 QStringLiteral("Ошибка ввода"),
                                 QStringLiteral("Введите название марки."));
            m_titleEdit->setFocus();
            return;
        }

        if (m_countryEdit->text().trimmed().isEmpty()) {
            QMessageBox::warning(this,
                                 QStringLiteral("Ошибка ввода"),
                                 QStringLiteral("Введите страну."));
            m_countryEdit->setFocus();
            return;
        }

        accept();
    });
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void StampDialog::setStamp(const Stamp &stamp)
{
    m_titleEdit->setText(stamp.title);
    m_countryEdit->setText(stamp.country);
    m_yearSpin->setValue(stamp.year);
    m_circulationSpin->setValue(stamp.circulation);
}

Stamp StampDialog::stamp() const
{
    return {
        m_titleEdit->text().trimmed(),
        m_countryEdit->text().trimmed(),
        m_yearSpin->value(),
        m_circulationSpin->value()
    };
}

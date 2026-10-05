#ifndef STAMPDIALOG_H
#define STAMPDIALOG_H

#include <QDialog>

#include "stamp.h"

class QDialogButtonBox;
class QLineEdit;
class QSpinBox;

class StampDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StampDialog(QWidget *parent = nullptr);

    void setStamp(const Stamp &stamp);
    Stamp stamp() const;

private:
    QLineEdit *m_titleEdit;
    QLineEdit *m_countryEdit;
    QSpinBox *m_yearSpin;
    QSpinBox *m_circulationSpin;
    QDialogButtonBox *m_buttonBox;
};

#endif // STAMPDIALOG_H

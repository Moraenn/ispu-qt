#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QKeyEvent>
#include <QLabel>
#include <QPushButton>
#include <QRegularExpression>
#include <QStack>
#include <QWidget>
#include <QSet>

class Calculator : public QWidget
{
    Q_OBJECT

private:
    QLabel *displayLabel;
    QLabel *expressionLabel;
    QStack<QString> stack;
    QSet<QString> nums       = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "." },
                  operations = { "+", "-", "*", "=", "/" };


    bool isNumber(const QString &text) const;

protected:
    void keyPressEvent(QKeyEvent *event) override;

public:
    Calculator(QWidget *parent = nullptr);
    QPushButton *createButton(const QString &text);
    void clearAll();
    void input(const QString &text);
    void calculate();

public slots:
    void slotButtonClicked();
};

#endif // CALCULATOR_H

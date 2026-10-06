#ifndef CALCULATOR_H
#define CALCULATOR_H
#include <QKeyEvent>
#include <QLabel>
#include <QPushButton>
#include <QStack>
#include <QSet>
#include <QWidget>
class Calculator : public QWidget{
    Q_OBJECT
public:
    explicit Calculator(QWidget *parent = nullptr);
    QPushButton *createButton(const QString &text);
    void clearAll();
    void input(const QString &text);
    void calculate();
public slots:
    void slotButtonClicked();
protected:
    void keyPressEvent(QKeyEvent *event) override;
private:
    bool isNumber(const QString &text) const;
    QLabel *displayLabel;
    QLabel *expressionLabel;
    QStack<QString> stack;
    QSet<QString> nums = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "."};
    QSet<QString> operations = {"+", "-", "*", "=", "/"};
};
#endif

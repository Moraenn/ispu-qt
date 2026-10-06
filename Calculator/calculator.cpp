#include "calculator.h"
#include <QGridLayout>
#include <cmath>

Calculator::Calculator(QWidget *parent) : QWidget(parent){
    setWindowTitle("Калькулятор");
    setStyleSheet("QWidget {background:#000000;} QLabel {color:#FFFFFF;}");
    setFocusPolicy(Qt::StrongFocus);
    expressionLabel = new QLabel;
    expressionLabel->setObjectName("expression");
    expressionLabel->setMinimumSize(250, 35);
    expressionLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    displayLabel = new QLabel;
    displayLabel->setObjectName("display");
    displayLabel->setMinimumSize(250, 50);
    displayLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    QFont displayFont = font();
    displayFont.setPointSize(24);
    displayFont.setBold(true);
    displayLabel->setFont(displayFont);
    const QString buttons[4][4] = {
        {"7", "8", "9", "/"}, {"4", "5", "6", "*"},
        {"1", "2", "3", "-"}, {".", "0", "=", "+"}
    };
    auto *layout = new QGridLayout(this);
    layout->addWidget(expressionLabel, 0, 0, 1, 4);
    layout->addWidget(displayLabel, 1, 0, 1, 4);
    layout->addWidget(createButton("CE"), 2, 3);
    for (int i = 0; i < 4; ++i){
        for (int j = 0; j < 4; ++j) layout->addWidget(createButton(buttons[i][j]), i + 3, j);
    }
}

QPushButton *Calculator::createButton(const QString &text){
    auto *button = new QPushButton(text);
    QString color = nums.contains(text) ? "#333333" : operations.contains(text) ? "#FF9F0A" : "#A5A5A5";
    button->setStyleSheet(QString("background:%1;color:white;border:none;border-radius:18px;font-size:16px;").arg(color));
    button->setMinimumSize(56, 42);
    button->setFocusPolicy(Qt::NoFocus);
    connect(button, &QPushButton::clicked, this, &Calculator::slotButtonClicked);
    return button;
}

bool Calculator::isNumber(const QString &text) const{
    bool ok = false;
    const double value = text.toDouble(&ok);
    // toDouble понимает и обычную, и научную запись результата.
    return ok && std::isfinite(value);
}

void Calculator::calculate(){
    if (stack.count() != 3) return;
    const QString second = stack.pop();
    const QString operation = stack.pop();
    const QString first = stack.pop();
    const double a = first.toDouble(), b = second.toDouble();
    double result = 0;
    bool ok = isNumber(first) && isNumber(second);
    if (operation == "+") result = a + b;
    else if (operation == "-") result = a - b;
    else if (operation == "*") result = a * b;
    else if (operation == "/" && b != 0) result = a / b;
    else ok = false;
    stack.clear();
    if (!ok || !std::isfinite(result)){
        expressionLabel->clear();
        displayLabel->setText("Ошибка");
        return;
    }
    displayLabel->setText(QString::number(result, 'g', 15));
}

void Calculator::slotButtonClicked(){
    const auto *button = qobject_cast<QPushButton *>(sender());
    if (!button) return;
    if (button->text() == "CE") clearAll();
    else input(button->text());
}

void Calculator::clearAll(){
    stack.clear();
    displayLabel->clear();
    expressionLabel->clear();
}

void Calculator::input(const QString &text){
    QString current = displayLabel->text();
    if (current == "Ошибка"){
        clearAll();
        current.clear();
    }
    if (text == "." || (text.size() == 1 && text[0] >= '0' && text[0] <= '9')){
        if (expressionLabel->text().endsWith('=')){
            clearAll();
            current.clear();
        }
        if (current.size() >= 64 || (text == "." && current.contains('.'))) return;
        if (current.isEmpty()) displayLabel->setText(text == "." ? "0." : text);
        else if (current == "-") displayLabel->setText(text == "." ? "-0." : current + text);
        else if (isNumber(current)) displayLabel->setText(current + text);
        return;
    }
    if (text == "-" && current.isEmpty()){
        displayLabel->setText("-");
        return;
    }
    if (text == "="){
        if (stack.count() == 2 && isNumber(current)){
            expressionLabel->setText(stack.at(0) + " " + stack.at(1) + " " + current + " =");
            stack.push(current);
            calculate();
        }
        return;
    }
    if (text != "+" && text != "-" && text != "*" && text != "/") return;
    if (stack.count() == 2 && current.isEmpty()){
        stack.pop();
        stack.push(text);
        expressionLabel->setText(stack.at(0) + " " + text);
        return;
    }
    if (!isNumber(current)) return;
    if (stack.count() == 2){
        stack.push(current);
        calculate();
        if (displayLabel->text() == "Ошибка") return;
        current = displayLabel->text();
    }
    stack.clear();
    stack.push(current);
    stack.push(text);
    expressionLabel->setText(current + " " + text);
    displayLabel->clear();
}

void Calculator::keyPressEvent(QKeyEvent *event){
    switch (event->key()){
    case Qt::Key_Return:
    case Qt::Key_Enter:
    case Qt::Key_Equal: input("="); return;
    case Qt::Key_Delete:
    case Qt::Key_Backspace:
    case Qt::Key_Escape: clearAll(); return;
    case Qt::Key_Comma: input("."); return;
    default: break;
    }
    const QString text = event->text();
    if (nums.contains(text) || operations.contains(text)) input(text);
    else QWidget::keyPressEvent(event);
}

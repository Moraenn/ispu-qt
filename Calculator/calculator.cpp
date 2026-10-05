#include "calculator.h"

#include <QGridLayout>

Calculator::Calculator(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("Калькулятор");
    setStyleSheet(
        "QWidget { background-color: #000000; }"
        "QLabel { color: #FFFFFF; }"
    );
    setFocusPolicy(Qt::StrongFocus);

    expressionLabel = new QLabel("");
    expressionLabel->setMinimumSize(150, 35);
    expressionLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    displayLabel = new QLabel("");
    displayLabel->setMinimumSize(150, 50);
    displayLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    QFont displayFont = font();
    displayFont.setPointSize(24);
    displayFont.setBold(true);

    displayLabel->setFont(displayFont);

    const QString buttons[4][4] = {
        {"7", "8", "9", "/"},
        {"4", "5", "6", "*"},
        {"1", "2", "3", "-"},
        {".", "0", "=", "+"}
    };

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(expressionLabel, 0, 0, 1, 4);
    layout->addWidget(displayLabel, 1, 0, 1, 4);
    layout->addWidget(createButton("CE"), 3, 3);

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            layout->addWidget(createButton(buttons[i][j]), i + 4, j);
        }
    }

    setLayout(layout);
}

QPushButton *Calculator::createButton(const QString &text)
{
    QPushButton *button = new QPushButton(text);

    QString style;
    if      (nums.contains(text))        style = "background-color: #333333; color: #FFFFFF; border: none; border-radius: 20px; font-size: 16px";
    else if (operations.contains(text))  style = "background-color: #FF9F0A; color: #FFFFFF; border: none; border-radius: 20px; font-size: 16px";
    else                                 style = "background-color: #A5A5A5; color: #000000; border: none; border-radius: 20px; font-size: 16px";

    button->setStyleSheet(style);
    button->setMinimumSize(50, 40);
    button->setFocusPolicy(Qt::NoFocus);
    connect(button, &QPushButton::clicked, this, &Calculator::slotButtonClicked);
    return button;
}

bool Calculator::isNumber(const QString &text) const
{
    static const QRegularExpression numberPattern(
        "^[-]?(?:\\d+\\.?\\d*|\\.\\d+)$"
    );

    return numberPattern.match(text).hasMatch();
}

void Calculator::calculate()
{
    if (stack.count() < 3) {
        return;
    }

    const double  operand2  = stack.pop().toDouble();
    const QString operation = stack.pop();
    const double  operand1  = stack.pop().toDouble();

    if (operation == "/" && operand2 == 0.0) {
        stack.clear();
        expressionLabel->clear();
        displayLabel->setText("Ошибка");
        return;
    }

    double result = 0.0;

    if      (operation == "+") result = operand1 + operand2;
    else if (operation == "-") result = operand1 - operand2;
    else if (operation == "/") result = operand1 / operand2;
    else if (operation == "*") result = operand1 * operand2;


    stack.clear();
    displayLabel->setText(QString::number(result, 'g', 15));
}

void Calculator::slotButtonClicked()
{
    const auto *button = qobject_cast<QPushButton *>(sender());
    if (button == nullptr) {
        return;
    }

    const QString text = button->text();
    if (text == "CE") {
        clearAll();
        return;
    }

    input(text);
}

void Calculator::clearAll()
{
    stack.clear();
    displayLabel->clear();
    expressionLabel->clear();
}

void Calculator::input(const QString &text)
{
    QString currentText = displayLabel->text();

    if (currentText == "Ошибка") {
        clearAll();
        currentText.clear();
    }

    if (text == "." || (text.length() == 1 && text[0].isDigit())) {
        // Если до этого уже было =, начинаем новый ввод
        if (expressionLabel->text().endsWith('=')) {
            clearAll();
            currentText.clear();
        }

        // Запрет второй точки
        if (text == "." && currentText.contains('.')) {
            return;
        }

        // Если поле пустое
        if (currentText.isEmpty()) {
            displayLabel->setText(text == "." ? "0." : text);
            return;
        }

        // Если на дисплее только минус
        if (currentText == "-") {
            displayLabel->setText(text == "." ? "-0." : currentText + text);
            return;
        }

        if (isNumber(currentText)) {
            displayLabel->setText(currentText + text);
        }
        return;
    }

    // Минус в пустом поле считаем началом отрицательного числа.
    if (text == "-" && currentText.isEmpty()) {
        if (expressionLabel->text().endsWith('=')) {
            clearAll();
        }
        displayLabel->setText("-");
        return;
    }

    const bool isOperator =
        text == "+" || text == "-" || text == "*" || text == "/";

    if (text == "=") {
        if (stack.count() == 2 && isNumber(currentText)) {
            stack.push(currentText);
            expressionLabel->setText(
                stack.at(0) + " " + stack.at(1) + " " + currentText + " ="
            );
            calculate();
        }
        return;
    }

    if (!isOperator) {
        return;
    }

    if (stack.count() == 2 && currentText.isEmpty() && text != "-") {
        stack.pop();
        stack.push(text);
        expressionLabel->setText(stack.at(0) + " " + text);
        return;
    }

    if (!isNumber(currentText)) {
        return;
    }

    QString valueForNextStep = currentText;
    if (stack.count() == 2) {
        const QString firstOperand = stack.at(0);
        const QString operation = stack.at(1);

        stack.push(currentText);
        expressionLabel->setText(
            firstOperand + " " + operation + " " + currentText + " ="
        );
        calculate();

        if (displayLabel->text() == "Ошибка") {
            return;
        }

        valueForNextStep = displayLabel->text();
    }

    stack.clear();
    stack.push(valueForNextStep);
    stack.push(text);
    expressionLabel->setText(valueForNextStep + " " + text);
    displayLabel->clear();
}

void Calculator::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {

        case Qt::Key_Return:
        case Qt::Key_Enter:
            input("=");
            return;

        case Qt::Key_Delete:
        case Qt::Key_Backspace:
        case Qt::Key_Escape:
            clearAll();
            return;

        case Qt::Key_Comma:
            input(".");
            return;

        default:
            break;
    }

    const QString text = event->text();
    if (text.length() == 1) {
        const QChar symbol = text[0];
        if (symbol.isDigit() || text == "." || text == "+" ||
            text == "-" || text == "*" || text == "/")
        {
            input(text);
            return;
        }
    }

    QWidget::keyPressEvent(event);
}

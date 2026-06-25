#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QListWidget>
#include <QSpinBox>
#include <QGroupBox>

#include "TabOrdinals.hpp"

TabOrdinals::TabOrdinals(QTextEdit *sharedLogArea, QWidget *parent)
    : QWidget(parent), logArea(sharedLogArea), currentResult(0), hasResult(false)
{
    setupLayout();
}

QString TabOrdinals::ordinalToString(const Ordinal& ord) const
{
    size_t inf = ord.GetInfinity();
    size_t fin = ord.GetSize();

    if (inf == 0 && fin == 0) return "0";

    QString result;

    if (inf > 0) {
        if (inf == 1)
            result += QString::fromUtf8("\xcf\x89");
        else
            result += QString::fromUtf8("\xcf\x89") + QString::fromUtf8("\xc2\xb7") + QString::number(inf);
    }

    if (fin > 0) {
        if (!result.isEmpty())
            result += " + ";
        result += QString::number(fin);
    }

    return result;
}

void TabOrdinals::updateDisplay(QLabel *label, size_t inf, size_t fin)
{
    label->setText(ordinalToString(Ordinal(fin, inf)));
}

void TabOrdinals::setupGroupA(QGroupBox *box)
{
    QGridLayout *grid = new QGridLayout(box);

    spinInfA = new QSpinBox(box);
    spinInfA->setRange(0, 9999);

    spinFinA = new QSpinBox(box);
    spinFinA->setRange(0, 9999);

    labelDisplayA = new QLabel("0", box);
    labelDisplayA->setAlignment(Qt::AlignCenter);

    grid->addWidget(new QLabel("k (коэф. при w):", box),    0, 0);
    grid->addWidget(spinInfA,                               0, 1);
    grid->addWidget(new QLabel("n (конечный остаток):", box), 1, 0);
    grid->addWidget(spinFinA,                               1, 1);
    grid->addWidget(new QLabel("Значение:", box),           2, 0);
    grid->addWidget(labelDisplayA,                          2, 1);
}

void TabOrdinals::setupGroupB(QGroupBox *box)
{
    QGridLayout *grid = new QGridLayout(box);

    spinInfB = new QSpinBox(box);
    spinInfB->setRange(0, 9999);

    spinFinB = new QSpinBox(box);
    spinFinB->setRange(0, 9999);

    labelDisplayB = new QLabel("0", box);
    labelDisplayB->setAlignment(Qt::AlignCenter);

    grid->addWidget(new QLabel("k (коэф. при w):", box),    0, 0);
    grid->addWidget(spinInfB,                               0, 1);
    grid->addWidget(new QLabel("n (конечный остаток):", box), 1, 0);
    grid->addWidget(spinFinB,                               1, 1);
    grid->addWidget(new QLabel("Значение:", box),           2, 0);
    grid->addWidget(labelDisplayB,                          2, 1);
}

void TabOrdinals::setupLayout()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    mainLayout->addWidget(new QLabel("Калькулятор ординалов (w*k + n)", this));

    QHBoxLayout *inputsRow = new QHBoxLayout();

    QGroupBox *groupA = new QGroupBox("Операнд A", this);
    setupGroupA(groupA);

    QGroupBox *groupB = new QGroupBox("Операнд B", this);
    setupGroupB(groupB);

    operationCombo = new QComboBox(this);
    operationCombo->addItem("+  (сложение)");
    operationCombo->addItem("<  (меньше)");
    operationCombo->addItem(">  (больше)");
    operationCombo->addItem("== (равенство)");
    operationCombo->addItem("<= (не больше)");
    operationCombo->addItem(">= (не меньше)");

    QVBoxLayout *opLayout = new QVBoxLayout();
    opLayout->addStretch();
    opLayout->addWidget(new QLabel("Операция:", this));
    opLayout->addWidget(operationCombo);
    opLayout->addStretch();

    inputsRow->addWidget(groupA);
    inputsRow->addLayout(opLayout);
    inputsRow->addWidget(groupB);
    mainLayout->addLayout(inputsRow);

    calcButton = new QPushButton("Вычислить", this);
    mainLayout->addWidget(calcButton);

    labelResult = new QLabel("—", this);
    labelResult->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(labelResult);

    useResultAsAButton = new QPushButton("<- Использовать как A", this);
    useResultAsBButton = new QPushButton("Использовать как B ->", this);
    useResultAsAButton->setEnabled(false);
    useResultAsBButton->setEnabled(false);

    QHBoxLayout *useRow = new QHBoxLayout();
    useRow->addWidget(useResultAsAButton);
    useRow->addWidget(useResultAsBButton);
    mainLayout->addLayout(useRow);

    QGroupBox *histGroup = new QGroupBox("История вычислений", this);
    QVBoxLayout *histLayout = new QVBoxLayout(histGroup);
    historyList = new QListWidget(this);
    clearHistoryButton = new QPushButton("Очистить историю", this);
    histLayout->addWidget(historyList);
    histLayout->addWidget(clearHistoryButton);
    mainLayout->addWidget(histGroup);

    connect(calcButton,         &QPushButton::clicked, this, &TabOrdinals::onCalculate);
    connect(clearHistoryButton, &QPushButton::clicked, this, &TabOrdinals::onClearHistory);
    connect(useResultAsAButton, &QPushButton::clicked, this, &TabOrdinals::onUseResultAsA);
    connect(useResultAsBButton, &QPushButton::clicked, this, &TabOrdinals::onUseResultAsB);

    connect(spinInfA, QOverload<int>::of(&QSpinBox::valueChanged), this, &TabOrdinals::onInputAChanged);
    connect(spinFinA, QOverload<int>::of(&QSpinBox::valueChanged), this, &TabOrdinals::onInputAChanged);
    connect(spinInfB, QOverload<int>::of(&QSpinBox::valueChanged), this, &TabOrdinals::onInputBChanged);
    connect(spinFinB, QOverload<int>::of(&QSpinBox::valueChanged), this, &TabOrdinals::onInputBChanged);
}

void TabOrdinals::onInputAChanged()
{
    updateDisplay(labelDisplayA, spinInfA->value(), spinFinA->value());
}

void TabOrdinals::onInputBChanged()
{
    updateDisplay(labelDisplayB, spinInfB->value(), spinFinB->value());
}

void TabOrdinals::onCalculate()
{
    Ordinal a(spinFinA->value(), spinInfA->value());
    Ordinal b(spinFinB->value(), spinInfB->value());

    int opIndex = operationCombo->currentIndex();

    QString aStr    = ordinalToString(a);
    QString bStr    = ordinalToString(b);
    QString opStr   = operationCombo->currentText();
    QString resultStr;

    if (opIndex == 0) {
        currentResult = a + b;
        hasResult = true;
        resultStr = ordinalToString(currentResult);
        useResultAsAButton->setEnabled(true);
        useResultAsBButton->setEnabled(true);
    } else {
        bool cmp = false;
        if (opIndex == 1) cmp = a < b;
        if (opIndex == 2) cmp = a > b;
        if (opIndex == 3) cmp = a == b;
        if (opIndex == 4) cmp = a <= b;
        if (opIndex == 5) cmp = a >= b;

        hasResult = false;
        useResultAsAButton->setEnabled(false);
        useResultAsBButton->setEnabled(false);
        resultStr = cmp ? "true" : "false";
    }

    labelResult->setText(resultStr);

    QString histEntry = aStr + "  " + opStr + "  " + bStr + "  =  " + resultStr;
    historyList->addItem(histEntry);
    historyList->scrollToBottom();

    if (logArea)
        logArea->append("[Ординалы] " + histEntry);
}

void TabOrdinals::onClearHistory()
{
    historyList->clear();
}

void TabOrdinals::onUseResultAsA()
{
    if (!hasResult) return;
    spinInfA->setValue(currentResult.GetInfinity());
    spinFinA->setValue(currentResult.GetSize());
}

void TabOrdinals::onUseResultAsB()
{
    if (!hasResult) return;
    spinInfB->setValue(currentResult.GetInfinity());
    spinFinB->setValue(currentResult.GetSize());
}
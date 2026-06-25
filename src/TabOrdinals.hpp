#pragma once

#include <QWidget>

#include "../include/LazySequence/ActionChangeOrdinal.hpp"

class QTextEdit;
class QPushButton;
class QComboBox;
class QLabel;
class QListWidget;
class QSpinBox;
class QGroupBox;

class TabOrdinals : public QWidget
{
    Q_OBJECT

private:
    QTextEdit   *logArea;

    QSpinBox    *spinInfA;
    QSpinBox    *spinFinA;
    QLabel      *labelDisplayA;

    QSpinBox    *spinInfB;
    QSpinBox    *spinFinB;
    QLabel      *labelDisplayB;

    QComboBox   *operationCombo;
    QLabel      *labelResult;
    QListWidget *historyList;

    QPushButton *calcButton;
    QPushButton *clearHistoryButton;
    QPushButton *useResultAsAButton;
    QPushButton *useResultAsBButton;

    Ordinal currentResult;
    bool    hasResult;

    void setupLayout();
    void setupGroupA(QGroupBox *box);
    void setupGroupB(QGroupBox *box);
    QString ordinalToString(const Ordinal& ord) const;
    void updateDisplay(QLabel *label, size_t inf, size_t fin);

    void onCalculate();
    void onClearHistory();
    void onUseResultAsA();
    void onUseResultAsB();
    void onInputAChanged();
    void onInputBChanged();

public:
    TabOrdinals(QTextEdit *sharedLogArea, QWidget *parent = nullptr);
    ~TabOrdinals() = default;
};
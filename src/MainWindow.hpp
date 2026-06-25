#pragma once

#include <QMainWindow>

class QTabWidget;
class QTextEdit;
class TabBase;
class TabAlgebra;
class TabOrdinals;

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    QTabWidget  *tabWidget;
    QTextEdit   *logArea;

    TabBase     *tabBase;
    TabAlgebra  *tabAlgebra;
    TabOrdinals *tabOrdinals;

    void setupLayout();

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() = default;
};
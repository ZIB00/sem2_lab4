#include <QTabWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

#include "MainWindow.hpp"
#include "TabBase.hpp"
#include "TabAlgebra.hpp"
#include "TabOrdinals.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setMinimumSize(900, 600);
    setupLayout();
}

void MainWindow::setupLayout()
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    tabWidget = new QTabWidget(centralWidget);
    logArea = new QTextEdit(centralWidget);
    logArea->setReadOnly(true);

    tabBase = new TabBase(logArea, this);
    tabAlgebra = new TabAlgebra(logArea, this);
    tabOrdinals = new TabOrdinals(logArea, this);

    tabWidget->addTab(tabBase, "Базовые операции");
    tabWidget->addTab(tabAlgebra, "Алгебра списков");
    tabWidget->addTab(tabOrdinals, "Алгебра ординалов");

    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(logArea);
}
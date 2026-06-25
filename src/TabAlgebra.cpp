#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QListWidget>
#include <QRegularExpressionValidator>
#include <QStringList>

#include "TabAlgebra.hpp"

TabAlgebra::TabAlgebra(QTextEdit *sharedLogArea, QWidget *parent)
    : QWidget(parent), logArea(sharedLogArea)
{
    setupLayout();

    sequences = new MutableArraySequence<std::shared_ptr<ISequenceWrapper>>();
    currentSeqIndex = -1;

    connect(ListWidget, &QListWidget::currentRowChanged, this, &TabAlgebra::onSequenceSelected);
    connect(CreateFiniteButton, &QPushButton::clicked, this, &TabAlgebra::onCreateFiniteClicked);
    connect(CreateInfiniteNaturalButton, &QPushButton::clicked, this, &TabAlgebra::onCreateInfiniteNaturalClicked);
    connect(MapButton, &QPushButton::clicked, this, &TabAlgebra::onMapClicked);
    connect(WhereButton, &QPushButton::clicked, this, &TabAlgebra::onWhereClicked);
    connect(ReduceButton, &QPushButton::clicked, this, &TabAlgebra::onReduceClicked);
    connect(ZipButton, &QPushButton::clicked, this, &TabAlgebra::onZipClicked);
}

void TabAlgebra::setupLayout()
{
    QHBoxLayout *mainLayout = new QHBoxLayout(this);

    leftArea = new QWidget(this);
    centerArea = new QWidget(this);
    rightArea = new QWidget(this);

    mainLayout->addWidget(leftArea, 0);
    mainLayout->addWidget(centerArea, 1);
    mainLayout->addWidget(rightArea, 0);

    setupLeftLayout();
    setupCenterLayout();
    setupRightLayout();
}

void TabAlgebra::setupLeftLayout()
{
    QVBoxLayout *leftLayout = new QVBoxLayout(leftArea);

    QLabel *CreateFiniteLabel = new QLabel("Элементы:", leftArea);
    CreateFiniteLineEdit = new QLineEdit(leftArea);
    CreateFiniteLineEdit->setPlaceholderText("1 2 3 4 5 ...");

    QRegularExpression rx("^[0-9\\.\\-\\s]*$");
    QValidator *validator = new QRegularExpressionValidator(rx, this);
    CreateFiniteLineEdit->setValidator(validator);

    QLabel *TypeLabel = new QLabel("Тип данных:", leftArea);
    TypeComboBox = new QComboBox(leftArea);
    TypeComboBox->addItem("Целые числа");
    TypeComboBox->addItem("Вещественные числа");

    CreateFiniteButton = new QPushButton("Создать конечную последовательность", leftArea);
    CreateInfiniteNaturalButton = new QPushButton("Натуральные числа (беск.)", leftArea);

    leftLayout->addWidget(CreateFiniteLabel);
    leftLayout->addWidget(CreateFiniteLineEdit);
    leftLayout->addWidget(TypeLabel);
    leftLayout->addWidget(TypeComboBox);
    leftLayout->addWidget(CreateFiniteButton);
    leftLayout->addWidget(CreateInfiniteNaturalButton);
    leftLayout->addStretch();
}

void TabAlgebra::setupCenterLayout()
{
    QVBoxLayout *centerLayout = new QVBoxLayout(centerArea);

    QLabel *listLabel = new QLabel("Доступные последовательности:", centerArea);
    ListWidget = new QListWidget(centerArea);

    QLabel *previewLabel = new QLabel("Предпросмотр (материализованные):", centerArea);
    PreviewArea = new QTextEdit(centerArea);
    PreviewArea->setReadOnly(true);

    centerLayout->addWidget(listLabel);
    centerLayout->addWidget(ListWidget);
    centerLayout->addWidget(previewLabel);
    centerLayout->addWidget(PreviewArea);
}

void TabAlgebra::setupRightLayout()
{
    QVBoxLayout *rightLayout = new QVBoxLayout(rightArea);

    QLabel *MapLabel = new QLabel("Map (текущая):", rightArea);
    MapFuncComboBox = new QComboBox(rightArea);
    MapFuncComboBox->addItem("x * 2");
    MapFuncComboBox->addItem("x * x");
    MapFuncComboBox->addItem("x + 1");
    MapButton = new QPushButton("Применить Map", rightArea);

    QLabel *WhereLabel = new QLabel("Where (текущая):", rightArea);
    WhereFuncComboBox = new QComboBox(rightArea);
    WhereFuncComboBox->addItem("x > 0");
    WhereFuncComboBox->addItem("Чётные");
    WhereButton = new QPushButton("Применить Where", rightArea);

    QLabel *ReduceLabel = new QLabel("Reduce (текущая):", rightArea);
    ReduceFuncComboBox = new QComboBox(rightArea);
    ReduceFuncComboBox->addItem("Сумма");
    ReduceFuncComboBox->addItem("Произведение");
    ReduceFuncComboBox->addItem("Максимум");
    ReduceFuncComboBox->addItem("Минимум");
    ReduceButton = new QPushButton("Применить Reduce", rightArea);

    QLabel *ZipLabel = new QLabel("Zip (текущая + выбранная ниже):", rightArea);
    ZipTargetComboBox = new QComboBox(rightArea);
    QLabel *ZipCountLabel = new QLabel("Сколько пар показать:", rightArea);
    ZipCountLineEdit = new QLineEdit(rightArea);
    ZipCountLineEdit->setText("10");
    QRegularExpression countRx("^[0-9]*$");
    QValidator *countValidator = new QRegularExpressionValidator(countRx, this);
    ZipCountLineEdit->setValidator(countValidator);
    ZipButton = new QPushButton("Применить Zip", rightArea);

    rightLayout->addWidget(MapLabel);
    rightLayout->addWidget(MapFuncComboBox);
    rightLayout->addWidget(MapButton);

    rightLayout->addWidget(WhereLabel);
    rightLayout->addWidget(WhereFuncComboBox);
    rightLayout->addWidget(WhereButton);

    rightLayout->addWidget(ReduceLabel);
    rightLayout->addWidget(ReduceFuncComboBox);
    rightLayout->addWidget(ReduceButton);

    rightLayout->addWidget(ZipLabel);
    rightLayout->addWidget(ZipTargetComboBox);
    rightLayout->addWidget(ZipCountLabel);
    rightLayout->addWidget(ZipCountLineEdit);
    rightLayout->addWidget(ZipButton);

    rightLayout->addStretch();
}

void TabAlgebra::updatePreview()
{
    if (!currentSeq) return;

    PreviewArea->clear();

    const size_t FINITE_LIMIT = 1000;
    const size_t INFINITE_LIMIT = 10;

    QString preview;
    size_t limit = currentSeq->IsLengthInfinite() ? INFINITE_LIMIT : FINITE_LIMIT;

    for (size_t i = 0; i < limit; ++i) {
        try {
            preview += currentSeq->Get(i) + " ";
        } catch (...) {
            break;
        }
    }

    if (currentSeq->IsLengthInfinite()) {
        preview += "...";
    }

    PreviewArea->setText(preview);
}

void TabAlgebra::addSequence(std::shared_ptr<ISequenceWrapper> seq, const QString& name)
{
    sequences->Append(seq);
    ListWidget->addItem(name);
    ZipTargetComboBox->addItem(name);
    ListWidget->setCurrentRow(ListWidget->count() - 1);
    logArea->append("Создана последовательность: " + name);
}

void TabAlgebra::onSequenceSelected(int index)
{
    if (index < 0 || index >= sequences->GetLength()) return;
    currentSeqIndex = index;
    currentSeq = sequences->Get(index);
    updatePreview();
}

void TabAlgebra::onCreateFiniteClicked()
{
    QString text = CreateFiniteLineEdit->text();
    QStringList parts = text.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

    size_t count = parts.size();
    QString name = count == 0 ? "Пустая последовательность" : "Конечная (" + text + ")";

    if (TypeComboBox->currentIndex() == 0) {
        int* arr = new int[count];
        for (size_t i = 0; i < count; ++i) {
            arr[i] = parts[i].toInt();
        }

        auto seq = std::make_shared<MutableArraySequence<int>>(arr, count);
        auto lazySeq = std::make_shared<LazySequence<int>>(seq);
        delete[] arr;

        addSequence(std::make_shared<SequenceWrapperImpl<int>>(lazySeq), name);
    } else {
        double* arr = new double[count];
        for (size_t i = 0; i < count; ++i) {
            arr[i] = parts[i].toDouble();
        }

        auto seq = std::make_shared<MutableArraySequence<double>>(arr, count);
        auto lazySeq = std::make_shared<LazySequence<double>>(seq);
        delete[] arr;

        addSequence(std::make_shared<SequenceWrapperImpl<double>>(lazySeq), name);
    }
}

void TabAlgebra::onCreateInfiniteNaturalClicked()
{
    if (TypeComboBox->currentIndex() == 0) {
        auto lazySeq = std::make_shared<LazySequence<int>>(
            [](auto seq) { return seq->GetLength() + 1; },
            std::initializer_list<int>{1}
        );
        addSequence(std::make_shared<SequenceWrapperImpl<int>>(lazySeq), "Натуральные числа");
    } else {
        auto lazySeq = std::make_shared<LazySequence<double>>(
            [](auto seq) { return seq->GetLength() + 1; },
            std::initializer_list<double>{1.0}
        );
        addSequence(std::make_shared<SequenceWrapperImpl<double>>(lazySeq), "Натуральные числа");
    }
}

void TabAlgebra::onMapClicked()
{
    if (!currentSeq || currentSeqIndex < 0) return;
    QString funcName = MapFuncComboBox->currentText();
    try {
        auto newSeq = currentSeq->MapByName(funcName);
        addSequence(newSeq, "Map(" + funcName + ")");
    } catch (const BaseError& e) {
        logArea->append("Ошибка: " + QString(e.what()));
    }
}

void TabAlgebra::onWhereClicked()
{
    if (!currentSeq || currentSeqIndex < 0) return;
    QString funcName = WhereFuncComboBox->currentText();
    try {
        auto newSeq = currentSeq->WhereByName(funcName);
        addSequence(newSeq, "Where(" + funcName + ")");
    } catch (const BaseError& e) {
        logArea->append("Ошибка: " + QString(e.what()));
    }
}

void TabAlgebra::onReduceClicked()
{
    if (!currentSeq) return;
    QString funcName = ReduceFuncComboBox->currentText();
    try {
        QString result = currentSeq->ReduceByName(funcName);
        logArea->append("Reduce(" + funcName + "): " + result);
    } catch (const BaseError& e) {
        logArea->append("Ошибка: " + QString(e.what()));
    }
}

void TabAlgebra::onZipClicked()
{
    if (!currentSeq) return;
    int targetIndex = ZipTargetComboBox->currentIndex();
    if (targetIndex < 0 || targetIndex >= sequences->GetLength()) return;

    auto otherSeq = sequences->Get(targetIndex);
    size_t count = ZipCountLineEdit->text().isEmpty() ? 10 : ZipCountLineEdit->text().toULongLong();
    QListWidgetItem *currentItem = ListWidget->currentItem();
    if (!currentItem) return;

    try {
        QString result = currentSeq->ZipWith(otherSeq, count);
        logArea->append("Zip(" + currentItem->text() + ", " + ZipTargetComboBox->currentText() + "): " + result);
    } catch (const BaseError& e) {
        logArea->append("Ошибка: " + QString(e.what()));
    }
}
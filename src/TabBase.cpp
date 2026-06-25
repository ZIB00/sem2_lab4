#include "TabBase.hpp"

TabBase::TabBase(QTextEdit *sharedLogArea, QWidget *parent) : QWidget(parent), logArea(sharedLogArea)
{
    setupLayout();

    sequences = new MutableArraySequence<std::shared_ptr<ISequenceWrapper>>();
    currentSeqIndex = -1;

    connect(ListWidget, &QListWidget::currentRowChanged, this, &TabBase::onSequenceSelected);
    connect(CreateFiniteButton, &QPushButton::clicked, this, &TabBase::onCreateFiniteClicked);
    connect(CreateInfiniteFibonacciButton, &QPushButton::clicked, this, &TabBase::onCreateInfiniteFibonacciClicked);
    connect(CreateInfiniteNaturalButton, &QPushButton::clicked, this, &TabBase::onCreateInfiniteNaturalClicked);
    connect(CreateInfiniteAll52Button, &QPushButton::clicked, this, &TabBase::onCreateInfiniteAll52Clicked);
    connect(CreateInfiniteMultiplyBy2Button, &QPushButton::clicked, this, &TabBase::onCreateInfiniteMultiplyBy2Clicked);
    connect(CreateInfiniteNaturalSquareButton, &QPushButton::clicked, this, &TabBase::onCreateInfiniteNaturalSquareClicked);
    connect(getFirstButton, &QPushButton::clicked, this, &TabBase::onGetFirstClicked);
    connect(getLastButton, &QPushButton::clicked, this, &TabBase::onGetLastClicked);
    connect(getButton, &QPushButton::clicked, this, &TabBase::onGetClicked);
    connect(getLengthButton, &QPushButton::clicked, this, &TabBase::onGetLengthClicked);
    connect(getMaterializedCountButton, &QPushButton::clicked, this, &TabBase::onGetMaterializedCountClicked);
    connect(getNextButton, &QPushButton::clicked, this, &TabBase::onGetNextClicked);
    connect(tryGetNextButton, &QPushButton::clicked, this, &TabBase::onTryGetNextClicked);
    connect(setButton, &QPushButton::clicked, this, &TabBase::onSetClicked);
    connect(appendButton, &QPushButton::clicked, this, &TabBase::onAppendClicked);
    connect(prependButton, &QPushButton::clicked, this, &TabBase::onPrependClicked);
    connect(insertAtButton, &QPushButton::clicked, this, &TabBase::onInsertAtClicked);
    connect(removeButton, &QPushButton::clicked, this, &TabBase::onRemoveClicked);
}

void TabBase::setupLayout()
{
    QHBoxLayout *mainLayout = new QHBoxLayout(this);

    topLeftArea = new QWidget(this);
    topCenterArea = new QWidget(this);
    topRightArea = new QWidget(this);

    mainLayout->addWidget(topLeftArea, 0);
    mainLayout->addWidget(topCenterArea, 1);
    mainLayout->addWidget(topRightArea, 0);

    setupTopLeftLayout();
    setupTopCentralLayout();
    setupTopRightLayout();
}

void TabBase::setupTopLeftLayout()
{
    QVBoxLayout *topLeftLayout = new QVBoxLayout(topLeftArea);

    QLabel* CreateFiniteLabel = new QLabel("Элементы:", topLeftArea);
    CreateFiniteLineEdit = new QLineEdit(topLeftArea);
    CreateFiniteLineEdit->setPlaceholderText("1 2 3 4 5 ...");
    
    QRegularExpression rx("^[0-9\\.\\-\\s]*$");
    QValidator *validator = new QRegularExpressionValidator(rx, this);
    CreateFiniteLineEdit->setValidator(validator);

    CreateFiniteButton = new QPushButton("Создание конечной последовательности", topLeftArea);

    QLabel* CreateInfiniteLabel       = new QLabel("Выбрать бесконечные последовательности:", topLeftArea);
    CreateInfiniteFibonacciButton     = new QPushButton("Числа Фибоначчи", topLeftArea);
    CreateInfiniteNaturalButton       = new QPushButton("Натуральные числа", topLeftArea);
    CreateInfiniteAll52Button         = new QPushButton("Все числа это 52", topLeftArea);
    CreateInfiniteMultiplyBy2Button   = new QPushButton("Удвоенные натуральные числа", topLeftArea);
    CreateInfiniteNaturalSquareButton = new QPushButton("Натуральные числа в квадрате", topLeftArea);

    QLabel* TypeLabel = new QLabel("Тип данных:", topLeftArea);
    TypeComboBox = new QComboBox(topLeftArea);
    TypeComboBox->addItem("Целые числа");
    TypeComboBox->addItem("Вещественные числа");

    topLeftLayout->addWidget(CreateFiniteLabel);
    topLeftLayout->addWidget(CreateFiniteLineEdit);
    topLeftLayout->addWidget(CreateFiniteButton);
    topLeftLayout->addWidget(CreateInfiniteLabel);
    topLeftLayout->addWidget(CreateInfiniteFibonacciButton);
    topLeftLayout->addWidget(CreateInfiniteNaturalButton);
    topLeftLayout->addWidget(CreateInfiniteAll52Button);
    topLeftLayout->addWidget(CreateInfiniteMultiplyBy2Button);
    topLeftLayout->addWidget(CreateInfiniteNaturalSquareButton);
    topLeftLayout->addWidget(TypeLabel);
    topLeftLayout->addWidget(TypeComboBox);
    topLeftLayout->addStretch();
}

void TabBase::setupTopCentralLayout()
{
    QVBoxLayout *topCenterLayout = new QVBoxLayout(topCenterArea);

    QLabel *listLabel = new QLabel("Доступные последовательности:", topCenterArea);
    ListWidget = new QListWidget(topCenterArea);

    QLabel *previewLabel = new QLabel("Предпросмотр (материализованные):", topCenterArea);
    PreviewArea = new QTextEdit(topCenterArea);
    PreviewArea->setReadOnly(true);

    topCenterLayout->addWidget(listLabel);
    topCenterLayout->addWidget(ListWidget);
    topCenterLayout->addWidget(previewLabel);
    topCenterLayout->addWidget(PreviewArea);
}

void TabBase::setupTopRightLayout()
{
    QVBoxLayout *topRightLayout = new QVBoxLayout(topRightArea);

    QLabel *valueLabel = new QLabel("Значение (Item):", topRightArea);
    actionValueLineEdit = new QLineEdit(topRightArea);
    
    QLabel *indexLabel = new QLabel("Индекс (Index):", topRightArea);
    actionIndexLineEdit = new QLineEdit(topRightArea);
    
    QRegularExpression valueRx("^-?[0-9]*\\.?[0-9]*$");
    QValidator *valueValidator = new QRegularExpressionValidator(valueRx, this);
    actionValueLineEdit->setValidator(valueValidator);

    QRegularExpression indexRx("^[0-9]*$");
    QValidator *indexValidator = new QRegularExpressionValidator(indexRx, this);
    actionIndexLineEdit->setValidator(indexValidator);

    getFirstButton   = new QPushButton("GetFirst", topRightArea);
    getLastButton    = new QPushButton("GetLast", topRightArea);
    getButton        = new QPushButton("Get (по индексу)", topRightArea);
    getLengthButton  = new QPushButton("GetLength", topRightArea);
    getMaterializedCountButton = new QPushButton("GetMaterializedCount", topRightArea);
    getNextButton    = new QPushButton("GetNext", topRightArea);
    tryGetNextButton = new QPushButton("TryGetNext", topRightArea);
    setButton        = new QPushButton("Set (значение по индексу)", topRightArea);
    appendButton     = new QPushButton("Append (значение)", topRightArea);
    prependButton    = new QPushButton("Prepend (значение)", topRightArea);
    insertAtButton   = new QPushButton("InsertAt (значение по индексу)", topRightArea);
    removeButton     = new QPushButton("Remove (по индексу)", topRightArea);

    topRightLayout->addWidget(valueLabel);
    topRightLayout->addWidget(actionValueLineEdit);
    topRightLayout->addWidget(indexLabel);
    topRightLayout->addWidget(actionIndexLineEdit);
    
    topRightLayout->addWidget(getFirstButton);
    topRightLayout->addWidget(getLastButton);
    topRightLayout->addWidget(getButton);
    topRightLayout->addWidget(getLengthButton);
    topRightLayout->addWidget(getMaterializedCountButton);
    topRightLayout->addWidget(getNextButton);
    topRightLayout->addWidget(tryGetNextButton);
    topRightLayout->addWidget(setButton);
    topRightLayout->addWidget(appendButton);
    topRightLayout->addWidget(prependButton);
    topRightLayout->addWidget(insertAtButton);
    topRightLayout->addWidget(removeButton);

    topRightLayout->addStretch();
}

void TabBase::updatePreview()
{
    if (!currentSeq) return;

    PreviewArea->clear();
    QString preview;

    size_t matCount = currentSeq->GetMaterializedCount();

    for (size_t i = 0; i < matCount; ++i) {
        preview += currentSeq->Get(i) + " ";
    }

    PreviewArea->setText(preview);
}

void TabBase::addSequence(std::shared_ptr<ISequenceWrapper> seq, const QString& name)
{
    sequences->Append(seq);
    ListWidget->addItem(name);
    ListWidget->setCurrentRow(ListWidget->count() - 1);
    logArea->append("Создана последовательность: " + name);
}

void TabBase::onSequenceSelected(int index)
{
    if (index < 0 || index >= sequences->GetLength()) return;
    currentSeqIndex = index;
    currentSeq = sequences->Get(index);
    updatePreview();
}

void TabBase::onCreateFiniteClicked()
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

void TabBase::onCreateInfiniteFibonacciClicked()
{
    if (TypeComboBox->currentIndex() == 0) {
        auto lazySeq = std::make_shared<LazySequence<int>>(
            [](std::shared_ptr<Sequence<int>> seq) {
                size_t len = seq->GetLength();
                return seq->Get(len - 1) + seq->Get(len - 2);
            }, 
        std::initializer_list<int>{1, 1});

        addSequence(std::make_shared<SequenceWrapperImpl<int>>(lazySeq), "Числа Фибоначчи");
    } else {
        auto lazySeq = std::make_shared<LazySequence<double>>(
            [](std::shared_ptr<Sequence<double>> seq) {
                size_t len = seq->GetLength();
                return seq->Get(len - 1) + seq->Get(len - 2);
            }, 
        std::initializer_list<double>{1.0, 1.0});

        addSequence(std::make_shared<SequenceWrapperImpl<double>>(lazySeq), "Числа Фибоначчи");
    }
}

void TabBase::onCreateInfiniteNaturalClicked()
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

void TabBase::onCreateInfiniteAll52Clicked()
{
    if (TypeComboBox->currentIndex() == 0) {
        auto lazySeq = std::make_shared<LazySequence<int>>(
            [](std::shared_ptr<Sequence<int>>) { return 52; }, 
            std::initializer_list<int>{52}
        );
        addSequence(std::make_shared<SequenceWrapperImpl<int>>(lazySeq), "Все числа 52");
    } else {
        auto lazySeq = std::make_shared<LazySequence<double>>(
            [](std::shared_ptr<Sequence<double>>) { return 52.0; }, 
            std::initializer_list<double>{52.0}
        );
        addSequence(std::make_shared<SequenceWrapperImpl<double>>(lazySeq), "Все числа 52");
    }
}

void TabBase::onCreateInfiniteMultiplyBy2Clicked()
{
    if (TypeComboBox->currentIndex() == 0) {
        auto lazySeq = std::make_shared<LazySequence<int>>(
            [](std::shared_ptr<Sequence<int>> seq) { return (seq->GetLength() + 2) * 2; }, 
            std::initializer_list<int>{2}
        );
        addSequence(std::make_shared<SequenceWrapperImpl<int>>(lazySeq), "Удвоенные натуральные числа");
    } else {
        auto lazySeq = std::make_shared<LazySequence<double>>(
            [](std::shared_ptr<Sequence<double>> seq) { return (seq->GetLength() + 2) * 2.0; }, 
            std::initializer_list<double>{2.0}
        );
        addSequence(std::make_shared<SequenceWrapperImpl<double>>(lazySeq), "Удвоенные натуральные числа");
    }
}

void TabBase::onCreateInfiniteNaturalSquareClicked()
{
    if (TypeComboBox->currentIndex() == 0) {
        auto lazySeq = std::make_shared<LazySequence<int>>(
            [](std::shared_ptr<Sequence<int>> seq) {
                int n = seq->GetLength() + 1;
                return n * n;
            }, 
            std::initializer_list<int>{1}
        );
        addSequence(std::make_shared<SequenceWrapperImpl<int>>(lazySeq), "Квадраты натуральных чисел");
    } else {
        auto lazySeq = std::make_shared<LazySequence<double>>(
            [](std::shared_ptr<Sequence<double>> seq) {
                double n = seq->GetLength() + 1;
                return n * n;
            }, 
            std::initializer_list<double>{1.0}
        );
        addSequence(std::make_shared<SequenceWrapperImpl<double>>(lazySeq), "Квадраты натуральных чисел");
    }
}

void TabBase::onGetFirstClicked()
{
    if (!currentSeq) return;
    try {
        QString val = currentSeq->GetFirst();
        logArea->append("GetFirst: " + val);
    } catch (const BaseError& e) {
        logArea->append("Ошибка: " + QString(e.what()));
    }
}

void TabBase::onGetLastClicked()
{
    if (!currentSeq) return;
    try {
        QString val = currentSeq->GetLast();
        logArea->append("GetLast: " + val);
    } catch (const BaseError& e) {
        logArea->append("Ошибка: " + QString(e.what()));
    }
}

void TabBase::onGetClicked()
{
    if (!currentSeq || actionIndexLineEdit->text().isEmpty()) return;
    size_t index = actionIndexLineEdit->text().toULongLong();
    try {
        QString val = currentSeq->Get(index);
        logArea->append("Get(" + QString::number(index) + "): " + val);
        updatePreview();
    } catch (const BaseError& e) {
        logArea->append("Ошибка: " + QString(e.what()));
    }
}

void TabBase::onGetLengthClicked()
{
    if (!currentSeq) return;
    if (currentSeq->IsLengthInfinite()) {
        logArea->append("GetLength: Бесконечность");
    } else {
        logArea->append("GetLength: " + QString::number(currentSeq->GetLengthSize()));
    }
}

void TabBase::onGetMaterializedCountClicked()
{
    if (!currentSeq) return;
    size_t count = currentSeq->GetMaterializedCount();
    logArea->append("GetMaterializedCount: " + QString::number(count));
}

void TabBase::onGetNextClicked()
{
    if (!currentSeq) return;
    try {
        QString val = currentSeq->GetNext();
        logArea->append("GetNext: " + val);
        updatePreview();
    } catch (const BaseError& e) {
        logArea->append("Ошибка: " + QString(e.what()));
    }
}

void TabBase::onTryGetNextClicked()
{
    if (!currentSeq) return;
    QString val;
    if (currentSeq->TryGetNext(val)) {
        logArea->append("TryGetNext: " + val);
        updatePreview();
    } else {
        logArea->append("TryGetNext: Конец последовательности");
    }
}

void TabBase::onSetClicked()
{
    if (!currentSeq || actionIndexLineEdit->text().isEmpty() || actionValueLineEdit->text().isEmpty() || currentSeqIndex < 0) return;
    size_t index = actionIndexLineEdit->text().toULongLong();
    QString value = actionValueLineEdit->text();
    try {
        auto newSeq = currentSeq->Set(value, index);
        addSequence(newSeq, "Set(" + QString::number(index) + ", " + value + ")");
    } catch (const BaseError& e) {
        logArea->append("Ошибка: " + QString(e.what()));
    }
}

void TabBase::onAppendClicked()
{
    if (!currentSeq || actionValueLineEdit->text().isEmpty() || currentSeqIndex < 0) return;
    QString value = actionValueLineEdit->text();
    try {
        auto newSeq = currentSeq->Append(value);
        addSequence(newSeq, "Append(" + value + ")");
    } catch (const BaseError& e) {
        logArea->append("Ошибка: " + QString(e.what()));
    }
}

void TabBase::onPrependClicked()
{
    if (!currentSeq || actionValueLineEdit->text().isEmpty() || currentSeqIndex < 0) return;
    QString value = actionValueLineEdit->text();
    try {
        auto newSeq = currentSeq->Prepend(value);
        addSequence(newSeq, "Prepend(" + value + ")");
    } catch (const BaseError& e) {
        logArea->append("Ошибка: " + QString(e.what()));
    }
}

void TabBase::onInsertAtClicked()
{
    if (!currentSeq || actionIndexLineEdit->text().isEmpty() || actionValueLineEdit->text().isEmpty() || currentSeqIndex < 0) return;
    size_t index = actionIndexLineEdit->text().toULongLong();
    QString value = actionValueLineEdit->text();
    try {
        auto newSeq = currentSeq->InsertAt(value, index);
        addSequence(newSeq, "InsertAt(" + QString::number(index) + ", " + value + ")");
    } catch (const BaseError& e) {
        logArea->append("Ошибка: " + QString(e.what()));
    }
}

void TabBase::onRemoveClicked()
{
    if (!currentSeq || actionIndexLineEdit->text().isEmpty() || currentSeqIndex < 0) return;
    size_t index = actionIndexLineEdit->text().toULongLong();
    try {
        auto newSeq = currentSeq->Remove(index);
        addSequence(newSeq, "Remove(" + QString::number(index) + ")");
    } catch (const BaseError& e) {
        logArea->append("Ошибка: " + QString(e.what()));
    }
}
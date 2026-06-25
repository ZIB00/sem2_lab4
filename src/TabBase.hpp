#pragma once
#include <memory>
#include <QWidget>
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

#include "SequenceWrapper.hpp"

class TabBase : public QWidget
{
    Q_OBJECT

private:
    std::shared_ptr<ISequenceWrapper> currentSeq;
    MutableArraySequence<std::shared_ptr<ISequenceWrapper>>* sequences;
    int currentSeqIndex;

    void addSequence(std::shared_ptr<ISequenceWrapper> seq, const QString& name);
    
    QTextEdit *logArea;

    QWidget *topLeftArea;
    QWidget *topCenterArea;
    QWidget *topRightArea;

    QLineEdit   *CreateFiniteLineEdit;
    QPushButton *CreateFiniteButton;
    QPushButton *CreateInfiniteFibonacciButton;
    QPushButton *CreateInfiniteNaturalButton;
    QPushButton *CreateInfiniteAll52Button;
    QPushButton *CreateInfiniteMultiplyBy2Button;
    QPushButton *CreateInfiniteNaturalSquareButton;
    QComboBox   *TypeComboBox;

    QListWidget *ListWidget;
    QTextEdit   *PreviewArea;

    QLineEdit   *actionValueLineEdit;
    QLineEdit   *actionIndexLineEdit;
    QPushButton *getFirstButton;
    QPushButton *getLastButton;
    QPushButton *getButton;
    QPushButton *getLengthButton;
    QPushButton *getMaterializedCountButton;
    QPushButton *getNextButton;
    QPushButton *tryGetNextButton;
    QPushButton *setButton;
    QPushButton *appendButton;
    QPushButton *prependButton;
    QPushButton *insertAtButton;
    QPushButton *removeButton;

    void setupLayout();
    void setupTopLeftLayout();
    void setupTopCentralLayout();
    void setupTopRightLayout();

public:
    TabBase(QTextEdit *sharedLogArea, QWidget *parent = nullptr);
    ~TabBase() = default;

    void updatePreview();
    void onSequenceSelected(int index);

    void onCreateFiniteClicked();
    void onCreateInfiniteFibonacciClicked();
    void onCreateInfiniteNaturalClicked();
    void onCreateInfiniteAll52Clicked();
    void onCreateInfiniteMultiplyBy2Clicked();
    void onCreateInfiniteNaturalSquareClicked();
    void onGetFirstClicked();
    void onGetLastClicked();
    void onGetClicked();
    void onGetLengthClicked();
    void onGetMaterializedCountClicked();
    void onGetNextClicked();
    void onTryGetNextClicked();
    void onSetClicked();
    void onAppendClicked();
    void onPrependClicked();
    void onInsertAtClicked();
    void onRemoveClicked();
};
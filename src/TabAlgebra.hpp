#pragma once

#include <QWidget>

#include "SequenceWrapper.hpp"

class QTextEdit;
class QLineEdit;
class QPushButton;
class QComboBox;
class QLabel;
class QListWidget;

class TabAlgebra : public QWidget
{
    Q_OBJECT

private:
    std::shared_ptr<ISequenceWrapper> currentSeq;
    MutableArraySequence<std::shared_ptr<ISequenceWrapper>>* sequences;
    int currentSeqIndex;

    void addSequence(std::shared_ptr<ISequenceWrapper> seq, const QString& name);

    QTextEdit *logArea;

    QWidget *leftArea;
    QWidget *centerArea;
    QWidget *rightArea;

    QLineEdit   *CreateFiniteLineEdit;
    QComboBox   *TypeComboBox;
    QPushButton *CreateFiniteButton;
    QPushButton *CreateInfiniteNaturalButton;

    QListWidget *ListWidget;
    QTextEdit   *PreviewArea;

    QComboBox   *MapFuncComboBox;
    QPushButton *MapButton;

    QComboBox   *WhereFuncComboBox;
    QPushButton *WhereButton;

    QComboBox   *ReduceFuncComboBox;
    QPushButton *ReduceButton;

    QComboBox   *ZipTargetComboBox;
    QLineEdit   *ZipCountLineEdit;
    QPushButton *ZipButton;

    void setupLayout();
    void setupLeftLayout();
    void setupCenterLayout();
    void setupRightLayout();

public:
    TabAlgebra(QTextEdit *sharedLogArea, QWidget *parent = nullptr);
    ~TabAlgebra() = default;

    void updatePreview();
    void onSequenceSelected(int index);

    void onCreateFiniteClicked();
    void onCreateInfiniteNaturalClicked();
    void onMapClicked();
    void onWhereClicked();
    void onReduceClicked();
    void onZipClicked();
};

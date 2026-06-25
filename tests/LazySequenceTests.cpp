#include <gtest/gtest.h>
#include <initializer_list>

#include "LazySequence/LazySequence.hpp"
#include "Sequences/other/Exceptions.hpp"
#include "TestUtils.hpp"
#include "../include/Sequences/sequences/ArraySequence.hpp"
#include "../include/Sequences/sequences/ListSequence.hpp"

TEST(LazySequenceTest, GetMaterializedCountIsZeroInTheBegining) {
    LazySequence<int> seq([](auto list) {return 1;});

    MutableArraySequence<int> expected = {};

    EXPECT_EQ(seq.GetMaterializedCount(), expected.GetLength());
}

TEST(LazySequenceTest, RemoveShiftMathCompact) {
    LazySequence<int> seq(std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{10, 20, 30, 40, 50}));

    LazySequence<int> modSeq = seq.Remove(2);
    MutableArraySequence<int> expected = {10, 20, 40, 50};

    LAZY_EQ(modSeq, expected, "Проверка удаления элемента по индексу 2");
}

TEST(LazySequenceTest, EmptySequenceLengthIsZero) {
    LazySequence<int> seq;

    EXPECT_EQ(seq.GetLength().GetSize(), 0);
}

TEST(LazySequenceTest, EmptySequenceIsFinite) {
    LazySequence<int> seq;
    
    EXPECT_FALSE(seq.GetLength().IsInfinite());
}

TEST(LazySequenceTest, InitFromInitializerListHasCorrectLength) {
    LazySequence<int> seq(std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{1, 2, 3}));
    
    EXPECT_EQ(seq.GetLength().GetSize(), 3);
}

TEST(LazySequenceTest, InitFromRuleHasCorrectMaterialization) {
    LazySequence<int> seq([](auto list) { return 0; }, std::make_shared<MutableListSequence<int>>());

    EXPECT_EQ(seq.GetMaterializedCount(), 0);
}

TEST(LazySequenceTest, InitFromRuleIsFinite) {
    LazySequence<int> seq([](auto list) { return 0; }, std::make_shared<MutableListSequence<int>>());

    EXPECT_TRUE(seq.GetLength().IsInfinite());
}

TEST(LazySequenceTest, GetMaterializesElementsToTargetIndex) {
    LazySequence<size_t> seq([](auto list) { return (list->GetLength() * 2); }, 
                                std::make_shared<MutableListSequence<size_t>>());
    
    seq.Get(4);
    
    EXPECT_EQ(seq.GetMaterializedCount(), 5);
}

TEST(LazySequenceTest, GetFirstMaterializesOnlyOneElement) {
    LazySequence<int> seq([](auto list) { return 52; }, std::make_shared<MutableListSequence<int>>());

    seq.GetFirst();

    EXPECT_EQ(seq.GetMaterializedCount(), 1);
}

TEST(LazySequenceTest, CachingPreventsReevaluationV1) {
    LazySequence<size_t> seq([](auto list) { return (list->GetLength()); }, std::make_shared<MutableListSequence<size_t>>());
    
    seq.Get(10);

    EXPECT_EQ(seq.GetMaterializedCount(), 11);
}

TEST(LazySequenceTest, CachingPreventsReevaluationV2) {
    LazySequence<size_t> seq([](auto list) { return (list->GetLength()); }, std::make_shared<MutableListSequence<size_t>>());
    
    seq.Get(10);
    seq.Get(5);

    EXPECT_EQ(seq.GetMaterializedCount(), 11);
}

TEST(LazySequenceTest, SetDoesNotModifyOriginal) {
    LazySequence<int> seq(std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{1, 2, 3}));
    
    seq.Set(52, 1);
    
    EXPECT_EQ(seq.Get(1), 2);
}

TEST(LazySequenceTest, InsertDoesNotModifyOriginal) {
    LazySequence<int> seq(std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{1, 2, 3}));
    
    seq.InsertAt(52, 1);
    
    EXPECT_EQ(seq.GetLength().GetSize(), 3);
}

TEST(LazySequenceTest, RemoveDoesNotModifyOriginalGetLength) {
    LazySequence<int> seq(std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{1, 2, 3}));
    
    seq.Remove(1);
    
    EXPECT_EQ(seq.GetLength().GetSize(), 3);
}

TEST(LazySequenceTest, RemoveDoesNotModifyOriginalGet) {
    LazySequence<int> seq(std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{1, 2, 3}));
    
    seq.Remove(1);
    
    EXPECT_EQ(seq.Get(1), 2);
}

TEST(LazySequenceTest, MultipleBranchesFromOneBaseV1) {
    LazySequence<int> seq(std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{1, 2, 3}));
    LazySequence<int> seq1 = seq.Set(10, 1);
    LazySequence<int> seq2 = seq.Set(20, 1);
    EXPECT_EQ(seq1.Get(1), 10);
}

TEST(LazySequenceTest, MultipleBranchesFromOneBaseV2) {
    LazySequence<int> seq(std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{1, 2, 3}));
    LazySequence<int> seq1 = seq.Set(10, 1);
    LazySequence<int> seq2 = seq.Set(20, 1);
    EXPECT_EQ(seq2.Get(1), 20);
}

TEST(LazySequenceTest, MultipleBranchesFromOneBaseV3) {
    LazySequence<int> seq(std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{1, 2, 3}));
    LazySequence<int> seq1 = seq.Set(10, 1);
    LazySequence<int> seq2 = seq.Set(20, 1);
    EXPECT_EQ(seq.Get(1), 2);
}

TEST(LazySequenceTest, SetFirstElement) {
    LazySequence<int> seq(std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{10, 20, 30}));
    
    LazySequence<int> modSeq = seq.Set(52, 0);
    MutableArraySequence<int> expected = {52, 20, 30};

    LAZY_EQ(modSeq, expected, "Set(0)");
}

TEST(LazySequenceTest, SetMiddleElement) {
    LazySequence<int> seq(std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{10, 20, 30}));
    
    LazySequence<int> modSeq = seq.Set(52, 1);
    MutableArraySequence<int> expected = {10, 52, 30};

    LAZY_EQ(modSeq, expected, "Set(1)");
}

TEST(LazySequenceTest, SetLastElement) {
    LazySequence<int> seq(std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{10, 20, 30}));
    
    LazySequence<int> modSeq = seq.Set(52, 2);
    MutableArraySequence<int> expected = {10, 20, 52};

    LAZY_EQ(modSeq, expected, "Set(2)");
}

TEST(LazySequenceTest, SetMultipleTimesOnSameIndex) {
    LazySequence<int> seq(std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{10, 20, 30}));
    
    LazySequence<int> modSeq = seq.Set(52, 1).Set(228, 1);
    MutableArraySequence<int> expected = {10, 228, 30};

    LAZY_EQ(modSeq, expected, "Set(1) -> Set(1)");
}

TEST(LazySequenceTest, SetChainedMultipleIndices) {
    LazySequence<int> seq(std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{10, 20, 30}));
    
    LazySequence<int> modSeq = seq.Set(52, 0).Set(228, 2);
    MutableArraySequence<int> expected = {52, 20, 228};

    LAZY_EQ(modSeq, expected, "Set(0) -> Set(2)");
}

TEST(LazySequenceTest, InsertAtZero) {
    LazySequence<int> seq(std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{10, 20}));
    
    LazySequence<int> modSeq = seq.InsertAt(52, 0);
    MutableArraySequence<int> expected = {52, 10, 20};

    LAZY_EQ(modSeq, expected, "InsertAt(0)");
}

TEST(LazySequenceTest, InsertAtMiddle) {
    LazySequence<int> seq(std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{10, 20, 30}));
    
    LazySequence<int> modSeq = seq.InsertAt(52, 1);
    MutableArraySequence<int> expected = {10, 52, 20, 30};

    LAZY_EQ(modSeq, expected, "InsertAt(1)");
}

TEST(LazySequenceTest, InsertAtEnd) {
    LazySequence<int> seq(std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{10, 20}));
    
    LazySequence<int> modSeq = seq.InsertAt(52, 2);
    MutableArraySequence<int> expected = {10, 20, 52};

    LAZY_EQ(modSeq, expected, "InsertAt(2)");
}

TEST(LazySequenceTest, PrependElement) {
    LazySequence<int> seq(std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{10, 20}));
    
    LazySequence<int> modSeq = seq.Prepend(52);
    MutableArraySequence<int> expected = {52, 10, 20};

    LAZY_EQ(modSeq, expected, "Prepend()");
}

TEST(LazySequenceTest, AppendElement) {
    LazySequence<int> seq(std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{10, 20}));
    
    LazySequence<int> modSeq = seq.Append(52);
    MutableArraySequence<int> expected = {10, 20, 52};

    LAZY_EQ(modSeq, expected, "Append()");
}

TEST(LazySequenceTest, MultiplePrepends) {
    LazySequence<int> seq(std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{10}));
    
    LazySequence<int> modSeq = seq.Prepend(20).Prepend(30);
    MutableArraySequence<int> expected = {30, 20, 10};

    LAZY_EQ(modSeq, expected, "Prepend() -> Prepend()");
}

TEST(LazySequenceTest, MultipleAppends) {
    LazySequence<int> seq(std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{10}));
    
    LazySequence<int> modSeq = seq.Append(20).Append(30);
    MutableArraySequence<int> expected = {10, 20, 30};

    LAZY_EQ(modSeq, expected, "Append() -> Append()");
}

TEST(LazySequenceTest, RemoveFirstElement) {
    LazySequence<int> seq(std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{10, 20, 30}));
    
    LazySequence<int> modSeq = seq.Remove(0);
    MutableArraySequence<int> expected = {20, 30};

    LAZY_EQ(modSeq, expected, "Remove(0)");
}

TEST(LazySequenceTest, RemoveMiddleElement) {
    LazySequence<int> seq(std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{10, 20, 30}));
    
    LazySequence<int> modSeq = seq.Remove(1);
    MutableArraySequence<int> expected = {10, 30};

    LAZY_EQ(modSeq, expected, "Remove(1)");
}

TEST(LazySequenceTest, RemoveLastElement) {
    LazySequence<int> seq(std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{10, 20, 30}));
    
    LazySequence<int> modSeq = seq.Remove(2);
    MutableArraySequence<int> expected = {10, 20};

    LAZY_EQ(modSeq, expected, "Remove(2)");
}

TEST(LazySequenceTest, RemoveMultipleConsecutive) {
    LazySequence<int> seq(std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{10, 20, 30, 40}));
    
    LazySequence<int> modSeq = seq.Remove(1).Remove(1);
    MutableArraySequence<int> expected = {10, 40};

    LAZY_EQ(modSeq, expected, "Remove(1) -> Remove(1)");
}

TEST(LazySequenceTest, RemoveAllElements) {
    LazySequence<int> seq(std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{10, 20}));
    
    LazySequence<int> modSeq = seq.Remove(0).Remove(0);
    
    EXPECT_EQ(modSeq.GetLength().GetSize(), 0);
}

TEST(LazySequenceTest, InsertThenRemoveSameIndex) {
    LazySequence<int> seq(std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{10, 20, 30}));
    
    LazySequence<int> modSeq = seq.InsertAt(52, 1).Remove(1);
    MutableArraySequence<int> expected = {10, 20, 30};

    LAZY_EQ(modSeq, expected, "InsertAt(1) -> Remove(1)");
}

TEST(LazySequenceTest, SetThenRemoveSameIndex) {
    LazySequence<int> seq(std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{10, 20, 30}));
    
    LazySequence<int> modSeq = seq.Set(52, 1).Remove(1);
    MutableArraySequence<int> expected = {10, 30};

    LAZY_EQ(modSeq, expected, "Set(1) -> Remove(1)");
}

TEST(LazySequenceTest, RemoveThenInsertSameIndex) {
    LazySequence<int> seq(std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{10, 20, 30}));
    
    LazySequence<int> modSeq = seq.Remove(1).InsertAt(52, 1);
    MutableArraySequence<int> expected = {10, 52, 30};

    LAZY_EQ(modSeq, expected, "Remove(1) -> InsertAt(1)");
}

TEST(LazySequenceTest, SetThenInsertBefore) {
    LazySequence<int> seq(std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{10, 20, 30}));
    
    LazySequence<int> modSeq = seq.Set(52, 1).InsertAt(228, 0);
    MutableArraySequence<int> expected = {228, 10, 52, 30};

    LAZY_EQ(modSeq, expected, "Set(1) -> InsertAt(0)");
}

TEST(LazySequenceTest, InsertThenSetAfter) {
    LazySequence<int> seq(std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{10, 20, 30}));
    
    LazySequence<int> modSeq = seq.InsertAt(52, 1).Set(228, 2);
    MutableArraySequence<int> expected = {10, 52, 228, 30};

    LAZY_EQ(modSeq, expected, "InsertAt(1) -> Set(2)");
}

TEST(LazySequenceTest, SubsequenceFull) {
    LazySequence<int> seq(std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{10, 20, 30}));
    
    LazySequence<int> sub = seq.GetSubsequence(0, 2);
    MutableArraySequence<int> expected = {10, 20, 30};

    LAZY_EQ(sub, expected, "Subsequence(0, 2)");
}

TEST(LazySequenceTest, SubsequenceMiddle) {
    LazySequence<int> seq(std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{10, 20, 30, 40, 50}));
    
    LazySequence<int> sub = seq.GetSubsequence(1, 3);
    MutableArraySequence<int> expected = {20, 30, 40};

    LAZY_EQ(sub, expected, "Subsequence(1, 3)");
}

TEST(LazySequenceTest, SubsequenceSingleElement) {
    LazySequence<int> seq(std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{10, 20, 30}));
    
    LazySequence<int> sub = seq.GetSubsequence(1, 1);
    MutableArraySequence<int> expected = {20};

    LAZY_EQ(sub, expected, "Subsequence(1, 1)");
}

TEST(LazySequenceTest, SubsequenceFromModified) {
    LazySequence<int> seq(std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{10, 20, 30, 40}));
    
    LazySequence<int> modSeq = seq.Remove(1).InsertAt(52, 1);
    LazySequence<int> sub = modSeq.GetSubsequence(1, 2);
    MutableArraySequence<int> expected = {52, 30};

    LAZY_EQ(sub, expected, "Subsequence on modified");
}

TEST(LazySequenceTest, ConcatTwoFinite) {
    LazySequence<int> seq1(std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{1, 2}));
    LazySequence<int> seq2(std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{3, 4}));
    
    LazySequence<int> res = seq1.Concat(seq2);
    MutableArraySequence<int> expected = {1, 2, 3, 4};

    LAZY_EQ(res, expected, "Concat two finite");
}

TEST(LazySequenceTest, ConcatEmptyWithFinite) {
    LazySequence<int> seq1(std::make_shared<MutableListSequence<int>>());
    LazySequence<int> seq2(std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{1, 2}));
    
    LazySequence<int> res = seq1.Concat(seq2);
    MutableArraySequence<int> expected = {1, 2};

    LAZY_EQ(res, expected, "Concat empty and finite");
}

TEST(LazySequenceTest, ConcatModifiedSequences) {
    LazySequence<int> seq1(std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{10, 20}));
    LazySequence<int> seq2(std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{30, 40}));
    
    LazySequence<int> res = seq1.Remove(1).Concat(seq2.InsertAt(52, 0));
    MutableArraySequence<int> expected = {10, 52, 30, 40};

    LAZY_EQ(res, expected, "Concat modified sequences");
}

TEST(LazySequenceTest, LengthAfterAppends) {
    LazySequence<int> seq(std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{10}));
    
    LazySequence<int> modSeq = seq.Append(20).Append(30);

    EXPECT_EQ(modSeq.GetLength().GetSize(), 3);
}

TEST(LazySequenceTest, LengthAfterRemoves) {
    LazySequence<int> seq(std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{10, 20, 30}));
    
    LazySequence<int> modSeq = seq.Remove(0).Remove(0);
    
    EXPECT_EQ(modSeq.GetLength().GetSize(), 1);
}

TEST(LazySequenceTest, LengthAfterConcat) {
    LazySequence<int> seq1(std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{10, 20}));
    LazySequence<int> seq2(std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{30, 40, 50}));
    
    LazySequence<int> res = seq1.Concat(seq2);

    EXPECT_EQ(res.GetLength().GetSize(), 5);
}

TEST(LazySequenceTest, GetOutOfBoundsEmpty) {
    LazySequence<int> seq(std::make_shared<MutableListSequence<int>>());
    
    EXPECT_THROW(seq.Get(0), OutOfRange);
}

TEST(LazySequenceTest, GetOutOfBoundsFinite) {
    LazySequence<int> seq(std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{10, 20}));
    
    EXPECT_THROW(seq.Get(2), OutOfRange);
}

TEST(LazySequenceTest, GetLastOnEmpty) {
    LazySequence<int> seq(std::make_shared<MutableListSequence<int>>());
    
    EXPECT_THROW(seq.GetLast(), OutOfRange);
}

TEST(LazySequenceTest, SubsequenceInvalidRange) {
    LazySequence<int> seq(std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{10, 20}));
    
    EXPECT_THROW(seq.GetSubsequence(1, 0), LogicError);
}

TEST(LazySequenceTest, AppendToInfiniteThrows) {
    LazySequence<int> seq([](auto list) { return 1; });
    
    EXPECT_THROW(seq.Append(52), OutOfRange);
}

TEST(LazySequenceTest, GetLastOnInfiniteThrows) {
    LazySequence<int> seq([](auto list) { return 1; });
    
    EXPECT_THROW(seq.GetLast(), OutOfRange);
}

TEST(LazySequenceTest, InfiniteConcatFiniteIsInfinite) {
    LazySequence<int> seq1([](auto list) { return 1; });
    LazySequence<int> seq2(std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{10, 20}));
    
    LazySequence<int> res = seq1.Concat(seq2);

    EXPECT_TRUE(res.GetLength().IsInfinite());
}

TEST(LazySequenceTest, GetNextWork) {
    LazySequence<int> seq( [](auto list) { return 1; } );

    EXPECT_EQ(seq.GetNext(), 1);
}

TEST(LazySequenceTest, GetNextManyCalls) {
    LazySequence<size_t> seq([](auto list) { return (list->GetLength()); });
    
    seq.GetNext();
    seq.GetNext();
    seq.GetNext();

    EXPECT_EQ(seq.GetNext(), 3);
}

TEST(LazySequenceTest, TryGetNextwork) {
    LazySequence<size_t> seq([](auto list) { return (list->GetLength()); });

    EXPECT_EQ(seq.TryGetNext().Value(), 0);
}

TEST(LazySequenceTest, TryGetNextworkWithEmptySequenceValueOr) {
    LazySequence<size_t> seq(std::make_shared<MutableListSequence<size_t>>());

    EXPECT_EQ(seq.TryGetNext().ValueOr(52), 52);
}

TEST(LazySequenceTest, TryGetNextworkWithEmptySequenceValue) {
    LazySequence<size_t> seq(std::make_shared<MutableListSequence<size_t>>());

    EXPECT_THROW(seq.TryGetNext().Value(), BadOptionalAccess);
}
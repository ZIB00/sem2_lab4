#include <gtest/gtest.h>
#include <memory>

#include "../include/LazySequence/LazySequence.hpp"
#include "LazySequence/LazySequence.hpp"
#include "Sequences/other/Exceptions.hpp"
#include "Sequences/sequences/ListSequence.hpp"
#include "TestUtils.hpp"

// Тесты для Map

TEST(LazySequenceUtiltsTests, MapWork) {
    auto seq = std::make_shared<LazySequence<int>>(std::make_shared<MutableListSequence<int>>(MutableListSequence<int>({1, 2, 3, 4})));

    LazySequence<int> res = seq->Map<int>( [](int item) -> int {return item * 2;} );

    LAZY_EQ2(res, {2, 4, 6, 8}, "Map должен умножить все элементы на 2");
}

TEST(LazySequenceUtiltsTests, MapT1ToT2) {
    auto seq = std::make_shared<LazySequence<int>>(std::make_shared<MutableListSequence<int>>(MutableListSequence<int>({1, 2, 3, 4})));

    LazySequence<double> res = seq->Map<double>( [](int item) -> double {return static_cast<double>(item) / 2;} );

    LAZY_EQ2(res, {0.5, 1.0, 1.5, 2.0}, "Map должен разделить все элементы на 2");
} 

TEST(LazySequenceUtiltsTests, MapWithInfinite) {
    auto seq = std::make_shared<LazySequence<int>>( [](auto list) {return (list->GetLength());});

    LazySequence<double> res = seq->Map<double>( [](int item) -> double {return static_cast<double>(item) / 2;} );

    for( int i = 0; i < 10; ++i ) {
        EXPECT_EQ(seq->Get(i), i) << "Элемент должен совпадать с его индексом, i — индекс";
    }
} 

TEST(LazySequenceUtiltsTests, MapEmptySequence) {
    auto seq = std::make_shared<LazySequence<int>>(std::make_shared<MutableListSequence<int>>(MutableListSequence<int>({})));

    LazySequence<int> res = seq->Map<int>( [](int item) -> int {return item * 2;} );

    LAZY_EQ2(res, {}, "Map от пустой последовательности должен возвращать пустую");
}

// Where

TEST(LazySequenceUtiltsTests, WhereWork) {
    auto seq = std::make_shared<LazySequence<int>>(std::make_shared<MutableListSequence<int>>(MutableListSequence<int>({1, 2, 3, 4})));

    LazySequence<int> res = seq->Where( [](int item) -> bool {return item > 2;} );

    LAZY_EQ2(res, {3, 4}, "Генератор должен оставить элементы больше 2");
}

TEST(LazySequenceUtiltsTests, WhereWithInfinite) {
    auto seq = std::make_shared<LazySequence<int>>( [](auto list) {return (list->GetLength());});

    for( int i = 0; i < 10; ++i ) EXPECT_EQ(seq->Get(i), i) << "Элемент должен совпадать с его индексом, i — индекс";
    
    LazySequence<int> res = seq->Where( [](int item) -> bool {return item > 3;} );

    LAZY_EQ2(res, {4, 5, 6, 7, 8, 9}, "Проверка чисел до 9 на то, больше ли они 3");
} 

TEST(LazySequenceUtiltsTests, WhereNoMatch) {
    auto seq = std::make_shared<LazySequence<int>>(std::make_shared<MutableListSequence<int>>(MutableListSequence<int>({1, 2, 3, 4})));

    LazySequence<int> res = seq->Where( [](int item) -> bool {return item > 10;} );

    LAZY_EQ2(res, {}, "Where должен вернуть пустую последовательность, если ни один элемент не подошел");
}

TEST(LazySequenceUtiltsTests, WhereEmptySequence) {
    auto seq = std::make_shared<LazySequence<int>>(std::make_shared<MutableListSequence<int>>(MutableListSequence<int>({})));

    LazySequence<int> res = seq->Where( [](int item) -> bool {return item > 0;} );

    LAZY_EQ2(res, {}, "Where от пустой последовательности должен возвращать пустую");
}

// Reduce

TEST(LazySequenceUtiltsTests, ReduceWork) {
    auto seq = std::make_shared<LazySequence<int>>(std::make_shared<MutableListSequence<int>>(MutableListSequence<int>({1, 2, 3, 4})));

    int res = seq->Reduce<int>( [](int res, int item) -> int {return res += item;} );
    EXPECT_EQ(res, 10) << "1 + 2 + 3 + 4";
} 

TEST(LazySequenceUtiltsTests, ReduceT1ToT2) {
    auto seq = std::make_shared<LazySequence<int>>(std::make_shared<MutableListSequence<int>>(MutableListSequence<int>({10, 2, 2})));

    double res = seq->Reduce<double>( [](double res, int item) -> double {return res / item;} );
    EXPECT_EQ(res, 2.5) << "10 / 2 / 2";
} 

TEST(LazySequenceUtiltsTests, ReduceWithInfinite) {
    auto seq = std::make_shared<LazySequence<int>>( [](auto list) {return 1;} );

    EXPECT_THROW(seq->Reduce<double>( [](int res, int item) -> int {return res / item;} ), LogicError );
} 

TEST(LazySequenceUtiltsTests, ReduceEmptySequenceThrows) {
    auto seq = std::make_shared<LazySequence<int>>(std::make_shared<MutableListSequence<int>>(MutableListSequence<int>({})));

    EXPECT_THROW(seq->Reduce<int>( [](int res, int item) -> int {return res + item;} ), InvalidArgument);
}

// Zip

TEST(LazySequenceUtiltsTests, ZipSameLength) {
    auto seq1 = std::make_shared<LazySequence<int>>(std::make_shared<MutableListSequence<int>>(MutableListSequence<int>({1, 2, 3})));
    auto seq2 = std::make_shared<LazySequence<double>>(std::make_shared<MutableListSequence<double>>(MutableListSequence<double>({1.5, 2.5, 3.5})));

    auto res = Zip<int, double>(seq1, seq2);

    EXPECT_EQ(res.Get(0), std::make_pair(1, 1.5)) << "Проверка 1 элемента";
    EXPECT_EQ(res.Get(1), std::make_pair(2, 2.5)) << "Проверка 2 элемента";
    EXPECT_EQ(res.Get(2), std::make_pair(3, 3.5)) << "Проверка 3 элемента";
    EXPECT_EQ(res.GetLength().GetSize(), 3) << "Размер итоговой равен размеру исходных";
}

TEST(LazySequenceUtiltsTests, ZipDifferentLengths) {
    auto seq1 = std::make_shared<LazySequence<int>>(std::make_shared<MutableListSequence<int>>(MutableListSequence<int>({1, 2})));
    auto seq2 = std::make_shared<LazySequence<int>>(std::make_shared<MutableListSequence<int>>(MutableListSequence<int>({10, 20, 30, 40})));

    auto res = Zip<int, int>(seq1, seq2);
    
    EXPECT_EQ(res.Get(0), std::make_pair(1, 10)) << "Проверка 1 элемента";
    EXPECT_EQ(res.Get(1), std::make_pair(2, 20)) << "Проверка 2 элемента";
    EXPECT_EQ(res.GetLength().GetSize(), 2) << "Размер итоговой равен размеру наименьшего";
}

TEST(LazySequenceUtiltsTests, ZipFiniteWithInfinite) {
    auto seq1 = std::make_shared<LazySequence<int>>(std::make_shared<MutableListSequence<int>>(MutableListSequence<int>({1, 2, 3})));
    auto seqInfinite = std::make_shared<LazySequence<int>>( [](auto list) {return list->GetLength();} );

    auto res = Zip<int, int>(seq1, seqInfinite);

    EXPECT_FALSE(res.GetLength().IsInfinite()) << "Проверка размера бесконечного";
    EXPECT_EQ(res.GetLength().GetSize(), 3) << "Проверка размера меньшего";
    EXPECT_EQ(res.Get(2), std::make_pair(3, 2)) << "Размер итоговой равен размеру наименьшего";
}

TEST(LazySequenceUtiltsTests, ZipTwoInfinite) {
    auto seqInf1 = std::make_shared<LazySequence<int>>( [](auto list) {return list->GetLength();} );
    auto seqInf2 = std::make_shared<LazySequence<int>>( [](auto list) {return list->GetLength() * 2;} );

    auto res = Zip<int, int>(seqInf1, seqInf2);

    EXPECT_TRUE(res.GetLength().IsInfinite()) << "Проверка бесконечности итога";
    EXPECT_EQ(res.Get(5), std::make_pair(5, 10)) << "Проверка пятого(случайного) элемента";
}

// Цепь

TEST(LazySequenceUtiltsTests, ChainOperations) {
    auto seq = std::make_shared<LazySequence<int>>(std::make_shared<MutableListSequence<int>>(MutableListSequence<int>({1, 2, 3, 4, 5})));

    auto map = std::make_shared<LazySequence<int>>( seq->Map<int>([](int x) -> int { return x * 10; }) );
    
    LAZY_EQ2(*map, {10, 20, 30, 40, 50}, "Проверка умножения на 10");
    
    int result = map->Reduce<int>([](int sum, int x) -> int { return sum + x; });

    EXPECT_EQ(result, 150) << "суммирование всей последовательности";
}
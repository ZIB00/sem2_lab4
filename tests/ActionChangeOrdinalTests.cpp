#include <gtest/gtest.h>
#include "../include/LazySequence/ActionChangeOrdinal.hpp"

// Тесты создания и базовых свойств

TEST(OrdinalTest, InitFiniteness) {
    Ordinal ord(4);
    
    EXPECT_EQ(ord.GetSize(), 4) << "ord(4)";
}

TEST(OrdinalTest, InitInfinity) {
    Ordinal ord(0, 1);
    
    EXPECT_EQ(ord.GetInfinity(), 1) << "ord(0, 1)";
}

TEST(OrdinalTest, IsInfiniteTrue) {
    Ordinal ord = Ordinal::Infinite();
    
    EXPECT_TRUE(ord.IsInfinite()) << "ord.IsInfinite() должно быть true";
}

TEST(OrdinalTest, IsInfiniteFalse) {
    Ordinal ord(5);
    
    EXPECT_FALSE(ord.IsInfinite()) << "ord.IsInfinite() должно быть false";
}

// +

TEST(OrdinalAdditionTest, FinitePlusFinite) {
    Ordinal ord1(3);
    Ordinal ord2(2);

    Ordinal result = ord1 + ord2;
    
    EXPECT_EQ(result.GetSize(), 5) << "result.GetSize( ord1(3) + ord2(2) )";
}

TEST(OrdinalAdditionTest, FinitePlusFinite_NoInfinity) {
    Ordinal ord1(3);
    Ordinal ord2(2);

    Ordinal result = ord1 + ord2;
    
    EXPECT_EQ(result.GetInfinity(), 0) << "сравнение бесконечностей ord1(3) и ord2(2)";
}

TEST(OrdinalAdditionTest, FinitePlusInfiniteInfinityCheck) {
    Ordinal ord1(5);
    Ordinal ord2 = Ordinal::Infinite();

    Ordinal result = ord1 + ord2;
    
    EXPECT_EQ(result.GetInfinity(), 1) << "result.GetInfinity( конечное + бесконечное ) == 1";
}

TEST(OrdinalAdditionTest, FinitePlusInfinitefinitenessCheck) {
    Ordinal ord1(5);
    Ordinal ord2 = Ordinal::Infinite();

    Ordinal result = ord1 + ord2;
    
    EXPECT_EQ(result.GetSize(), 0) << "result.GetSize( конечное + бесконечное ) == 0";
}

TEST(OrdinalAdditionTest, InfinitePlusFinitefinitenessCheck) {
    Ordinal ord1 = Ordinal::Infinite();
    Ordinal ord2(5);

    Ordinal result = ord1 + ord2;
    
    EXPECT_EQ(result.GetSize(), 5) << "result.GetSize( бесконечное + 5 ) == 5";
}

TEST(OrdinalAdditionTest, InfinitePlusFiniteInfinityCheck) {
    Ordinal ord1 = Ordinal::Infinite();
    Ordinal ord2(5);

    Ordinal result = ord1 + ord2;
    
    EXPECT_EQ(result.GetInfinity(), 1) << "result.GetInfinity( бесконечное + 5 ) == 1";
}

TEST(OrdinalAdditionTest, InfinitePlusInfinite) {
    Ordinal ord1 = Ordinal::Infinite();
    Ordinal ord2 = Ordinal::Infinite();

    Ordinal result = ord1 + ord2;
    
    EXPECT_EQ(result.GetInfinity(), 2) << "Сложение бесконечностей result.GetInfinity( бесконечное + бесконечное ) == 2";
}

TEST(OrdinalAdditionTest, InfiniteWithFinitenessPlusInfinite) {
    Ordinal ord1(5, 1); 
    Ordinal ord2 = Ordinal::Infinite();

    Ordinal result = ord1 + ord2;
    
    EXPECT_EQ(result.GetInfinity(), 2) << "Прибавление к бесконечности с конечной частью бесконечности даёт 2 бесконечности result.GetInfinity() = 2";
}

TEST(OrdinalAdditionTest, InfiniteWithFinitenessPlusInfinitefinitenessCheck) {
    Ordinal ord1(5, 1);
    Ordinal ord2 = Ordinal::Infinite();

    Ordinal result = ord1 + ord2;
    
    EXPECT_EQ(result.GetSize(), 0) << "Прибавление к бесконечности с конечной частью бесконечности даёт 2 бесконечности result.GetSize() = 0";
}

// -

TEST(OrdinalSubtractionTest, FiniteMinusFinite) {
    Ordinal ord(10);

    Ordinal result = ord - 3;
    
    EXPECT_EQ(result.GetSize(), 7) << "10 - 3 == 7";
}

TEST(OrdinalSubtractionTest, FiniteMinusGreaterThanFinite) {
    Ordinal ord(10);

    Ordinal result = ord - 15;
    
    EXPECT_EQ(result.GetSize(), 0) << "10 - 15 = 0 (ординалы порядковые,)";
}

TEST(OrdinalSubtractionTest, InfiniteMinusFiniteInfinityCheck) {
    Ordinal ord = Ordinal::Infinite();

    Ordinal result = ord - 5;
    
    EXPECT_EQ(result.GetInfinity(), 1) << "Бесконечное минус кконечное, всё ещё такое же бесконечное";
}

TEST(OrdinalSubtractionTest, InfiniteMinusFiniteIsFinitenessZero) {
    Ordinal ord = Ordinal::Infinite();

    Ordinal result = ord - 5;
    
    EXPECT_EQ(result.GetSize(), 0) << "Бесконечное минус коенчное, всё ещё бесконечное, без конечной состовляющей";
}

TEST(OrdinalSubtractionTest, InfiniteWithFinitenessMinusFinite) {
    Ordinal ord(5, 1);

    Ordinal result = ord - 2;
    
    EXPECT_EQ(result.GetSize(), 3) << "бесконечное + конечное(5) - конечное(2) = бесконечное + конечное(3)";
}

TEST(OrdinalSubtractionTest, InfiniteWithFinitenessMinusGreaterThanFinitenessInfinityCheck) {
    Ordinal ord(5, 1);

    Ordinal result = ord - 10;
    
    EXPECT_EQ(result.GetInfinity(), 1) << "бесконечное + конечное(5) - конечное(10) = бесконечное";
}

TEST(OrdinalSubtractionTest, InfiniteWithFinitenessMinusGreaterThanFinitenessFinitenessCheck) {
    Ordinal ord(5, 1);

    Ordinal result = ord - 10;
    
    EXPECT_EQ(result.GetSize(), 0) << "бесконечное + конечное(5) - конечное(10) = бесконечное + 0. проверка на 0";
}

// ==

TEST(OrdinalEqualityTest, EqualsFinite) {
    Ordinal ord1(5);
    Ordinal ord2(5);
    
    EXPECT_TRUE(ord1 == ord2) << "5 == 5";
}

TEST(OrdinalEqualityTest, NotEqualsFinite) {
    Ordinal ord1(5);
    Ordinal ord2(6);
    
    EXPECT_FALSE(ord1 == ord2) << "5 != 6";
}

TEST(OrdinalEqualityTest, EqualsInfinite) {
    Ordinal ord1 = Ordinal::Infinite();
    Ordinal ord2 = Ordinal::Infinite();
    
    EXPECT_TRUE(ord1 == ord2) << "бесконечное == бесконечное";
}

TEST(OrdinalEqualityTest, EqualsInfiniteWithFiniteness) {
    Ordinal ord1(5, 2);
    Ordinal ord2(5, 2);
    
    EXPECT_TRUE(ord1 == ord2) << "бесконечное + 5 == бесконечное + 5";
}

TEST(OrdinalEqualityTest, NotEqualsInfiniteDifferentFiniteness) {
    Ordinal ord1(5, 1);
    Ordinal ord2(6, 1);
    
    EXPECT_FALSE(ord1 == ord2) << "бесконечное + 5 != бесконечное + 6";
}

TEST(OrdinalEqualityTest, NotEqualsInfiniteDifferentInfinity) {
    Ordinal ord1(5, 1);
    Ordinal ord2(5, 2);
    
    EXPECT_FALSE(ord1 == ord2) << "бесконечное + 5 != бесконечное*2 + 5";
}

// >

TEST(OrdinalGreaterThanTest, FiniteGreaterFinite) {
    Ordinal ord1(6);
    Ordinal ord2(5);
    
    EXPECT_TRUE(ord1 > ord2) << "6 > 5";
}

TEST(OrdinalGreaterThanTest, FiniteNotGreaterFinite) {
    Ordinal ord1(5);
    Ordinal ord2(6);
    
    EXPECT_FALSE(ord1 > ord2) << "5 не больше 6";
}

TEST(OrdinalGreaterThanTest, InfiniteGreaterFinite) {
    Ordinal ord1 = Ordinal::Infinite();
    Ordinal ord2(1000);
    
    EXPECT_TRUE(ord1 > ord2) << "бесконечное > конечного(сколько-либо боьшого(взято 1000))";
}

TEST(OrdinalGreaterThanTest, InfiniteWithFinitenessGreaterInfinite) {
    Ordinal ord1(1, 1);
    Ordinal ord2 = Ordinal::Infinite();
    
    EXPECT_TRUE(ord1 > ord2) << "Бесконечное + 1 > бесконечное";
}

TEST(OrdinalGreaterThanTest, MultiInfiniteGreaterSingleInfinite) {
    Ordinal ord1(0, 2);
    Ordinal ord2 = Ordinal::Infinite();
    
    EXPECT_TRUE(ord1 > ord2) << "бесконечное*2 > бесконечное";
}

TEST(OrdinalGreaterThanTest, MultiInfiniteGreaterSingleInfiniteWithFiniteness) {
    Ordinal ord1(0, 2);
    Ordinal ord2(1000, 1);
    
    EXPECT_TRUE(ord1 > ord2) << "бесконечное*2 > бесконечное + конечного(сколько-либо боьшого(взято 1000))";
}

// <

TEST(OrdinalLessThanTest, FiniteLessFinite) {
    Ordinal ord1(5);
    Ordinal ord2(6);
    
    EXPECT_TRUE(ord1 < ord2) << "5 < 6";
}

TEST(OrdinalLessThanTest, FiniteLessInfinite) {
    Ordinal ord1(1000);
    Ordinal ord2 = Ordinal::Infinite();
    
    EXPECT_TRUE(ord1 < ord2) << "конечное(сколько-либо боьшого(взято 1000)) < бесконечного";
}

TEST(OrdinalLessThanTest, InfiniteLessInfiniteWithFiniteness) {
    Ordinal ord1 = Ordinal::Infinite();
    Ordinal ord2(1, 1);
    
    EXPECT_TRUE(ord1 < ord2) << "бесконечное < бесконечное + 1";
}

// >= 

TEST(OrdinalGreaterOrEqualTest, Greater) {
    Ordinal ord1(6);
    Ordinal ord2(5);
    
    EXPECT_TRUE(ord1 >= ord2) << "6 >= 5";
}

TEST(OrdinalGreaterOrEqualTest, Equal) {
    Ordinal ord1(5);
    Ordinal ord2(5);
    
    EXPECT_TRUE(ord1 >= ord2) << "5 >= 5";
}

// <=

TEST(OrdinalLessOrEqualTest, Less) {
    Ordinal ord1(5);
    Ordinal ord2(6);
    
    EXPECT_TRUE(ord1 <= ord2) << "5 <= 6";
}

TEST(OrdinalLessOrEqualTest, Equal) {
    Ordinal ord1(5);
    Ordinal ord2(5);
    
    EXPECT_TRUE(ord1 <= ord2) << "5 <= 5";
}
#pragma once

#include <gtest/gtest.h>
#include <initializer_list>
#include <string>

#include "../include/LazySequence/LazySequence.hpp"
#include "../include/Sequences/sequences/Sequence.hpp"
#include "Sequences/sequences/ArraySequence.hpp"

template <typename T>
void LAZY_EQ(const LazySequence<T>& seq, Sequence<T>& expected, const std::string& context) {
    size_t len = seq.GetLength().IsInfinite() ? seq.GetMaterializedCount() : seq.GetLength().GetSize();

    EXPECT_EQ(expected.GetLength(), len) 
        << "Количество материализованных элементов не совпадает с эталоном. Контекст: " << context;

    for (size_t i = 0; i < len; ++i) {
        EXPECT_EQ(expected.Get(i), seq.Get(i)) 
            << "Расхождение значений на индексе [" << i << "]. Контекст: " << context;
    }
}

template <typename T>
void LAZY_EQ2(const LazySequence<T>& seq, std::initializer_list<T> list, const std::string& context) {
    MutableArraySequence<T> expected = MutableArraySequence<T>(list);

    for (size_t i = 0; i < list.size(); ++i) {
        EXPECT_EQ(expected.Get(i), seq.Get(i)) 
            << "Расхождение значений на индексе [" << i << "]. Контекст: " << context;
    }
}
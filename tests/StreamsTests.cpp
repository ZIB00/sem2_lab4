#include <cstddef>
#include <gtest/gtest.h>
#include <initializer_list>
#include <string>
#include <fstream>
#include <memory>
#include <functional>

#include "TestUtils.hpp"
#include "../include/streams/Streams.hpp"

int StreamStrToIntDeser(const std::string& str) {
    if (str.empty()) return 0;
    return std::stoi(str);
}

std::string StreamIntToStrSer(const int& item) {
    return std::to_string(item);
}

void CreateStreamFile(const std::string& path, const std::string& content) {
    std::ofstream out(path);
    out << content;
    out.close();
}


TEST(SeqReadStreamTest, InitialPositionIsZero) {
    std::shared_ptr<Sequence<int>> seq = std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{10, 20});
    ReadOnlyStream<int> stream(seq);
    
    EXPECT_EQ(stream.GetPosition(), 0) << "Начальная позиция потока из последовательности должна быть 0";
}

TEST(SeqReadStreamTest, ReadReturnsFirstElement) {
    std::shared_ptr<Sequence<int>> seq = std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{10, 20});
    ReadOnlyStream<int> stream(seq);
    
    EXPECT_EQ(stream.Read(), 10) << "Первое чтение должно возвращать первый элемент коллекции";
}

TEST(SeqReadStreamTest, ReadAdvancesPosition) {
    std::shared_ptr<Sequence<int>> seq = std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{10, 20});
    ReadOnlyStream<int> stream(seq);
    stream.Read();
    
    EXPECT_EQ(stream.GetPosition(), 1) << "Чтение должно увеличивать позицию на 1";
}

TEST(SeqReadStreamTest, ReadReturnsSecondElement) {
    std::shared_ptr<Sequence<int>> seq = std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{10, 20});
    ReadOnlyStream<int> stream(seq);
    stream.Read();
    
    EXPECT_EQ(stream.Read(), 20) << "Второе чтение должно возвращать второй элемент коллекции";
}

TEST(SeqReadStreamTest, EOFIsTrueAtEnd) {
    std::shared_ptr<Sequence<int>> seq = std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{10});
    ReadOnlyStream<int> stream(seq);
    stream.Read();
    
    EXPECT_TRUE(stream.IsEndOfStream()) << "IsEndOfStream должен возвращать true, если считаны все элементы";
}

TEST(SeqReadStreamTest, ReadThrowsEndOfStream) {
    std::shared_ptr<Sequence<int>> seq = std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{10});
    ReadOnlyStream<int> stream(seq);
    stream.Read();
    
    EXPECT_THROW(stream.Read(), EndOfStream) << "Попытка чтения после конца последовательности должна вызывать EndOfStream";
}

TEST(SeqReadStreamTest, SeekUpdatesPositionForward) {
    std::shared_ptr<Sequence<int>> seq = std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{10, 20, 30});
    ReadOnlyStream<int> stream(seq);
    stream.Seek(2);
    
    EXPECT_EQ(stream.GetPosition(), 2) << "Seek вперед должен обновлять позицию до указанного индекса";
}

TEST(SeqReadStreamTest, SeekForwardReadsCorrectValue) {
    std::shared_ptr<Sequence<int>> seq = std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{10, 20, 30});
    ReadOnlyStream<int> stream(seq);
    stream.Seek(2);
    
    EXPECT_EQ(stream.Read(), 30) << "Чтение после Seek вперед должно возвращать элемент по новому индексу";
}

TEST(SeqReadStreamTest, SeekClampsToMaxAvailableIndex) {
    std::shared_ptr<Sequence<int>> seq = std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{10, 20});
    ReadOnlyStream<int> stream(seq);
    stream.Seek(100);
    
    EXPECT_EQ(stream.GetPosition(), 1) << "Seek за пределы длины должен устанавливать позицию на последний возможный элемент (GetLength() - 1)";
}

TEST(SeqReadStreamTest, SeekBackwardAllowsRereading) {
    std::shared_ptr<Sequence<int>> seq = std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{10, 20, 30});
    ReadOnlyStream<int> stream(seq);
    stream.Read();
    stream.Read();
    stream.Seek(0);
    
    EXPECT_EQ(stream.Read(), 10) << "Seek назад должен позволять повторное чтение начальных элементов";
}


TEST(StringReadStreamTest, ReadParsesFirstElement) {
    std::shared_ptr<std::string> str = std::make_shared<std::string>("100 200 300");
    ReadOnlyStream<int> stream(str, StreamStrToIntDeser);
    
    EXPECT_EQ(stream.Read(), 100) << "Чтение из строки должно возвращать первое десериализованное значение";
}

TEST(StringReadStreamTest, PositionAdvancesInString) {
    std::shared_ptr<std::string> str = std::make_shared<std::string>("100 200");
    ReadOnlyStream<int> stream(str, StreamStrToIntDeser);
    stream.Read();
    
    EXPECT_EQ(stream.GetPosition(), 1) << "Чтение из строки должно корректно увеличивать позицию элементов";
}

TEST(StringReadStreamTest, ReadParsesSecondElement) {
    std::shared_ptr<std::string> str = std::make_shared<std::string>("100 200");
    ReadOnlyStream<int> stream(str, StreamStrToIntDeser);
    stream.Read();
    
    EXPECT_EQ(stream.Read(), 200) << "Второе чтение из строки должно корректно десериализовать второе число";
}

TEST(StringReadStreamTest, SeekForwardReadsCorrectToken) {
    std::shared_ptr<std::string> str = std::make_shared<std::string>("100 200 300");
    ReadOnlyStream<int> stream(str, StreamStrToIntDeser);
    stream.Seek(2);
    
    EXPECT_EQ(stream.Read(), 300) << "Seek вперед в строке должен пропускать токены";
}

TEST(StringReadStreamTest, SeekBackwardRewindsStringStream) {
    std::shared_ptr<std::string> str = std::make_shared<std::string>("100 200");
    ReadOnlyStream<int> stream(str, StreamStrToIntDeser);
    stream.Read();
    stream.Read();
    stream.Seek(0);
    
    EXPECT_EQ(stream.Read(), 100) << "Seek назад в строке должен сбрасывать stringstream и считывать с начала";
}


TEST(LazySeqReadStreamTest, FiniteLazyReachesEOF) {
    std::shared_ptr<LazySequence<int>> seq = std::make_shared<LazySequence<int>>(std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{10, 20}));
    ReadOnlyStream<int> stream(seq);
    stream.Read();
    stream.Read();
    
    EXPECT_TRUE(stream.IsEndOfStream()) << "Конечный ленивый поток должен достигать EOF";
}

TEST(LazySeqReadStreamTest, InfiniteLazyNeverReachesEOF) {
    std::shared_ptr<LazySequence<int>> seq = std::make_shared<LazySequence<int>>([](auto list) { return 1; }, std::make_shared<MutableListSequence<int>>());
    ReadOnlyStream<int> stream(seq);
    stream.Seek(100);
    
    EXPECT_FALSE(stream.IsEndOfStream()) << "Бесконечный ленивый поток не должен возвращать true для EOF";
}

TEST(LazySeqReadStreamTest, InfiniteLazyGeneratesValues) {
    std::shared_ptr<LazySequence<int>> seq = std::make_shared<LazySequence<int>>([](auto list) { return list->GetLength() + 1; }, std::make_shared<MutableListSequence<int>>());
    ReadOnlyStream<int> stream(seq);
    stream.Read(); 
    stream.Read(); 
    
    EXPECT_EQ(stream.Read(), 3) << "Поток над бесконечной ленивой последовательностью должен вычислять значения на лету";
}

TEST(LazySeqReadStreamTest, FiniteLazySeekClampsToBound) {
    std::shared_ptr<LazySequence<int>> seq = std::make_shared<LazySequence<int>>(std::make_shared<MutableListSequence<int>>(std::initializer_list<int>{10, 20}));
    ReadOnlyStream<int> stream(seq);
    stream.Seek(50);
    
    EXPECT_EQ(stream.GetPosition(), 1) << "Seek за пределы конечной ленивой последовательности должен обрезать индекс до GetSize() - 1";
}


TEST(FileReadStreamTest, ReadFromDiskWorks) {
    CreateStreamFile("test_file_read_data.txt", "777 888");
    ReadOnlyStream<int> stream("test_file_read_data.txt", StreamStrToIntDeser);
    
    EXPECT_EQ(stream.Read(), 777) << "Должно корректно считываться и десериализовываться первое значение из файла";
}

TEST(FileReadStreamTest, EOFTriggeredCorrectly) {
    CreateStreamFile("test_file_read_eof.txt", "777");
    ReadOnlyStream<int> stream("test_file_read_eof.txt", StreamStrToIntDeser);
    stream.Read();
    
    EXPECT_TRUE(stream.IsEndOfStream()) << "После прочтения последнего элемента файла IsEndOfStream должен быть true";
}

TEST(FileReadStreamTest, ReadThrowsAtEOF) {
    CreateStreamFile("test_file_read_throw.txt", "777");
    ReadOnlyStream<int> stream("test_file_read_throw.txt", StreamStrToIntDeser);
    stream.Read();
    
    EXPECT_THROW(stream.Read(), EndOfStream) << "Попытка прочитать из файла после EOF должна вызывать EndOfStream";
}

TEST(FileReadStreamTest, SeekForwardSkipsTokens) {
    CreateStreamFile("test_file_read_seek.txt", "11 22 33");
    ReadOnlyStream<int> stream("test_file_read_seek.txt", StreamStrToIntDeser);
    stream.Seek(2);
    
    EXPECT_EQ(stream.Read(), 33) << "Seek вперед в файле должен корректно пропускать токены";
}

TEST(FileReadStreamTest, SeekBackwardRewindsFileStream) {
    CreateStreamFile("test_file_read_rewind.txt", "11 22 33");
    ReadOnlyStream<int> stream("test_file_read_rewind.txt", StreamStrToIntDeser);
    stream.Read();
    stream.Read();
    stream.Seek(0);
    
    EXPECT_EQ(stream.Read(), 11) << "Seek назад в файле должен переоткрывать позицию на начало и читать с нуля";
}


TEST(SeqWriteStreamTest, WriteAppendsElement) {
    std::shared_ptr<Sequence<int>> seq = std::make_shared<MutableListSequence<int>>();
    WriteOnlyStream<int> stream(seq);
    stream.Write(52);
    
    EXPECT_EQ(seq->GetLast(), 52) << "Write должен добавлять элемент в конец оригинальной последовательности";
}

TEST(SeqWriteStreamTest, WriteAdvancesPosition) {
    std::shared_ptr<Sequence<int>> seq = std::make_shared<MutableListSequence<int>>();
    WriteOnlyStream<int> stream(seq);
    stream.Write(52);
    
    EXPECT_EQ(stream.GetPosition(), 1) << "После выполнения записи позиция потока записи должна увеличиться на 1";
}

TEST(SeqWriteStreamTest, MultipleWritesUpdateLength) {
    std::shared_ptr<Sequence<int>> seq = std::make_shared<MutableListSequence<int>>();
    WriteOnlyStream<int> stream(seq);
    stream.Write(10);
    stream.Write(20);
    stream.Write(30);
    
    EXPECT_EQ(seq->GetLength(), 3) << "Несколько записей должны корректно изменять длину базовой коллекции";
}


TEST(FileWriteStreamTest, WriteAdvancesPositionInFile) {
    WriteOnlyStream<int> stream("test_file_write_data.txt", StreamIntToStrSer);
    
    stream.Write(52);
    
    EXPECT_EQ(stream.GetPosition(), 1) << "Успешная запись в файл должна увеличивать счетчик позиции";
}

TEST(FileWriteStreamTest, WritesCorrectDataToDisk) {
    WriteOnlyStream<int> stream("test_file_write_verify.txt", StreamIntToStrSer);
    stream.Write(123);
    stream.Write(456);

    std::ifstream in("test_file_write_verify.txt");
    std::string result;
    std::getline(in, result);
    in.close();
    
    EXPECT_EQ(result, "123 456 ") << "Данные должны физически сохраняться в файл через разделитель";
}

TEST(HugeStreamTest, WriteOneMillionElements) {
    WriteOnlyStream<int> stream("huge_test_file.txt", StreamIntToStrSer);
    for (int i = 0; i < 1000000; ++i) {
        stream.Write(i);
    }
    
    std::ifstream in("huge_test_file.txt", std::ios::ate | std::ios::binary);
    bool isLarge = in.tellg() > 1000000;
    in.close();
    std::remove("huge_test_file.txt");
    
    EXPECT_TRUE(isLarge) << "Файл на миллион элементов должен быть успешно создан";
}

TEST(HugeStreamTest, GetPositionFromHugeFileWork) {
    std::ofstream out("huge_test_file.txt");
    for (int i = 0; i < 500000; ++i) {
        out << i << " ";
    }
    out.close();
    ReadOnlyStream<int> stream("huge_test_file.txt", StreamStrToIntDeser);
    for (int i = 0; i < 500000; ++i) {
        stream.Read();
    }

    size_t pos = stream.GetPosition();

    std::remove("huge_test_file.txt");

    EXPECT_EQ(pos, 500000) << "GetPosition в конце огромного файла";
}

TEST(HugeStreamTest, SeekInHugeFileWork) {
    std::ofstream out("huge_test_file.txt");
    for (int i = 0; i < 1000000; ++i) {
        out << i << " ";
    }
    out.close();
    ReadOnlyStream<int> stream("huge_test_file.txt", StreamStrToIntDeser);

    stream.Seek(999999);
    int val = stream.Read();

    std::remove("huge_test_file.txt");

    EXPECT_EQ(val, 999999) << "Seek может считать верное значение с конца огромного файла";
}

TEST(HugeStreamTest, SeekBackwardWorkInHugeFile) {
    std::ofstream out("huge_test_file.txt");
    for (int i = 0; i < 500000; ++i) {
        out << i << " ";
    }
    out.close();

    ReadOnlyStream<int> stream("huge_test_file.txt", StreamStrToIntDeser);

    stream.Seek(499999);
    stream.Seek(0);
    int val = stream.Read();

    std::remove("huge_test_file.txt");

    EXPECT_EQ(val, 0) << "Seek может считать верное значение с начала огромного файла, побывав в конце";
}

TEST(InfiniteStreamTest, IsEndOfStreamAlwaysFalseAfterDeepSeek) {
    std::shared_ptr<LazySequence<int>> seq = std::make_shared<LazySequence<int>>([](auto list) { return list->GetLength(); }, std::make_shared<MutableListSequence<int>>());
    ReadOnlyStream<int> stream(seq);

    stream.Seek(10000);
    
    EXPECT_FALSE(stream.IsEndOfStream()) << "Seek не может делать бесконечный поток завершенным";
}

TEST(InfiniteStreamTest, ReadThrowsNoExceptionsOnExtensiveUsage) {
    std::shared_ptr<LazySequence<int>> seq = std::make_shared<LazySequence<int>>([](auto list) { return 1; }, std::make_shared<MutableListSequence<int>>());
    ReadOnlyStream<int> stream(seq);
    bool success = true;
    
    try {
        for (int i = 0; i < 15000; ++i) {
            stream.Read();
        }
    } catch(...) {
        success = false;
    }
    
    EXPECT_TRUE(success) << "Чтение бесконечного потока не должно приводить к выбросу исключений";
}
#include "file_descriptor.h"
#include "files_comparator.h"
#include <boost/filesystem.hpp>
#include <gtest/gtest.h>
#include <iostream>

namespace fs = boost::filesystem;

namespace OTUS 
{
template<>
int32_t calcChecksum(size_t block_sz, char* buf)
{
    int res = 0;
    for (size_t i = 0; i < block_sz; i++)
    {
        res += buf[i];
    }
    std::cout << "calc from specialization [int32_t] " << res << std::endl;
    return res;
}
}

TEST(file_descriptor, equal_op)
{
    fs::path test_dir("sandbox");
    size_t blocksize = 4;
    std::string file1{"sandbox/file1.txt"};
    std::string file2{"sandbox/file2.txt"};
    std::string file3{"sandbox/file3.txt"};

    OTUS::FileDescriptor<int32_t> fd1(blocksize, file1, 16);
    OTUS::FileDescriptor<int32_t> fd2(blocksize, file2, 16);
    OTUS::FileDescriptor<int32_t> fd2a(blocksize, file2, 16);
    OTUS::FileDescriptor<int32_t> fd3(blocksize, file3, 16);
    bool eq12 = (fd1 == fd2);
    ASSERT_TRUE(eq12);
    bool eq12a = (fd1 == fd2a);
    ASSERT_TRUE(eq12a);
    bool eq13 = (fd1 == fd3);
    ASSERT_FALSE(eq13);
}

TEST(file_descriptor, diff_sizes)
{
    size_t blocksize = 4;
    std::string file1{"sandbox/file1.txt"};
    std::string file4{"sandbox/file4.txt"};
    OTUS::FileDescriptor<int32_t> fd1(blocksize, file1, 16);
    OTUS::FileDescriptor<int32_t> fd4(blocksize, file4, 4);
    bool eq14 = (fd1 == fd4);
    ASSERT_FALSE(fd1 == fd4);
    ASSERT_FALSE(fd4 == fd1);
}

static void create_test_filesystem()
{
    fs::create_directories("sandbox");
    std::ofstream f1("sandbox/file1.txt", std::ios::binary);
    std::ofstream f2("sandbox/file2.txt", std::ios::binary);
    std::ofstream f3("sandbox/file3.txt", std::ios::binary);
    std::ofstream f4("sandbox/file4.txt", std::ios::binary);
    int32_t n = 1;
    f1.write(reinterpret_cast<char*>(&n), 4);
    f2.write(reinterpret_cast<char*>(&n), 4);
    f3.write(reinterpret_cast<char*>(&n), 4);
    f4.write(reinterpret_cast<char*>(&n), 4);
    n = 2;
    f1.write(reinterpret_cast<char*>(&n), 4);
    f2.write(reinterpret_cast<char*>(&n), 4);
    f3.write(reinterpret_cast<char*>(&n), 4);
    n = 3;
    f1.write(reinterpret_cast<char*>(&n), 4);
    f2.write(reinterpret_cast<char*>(&n), 4);
    f3.write(reinterpret_cast<char*>(&n), 4);
    n = 4;
    f1.write(reinterpret_cast<char*>(&n), 4);
    f2.write(reinterpret_cast<char*>(&n), 4);
    n = 5;
    f3.write(reinterpret_cast<char*>(&n), 4);
}

static void delete_test_filesystem()
{
    fs::remove_all("sandbox");
}

int main(int argc, char **argv) {
    create_test_filesystem();
    ::testing::InitGoogleTest(&argc, argv);
    auto res = RUN_ALL_TESTS();
    // delete_test_filesystem();
    return res;
}
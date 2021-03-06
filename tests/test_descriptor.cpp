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
    // std::cout << "calc from specialization [int32_t] " << res << std::endl;
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

TEST(file_descriptor, permissions)
{
    size_t blocksize = 4;
    std::string file1{"sandbox/file1.txt"};
    std::string file_perm{"sandbox/file_perm.txt"};
    OTUS::FileDescriptor<int32_t> fd1(blocksize, file1, 16);
    OTUS::FileDescriptor<int32_t> fdp(blocksize, file_perm, 16);
    bool eq;
    ASSERT_THROW(eq = (fd1 == fdp), std::runtime_error);
    // ASSERT_FALSE(fd1 == fd4);
    // ASSERT_FALSE(fd4 == fd1);
}

TEST(file_descriptor, diff_checksums)
{
    size_t blocksize = 16;
    std::string file1{"sandbox/file1.txt"};
    OTUS::FileDescriptor<int32_t> fd1(blocksize, file1, 16);
    OTUS::FileDescriptor<int32_t> fd2(blocksize, file1, 16);
    ASSERT_EQ(0, fd1.blocks().size());
    // Force descriptors to read files content
    (void)(fd1 == fd2);
    ASSERT_EQ(1, fd1.blocks().size());
    ASSERT_EQ(10, fd1.blocks().front());
}

static void create_test_filesystem()
{
    fs::create_directories("sandbox");
    std::ofstream f1("sandbox/file1.txt", std::ios::binary);
    std::ofstream f2("sandbox/file2.txt", std::ios::binary);
    std::ofstream f3("sandbox/file3.txt", std::ios::binary);
    std::ofstream f4("sandbox/file4.txt", std::ios::binary);
    std::ofstream f_perm("sandbox/file_perm.txt", std::ios::binary);
    int32_t n = 1;
    f1.write(reinterpret_cast<char*>(&n), 4);
    f2.write(reinterpret_cast<char*>(&n), 4);
    f3.write(reinterpret_cast<char*>(&n), 4);
    f4.write(reinterpret_cast<char*>(&n), 4);
    f_perm.write(reinterpret_cast<char*>(&n), 4);
    n = 2;
    f1.write(reinterpret_cast<char*>(&n), 4);
    f2.write(reinterpret_cast<char*>(&n), 4);
    f3.write(reinterpret_cast<char*>(&n), 4);
    f_perm.write(reinterpret_cast<char*>(&n), 4);
    n = 3;
    f1.write(reinterpret_cast<char*>(&n), 4);
    f2.write(reinterpret_cast<char*>(&n), 4);
    f3.write(reinterpret_cast<char*>(&n), 4);
    f_perm.write(reinterpret_cast<char*>(&n), 4);
    n = 4;
    f1.write(reinterpret_cast<char*>(&n), 4);
    f2.write(reinterpret_cast<char*>(&n), 4);
    f_perm.write(reinterpret_cast<char*>(&n), 4);
    n = 5;
    f3.write(reinterpret_cast<char*>(&n), 4);

    f_perm.close();
    fs::permissions("sandbox/file_perm.txt", fs::remove_perms | fs::all_all);
}

static void delete_test_filesystem()
{
    fs::remove_all("sandbox");
}

int main(int argc, char **argv) {
    // Just in case, better to clean the test filesystem:
    delete_test_filesystem();
    create_test_filesystem();
    ::testing::InitGoogleTest(&argc, argv);
    auto res = RUN_ALL_TESTS();
    delete_test_filesystem();
    return res;
}
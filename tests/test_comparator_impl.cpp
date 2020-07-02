#include "file_descriptor.h"
#include "files_comparator_impl.h"
#include <boost/filesystem.hpp>
#include <gtest/gtest.h>
#include <iostream>
#include <ostream>
#include <sstream>

namespace fs = boost::filesystem;

std::ostream& operator<<(std::ostream& out, OTUS::IFilesComparatorImpl::Duplicates_t dup)
{
    for(auto& r: dup)
    {
        out << r.first << ":";
        for (auto& d: r.second)
        {
            out << " " << d;
        }
        out << std::endl;
    }
    return out;
}

TEST(files_comparator_impl, basic)
{
    std::stringstream ss;
    size_t blocksize = 4;
    auto fc_crc = OTUS::FilesComparatorImpl<unsigned int>(blocksize);
    fc_crc.add_path("sandbox/file1.txt", 16);
    fc_crc.add_path("sandbox/a/file2.txt", 16);
    fc_crc.add_path("sandbox/file3.txt", 16);
    fc_crc.add_path("sandbox/file4.txt", 4);
    ss << fc_crc.duplicates();
    std::cout << "Report:\n" << ss.str() << std::endl;
    std::string exp("sandbox/file1.txt: sandbox/a/file2.txt\n");
    ASSERT_STREQ(exp.c_str(), ss.str().c_str());
}

static void create_test_filesystem()
{
    fs::create_directories("sandbox/a");
    std::ofstream f1("sandbox/file1.txt", std::ios::binary);
    std::ofstream f2("sandbox/a/file2.txt", std::ios::binary);
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
    delete_test_filesystem();
    create_test_filesystem();
    ::testing::InitGoogleTest(&argc, argv);
    auto res = RUN_ALL_TESTS();
    delete_test_filesystem();
    return res;
}
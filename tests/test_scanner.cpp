#include <gtest/gtest.h>
#include <sstream>
#include <vector>
#include <memory>
#include <iostream>

#include "iobserver.h"
#include "scanner.h"

namespace fs = boost::filesystem;

namespace 
{

struct TestExecutor: public OTUS::IObserver
{
    void update(OTUS::Event const& ev)
    {
        m_events.push_back(ev);
    }
    std::vector<OTUS::Event> m_events;
};

void create_test_filesystem()
{
    fs::create_directories("sandbox/a/b/c");
    fs::create_directories("sandbox/a/b1");
    std::ofstream("sandbox/file1.txt");
    std::ofstream("sandbox/file2.txt");
    std::ofstream("sandbox/a/b/c/file3.txt");
    std::ofstream("sandbox/a/b1/file4.txt");
}

void delete_test_filesystem()
{
    fs::remove_all("sandbox");
}

TEST(scanner, regular)
{
    fs::path test_dir("sandbox");
    std::vector<fs::path> arg{test_dir};
    auto sc = OTUS::Scanner(arg);
    auto ex1_ptr = std::make_shared<TestExecutor>();
    sc.subscribe(ex1_ptr);
    sc.run();
    ASSERT_EQ(5, ex1_ptr->m_events.size()); // 3 files and END_STREAM
}

TEST(scanner, depth)
{
    fs::path test_dir("sandbox");
    std::vector<fs::path> arg{test_dir};
    auto sc = OTUS::Scanner(arg);
    sc.set_depth(0);
    auto ex1_ptr = std::make_shared<TestExecutor>();
    sc.subscribe(ex1_ptr);
    sc.run();
    ASSERT_EQ(3, ex1_ptr->m_events.size()); // 2 files and END_STREAM
}

TEST(scanner, exclude_dir)
{
    fs::path test_dir("sandbox");
    std::vector<fs::path> arg{test_dir};
    auto sc = OTUS::Scanner(arg);
    std::vector<fs::path> exclude_paths{"sandbox/a/b"};
    sc.exclude_paths(exclude_paths);
    auto ex1_ptr = std::make_shared<TestExecutor>();
    sc.subscribe(ex1_ptr);
    sc.run();
    ASSERT_EQ(4, ex1_ptr->m_events.size()); // 3 files and END_STREAM
}

TEST(scanner, mask)
{
    fs::path test_dir("sandbox");
    std::vector<fs::path> arg{test_dir};
    auto sc = OTUS::Scanner(arg);
    std::vector<std::string> masks{".*/file[24].*"};
    sc.masks(masks);
    auto ex1_ptr = std::make_shared<TestExecutor>();
    sc.subscribe(ex1_ptr);
    sc.run();
    ASSERT_EQ(3, ex1_ptr->m_events.size()); // 2 files and END_STREAM
}

} // namespace

int main(int argc, char **argv) {
    create_test_filesystem();
    ::testing::InitGoogleTest(&argc, argv);
    auto res = RUN_ALL_TESTS();
    delete_test_filesystem();
    return res;
}
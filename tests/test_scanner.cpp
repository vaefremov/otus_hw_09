#include <gtest/gtest.h>
#include <sstream>
#include <vector>
#include <memory>
#include <iostream>

#include "iobserver.h"
#include "scanner.h"

namespace fs = boost::filesystem;

struct TestExecutor: public OTUS::IObserver
{
    void update(OTUS::Event const& ev)
    {
        m_events.push_back(ev);
    }
    std::vector<OTUS::Event> m_events;
};

static void create_test_filesystem()
{
    fs::create_directories("sandbox/a/b/c");
    fs::create_directories("sandbox/a/b1");
    std::ofstream("sandbox/file1.txt");
    std::ofstream("sandbox/file2.txt");
    std::ofstream("sandbox/a/b/c/file3.txt");
    std::ofstream("sandbox/a/b1/file4.txt");
}

static void delete_test_filesystem()
{
    fs::remove_all("sandbox");
}

TEST(scanner, regular)
{
    create_test_filesystem();
    fs::path test_dir("sandbox");
    std::vector<fs::path> arg{test_dir};
    auto sc = OTUS::Scanner(arg);
    auto ex1_ptr = std::make_shared<TestExecutor>();
    sc.subscribe(ex1_ptr);
    sc.run();
    ASSERT_EQ(5, ex1_ptr->m_events.size()); // 3 files and END_STREAM

    for(auto& e: ex1_ptr->m_events)
    {
        std::cout << static_cast<int>(e.m_kind) << " " << e.m_filesize << " " << e.m_path << std::endl;
    }
}

TEST(scanner, depth)
{
    create_test_filesystem();
    fs::path test_dir("sandbox");
    std::vector<fs::path> arg{test_dir};
    auto sc = OTUS::Scanner(arg);
    sc.set_depth(0);
    auto ex1_ptr = std::make_shared<TestExecutor>();
    sc.subscribe(ex1_ptr);
    sc.run();
    ASSERT_EQ(3, ex1_ptr->m_events.size()); // 2 files and END_STREAM

    for(auto& e: ex1_ptr->m_events)
    {
        std::cout << static_cast<int>(e.m_kind) << " " << e.m_filesize << " " << e.m_path << std::endl;
    }
}

TEST(scanner, exclude_dir)
{
    create_test_filesystem();
    fs::path test_dir("sandbox");
    std::vector<fs::path> arg{test_dir};
    auto sc = OTUS::Scanner(arg);
    std::vector<fs::path> exclude_paths{"sandbox/a/b"};
    sc.exclude_paths(exclude_paths);
    auto ex1_ptr = std::make_shared<TestExecutor>();
    sc.subscribe(ex1_ptr);
    sc.run();
    ASSERT_EQ(4, ex1_ptr->m_events.size()); // 3 files and END_STREAM

    for(auto& e: ex1_ptr->m_events)
    {
        std::cout << static_cast<int>(e.m_kind) << " " << e.m_filesize << " " << e.m_path << std::endl;
    }
}

TEST(scanner, mask)
{
    create_test_filesystem();
    fs::path test_dir("sandbox");
    std::vector<fs::path> arg{test_dir};
    auto sc = OTUS::Scanner(arg);
    std::vector<std::string> masks{".*/file[24].*"};
    sc.masks(masks);
    auto ex1_ptr = std::make_shared<TestExecutor>();
    sc.subscribe(ex1_ptr);
    sc.run();

    for(auto& e: ex1_ptr->m_events)
    {
        std::cout << static_cast<int>(e.m_kind) << " " << e.m_filesize << " " << e.m_path << std::endl;
    }

    ASSERT_EQ(3, ex1_ptr->m_events.size()); // 3 files and END_STREAM
}


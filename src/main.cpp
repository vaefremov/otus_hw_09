#include <iostream>
#include <ostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <algorithm>
#include <boost/program_options.hpp>

#include "files_comparator.h"
#include "scanner.h"

namespace po = boost::program_options;
using str_vector=std::vector<std::string>;

size_t const DEFAULT_BLOCK_SZ = 1024UL; // Default block size

std::ostream& operator<<(std::ostream& out, std::vector<std::string> const vect)
{
    for(auto e: vect)
    {
        out << " " << e;
    }
    return out;
}

void output_report(std::ostream& out, OTUS::FilesComparator::Report_t const& report, bool isVerbose=false)
{
    if(isVerbose)
    {
        out << "=========================================" << std::endl;
        out << "== Report: total of " << report.size() << " groups of identical files" << std::endl;
        out << "=========================================" << std::endl;
    }
    for(auto& files_group: report)
    {
        for (auto& f: files_group)
        {
            out << f << std::endl;
        }
        out << std::endl;
    }
}

std::vector<OTUS::Scanner::fspath> convert_paths(std::vector<std::string> arg)
{
    std::vector<OTUS::Scanner::fspath> res;
    std::transform(arg.begin(), arg.end(), std::back_inserter(res), [](auto s){return boost::filesystem::path(s);});
    // @TODO: Check if paths are valid!
    return res;
}

int main(int argc, const char** argv) {
    po::options_description desc("Finding the identical files");
    desc.add_options()
        ("help,h", "this help")
        ("path,I", po::value<std::vector<std::string>>(), "search paths")
        ("exclude,x", po::value<std::vector<std::string>>(), "exclude directories from search, optional")
        ("depth,d", po::value<size_t>()->default_value(OTUS::DEF_MAX_DEPTH), "depth of recursive search")
        ("verbose,v", po::bool_switch(), "verbose output")
        ("block_sz,b", po::value<size_t>()->default_value(DEFAULT_BLOCK_SZ), "block size")
        ("mask,m", po::value<std::vector<std::string>>(), "masks (regexp), optional")
        ("hash,H", po::value<std::string>()->default_value("crc32"), "hash method, options are: crc32 (default), md5")
    ;

    po::variables_map vm;        
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);    

    if (vm.count("help")) 
    {
        std::cout << desc << "\n";
        return EXIT_SUCCESS;
    }
    bool isVerbose = vm["verbose"].as<bool>();
    if(isVerbose)
        std::cout << "Verbose: " << isVerbose << std::endl;

    size_t max_depth = vm["depth"].as<size_t>();
    if(isVerbose)
        std::cout << "Max depth: " << vm["depth"].as<size_t>() << std::endl;

    size_t block_sz = vm["block_sz"].as<size_t>();
    if(isVerbose)
        std::cout << "Block sz: " << block_sz << std::endl;

    str_vector paths{"./"};
    if(vm.count("path"))
        paths = vm["path"].as<std::vector<std::string>>();
    if(isVerbose)
        std::cout << "Paths: " << paths << std::endl;

    if (vm.count("exclude") && isVerbose)
        std::cout << "Exclude: " << vm["exclude"].as<std::vector<std::string>>() << std::endl;

    if (vm.count("mask") && isVerbose)   
        std::cout << "Masks: " << vm["mask"].as<std::vector<std::string>>() << std::endl;

    if(isVerbose)
        std::cout << "Hash: " << vm["hash"].as<std::string>() << std::endl;

    try
    {
        OTUS::HashKind hash_kind = OTUS::hash_name_from_string(vm["hash"].as<std::string>());

        OTUS::Scanner scanner(convert_paths(paths));
        scanner.set_verbose(vm["verbose"].as<bool>());
        if(vm.count("exclude"))
            scanner.exclude_paths(convert_paths(vm["exclude"].as<std::vector<std::string>>()));
        if(vm.count("mask"))
            scanner.masks(vm["mask"].as<std::vector<std::string>>());
        scanner.set_depth(max_depth);

        auto comparator = OTUS::FilesComparator::create_subscribed(scanner, block_sz, hash_kind, isVerbose);
        
        scanner.run();

        OTUS::FilesComparator::Report_t report = comparator->report();
        output_report(std::cout, report, isVerbose);
    }
    catch(const std::exception& e)
    {
        std::cout << "Error: " << e.what() << std::endl;
        std::cout << desc << "\n";
        return EXIT_SUCCESS;
    }
    

    return EXIT_SUCCESS;
}
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
size_t const DEFAULT_BLOCK_SZ = 5; // Default block size

std::ostream& operator<<(std::ostream& out, std::vector<std::string> const vect)
{
    for(auto e: vect)
    {
        out << " " << e;
    }
    return out;
}

void output_report(std::ostream& out, OTUS::FilesComparator::Report_t const& report)
{
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
    // if(res.empty()) // insert the default path: cwd
    // {
    //     res.push_back(boost::filesystem::path("."));
    // }
    return res;
}

int main(int argc, const char** argv) {
    po::options_description desc("Finding same files:");
    desc.add_options()
        ("help,h", "this help")
        ("path,I", po::value<std::vector<std::string>>(), "search paths")
        ("exclude,x", po::value<std::vector<std::string>>(), "exclude from search")
        ("depth,d", po::value<size_t>()->default_value(OTUS::MAX_DEPTH), "depth of recursive search")
        ("verbose,v", po::bool_switch(), "verbose output")
        ("block_sz,b", po::value<size_t>()->default_value(DEFAULT_BLOCK_SZ), "block size")
        ("mask,m", po::value<std::vector<std::string>>(), "masks, optional")
        ("hash,H", po::value<std::string>()->default_value("crc32"), "hash method")
    ;

    po::variables_map vm;        
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);    

    if (vm.count("help")) 
    {
        std::cout << desc << "\n";
        return EXIT_SUCCESS;
    }

    std::cout << "Max depth: " << vm["depth"].as<size_t>() << std::endl;
    std::cout << "Verbose: " << vm["verbose"].as<bool>() << std::endl;
    size_t block_sz = vm["block_sz"].as<size_t>();
    std::cout << "Block sz: " << block_sz << std::endl;

    std::cout << "Paths: " << vm["path"].as<std::vector<std::string>>() << std::endl;

    if (vm.count("exclude"))
        std::cout << "Exclude: " << vm["exclude"].as<std::vector<std::string>>() << std::endl;

    if (vm.count("mask"))   
        std::cout << "Masks: " << vm["mask"].as<std::vector<std::string>>() << std::endl;

    std::cout << "Hash: " << vm["hash"].as<std::string>() << std::endl;
    OTUS::HashKind hash_kind = OTUS::hash_name_from_string(vm["hash"].as<std::string>());

    OTUS::Scanner scanner(convert_paths(vm["path"].as<std::vector<std::string>>()));
    if(vm.count("exclude"))
        scanner.exclude_paths(convert_paths(vm["exclude"].as<std::vector<std::string>>()));
    if(vm.count("mask"))
        scanner.masks(vm["mask"].as<std::vector<std::string>>());
    scanner.set_verbose(vm["verbose"].as<bool>());

    auto comparator = OTUS::FilesComparator::create_subscribed(scanner, block_sz, hash_kind, vm["verbose"].as<bool>());
    scanner.run();

    OTUS::FilesComparator::Report_t report = comparator->report();
    output_report(std::cout, report);

    return EXIT_SUCCESS;
}
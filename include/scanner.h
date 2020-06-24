#pragma once

#include "iobserver.h"
#include <string>
#include <vector>
#include <boost/filesystem.hpp>

namespace OTUS
{

size_t const MAX_DEPTH = 1000; // Max depth of recursion

class Scanner final: public BaseObservable
{
    public:
    using fspath = boost::filesystem::path;
    
    Scanner() = delete;
    Scanner(std::vector<fspath> const& start_paths): m_start_paths(start_paths){}
    Scanner& set_depth(size_t max_depth);
    Scanner& exclude_paths(std::vector<fspath> const& exclude);
    Scanner& masks(std::vector<std::string> const& masks);
    Scanner& set_verbose(bool verbose=true);
    void run();

    private:
    bool isFileOK(boost::filesystem::recursive_directory_iterator& it);
    bool isDirExcluded(boost::filesystem::recursive_directory_iterator& it);

    std::vector<fspath> m_start_paths;
    size_t m_depth = MAX_DEPTH;
    std::vector<fspath> m_exclude_paths;
    std::vector<std::string> m_masks;
    bool m_verbose = false;
};

} // namespace OTUS
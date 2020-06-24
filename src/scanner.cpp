#include "scanner.h"
#include <regex>

namespace fs = boost::filesystem;

bool OTUS::Scanner::isFileOK(fs::recursive_directory_iterator& it)
{
    if(m_masks.empty())
        return true;
    return std::any_of(m_masks.begin(), m_masks.end(), [&it](auto m){
        return std::regex_match(it->path().c_str(), std::regex(m));
    });
    // return std::regex_match(it->path().c_str(), std::regex(m_masks[0]));
    // return true;
}

bool OTUS::Scanner::isDirExcluded(fs::recursive_directory_iterator& it)
{
    if(m_exclude_paths.empty())
        return false;
    return std::any_of(m_exclude_paths.begin(), m_exclude_paths.end(), [&it](auto p){return it->path() == p;});
}

void OTUS::Scanner::run()
{
    for (auto root_dir: m_start_paths)
    {
        fs::recursive_directory_iterator files_it(root_dir), end_it;
        while (files_it != end_it)
        {
            if(files_it->status().type() == fs::directory_file && isDirExcluded(files_it))
            {
                files_it.no_push();
                files_it++;
                continue;
            }
            if(files_it->status().type() == fs::regular_file && isFileOK(files_it))
            {
                notify(Event{EventKind::REGULAR_FILE, fs::file_size(files_it->path()), files_it->path()});
            }
            
            if(files_it.depth() >= m_depth)
                files_it.no_push();
            files_it++;
        }

    }
    notify(Event{EventKind::END_STREAM, 0, ""});
}

OTUS::Scanner& OTUS::Scanner::set_depth(size_t max_depth)
{
    m_depth = max_depth;
    return *this;
}

OTUS::Scanner& OTUS::Scanner::set_verbose(bool verbose)
{
    m_verbose = verbose;
    return *this;
}

OTUS::Scanner& OTUS::Scanner::masks(std::vector<std::string> const& masks)
{
    m_masks = masks;
    return *this;
}

OTUS::Scanner& OTUS::Scanner::exclude_paths(std::vector<fs::path> const& exclude)
{
    m_exclude_paths = exclude;
    return *this;
}

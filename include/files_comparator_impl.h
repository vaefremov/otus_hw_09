#pragma once
#include <iostream>
#include "file_descriptor.h"
#include "files_comparator.h"

namespace OTUS
{

template<typename T>
class FilesComparatorImpl: public IFilesComparatorImpl
{
    public:
    
    explicit FilesComparatorImpl(size_t blocksize): m_blocksize(blocksize) {}

    void add(std::string path, size_t fs) override
    {
        auto new_fd_ptr = std::make_unique<FileDescriptor<T>>(m_blocksize, path, fs);
        auto& candidates = m_fs2fds[fs];
        std::string filename_found;
        if(std::any_of(candidates.begin(), candidates.end(), 
            [&new_fd_ptr, &filename_found](auto& fd_ptr)
                {   
                    if(*fd_ptr == *new_fd_ptr)
                    {
                        filename_found = fd_ptr->filename();
                        return true;
                    }
                    return false;
                }))
        {
            // Found duplicate
            // std::cout << "Duplicate found! " << path << std::endl;
            m_duplicates[filename_found].emplace_back(std::move(new_fd_ptr->filename()));
        }
        else
        {
            // std::cout << "Unique file, putting to desctiptors " << path << std::endl;
            m_fs2fds[fs].emplace_back(std::move(new_fd_ptr));
            // m_duplicates[path] = std::list<std::string>>();
        }
    }
    Duplicates_t const& duplicates() override
    {
        return m_duplicates;
    }
    private:
    std::map<size_t, std::list<std::unique_ptr<FileDescriptor<T>>>> m_fs2fds;
    std::map<std::string, std::list<std::string>> m_duplicates;
    size_t m_blocksize;
};

} // namespace OTUS
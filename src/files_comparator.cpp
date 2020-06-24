#include "files_comparator.h"

#include <iostream>

OTUS::HashKind OTUS::hash_name_from_string(std::string hash_name)
{
    if (hash_name == "crc32")
        return HashKind::CRC32;
    if (hash_name == "md5")
        return HashKind::MD5;
    throw std::runtime_error("unsupported hash function!");
}


void OTUS::FilesComparator::update(Event const& ev)
{
    std::cout << event_type_name(ev.m_kind) << std::endl;
    if(ev.m_kind == EventKind::REGULAR_FILE)
    {
        std::cout << ev.m_filesize << " " << ev.m_path << std::endl;
    }
}

OTUS::FilesComparator::Report_t OTUS::FilesComparator::report() const
{
    std::cout << "Report!" << std::endl;;
    return Report_t{};
}
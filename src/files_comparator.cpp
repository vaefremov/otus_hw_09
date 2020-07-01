#include "files_comparator_impl.h"

#include <iostream>

OTUS::FilesComparator::FilesComparator(size_t block_sz, HashKind hash_kind, bool is_verbose): 
        m_block_sz(block_sz), m_hash_kind(hash_kind), m_verbose(is_verbose) 
{
    switch (hash_kind)
    {
    case HashKind::CRC32:
        m_pimpl = std::make_unique<FilesComparatorImpl<unsigned int>>(m_block_sz);
        break;
    case HashKind::MD5:
        m_pimpl = std::make_unique<FilesComparatorImpl<md5digest_t>>(m_block_sz);
        break;
    default:
        throw   std::invalid_argument("Unknown checksum type");
    }
}


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
    // std::cout << event_type_name(ev.m_kind) << std::endl;
    if(ev.m_kind == EventKind::REGULAR_FILE)
    {
        // std::cout << ev.m_filesize << " " << ev.m_path << std::endl;
        m_pimpl->add(ev.m_path.c_str(), ev.m_filesize);
    }
}

OTUS::FilesComparator::Report_t OTUS::FilesComparator::report() const
{
    Report_t res;
    auto duplicates = m_pimpl->duplicates();
    for(auto& r: duplicates)
    {
        res.emplace_back(std::vector<std::string>{r.first});
        for (auto& d: r.second)
        {
            res.back().emplace_back(d);
        }
    }

    return res;
}
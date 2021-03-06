#include "files_comparator_impl.h"
#include <exception>

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
    m_pimpl->set_verbose(m_verbose);
}


OTUS::HashKind OTUS::hash_name_from_string(std::string hash_name)
{
    if (hash_name == "crc32")
        return HashKind::CRC32;
    if (hash_name == "md5")
        return HashKind::MD5;
    throw std::invalid_argument("unsupported hash function!");
}


void OTUS::FilesComparator::update(Event const& ev)
{
    switch (ev.kind)
    {
    case EventKind::REGULAR_FILE:
        m_pimpl->add_path(ev.path.c_str(), ev.filesize);
        break;
    
    default: // The END_STREAM event ignored
        break;
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
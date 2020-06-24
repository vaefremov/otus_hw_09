#pragma once

#include <vector>
#include <memory>
#include <string>
#include "iobserver.h"
#include "scanner.h"

namespace OTUS
{

enum class HashKind
{
    DEFAULT, CRC32, MD5
};

HashKind hash_name_from_string(std::string hash_name);

class FilesComparator: public IObserver
{
    public:

    using Report_t=std::vector<std::vector<std::string>>;

    static std::shared_ptr<FilesComparator> create_subscribed(Scanner& sc, size_t block_sz, HashKind hash_kind, bool is_verbose=false)
    {
        auto ptr = std::make_shared<FilesComparator>(block_sz, hash_kind, is_verbose);
        sc.subscribe(ptr);
        return ptr;
    }
    FilesComparator() = delete;
    FilesComparator(size_t block_sz, HashKind hash_kind, bool is_verbose): 
        m_block_sz(block_sz), m_hash_kind(hash_kind), m_verbose(is_verbose) {};

    void update(Event const& ev) override;
    Report_t report() const;

    private:
    size_t m_block_sz;
    HashKind m_hash_kind;
    bool m_verbose;
};

}
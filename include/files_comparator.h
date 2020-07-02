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
    CRC32, MD5
};

HashKind hash_name_from_string(std::string hash_name);

class IFilesComparatorImpl
{
    public:

    using Duplicates_t = std::map<std::string, std::list<std::string>>;

    virtual ~IFilesComparatorImpl() = default;
    virtual void add_path(std::string path, size_t fs) = 0;
    virtual Duplicates_t const& duplicates() = 0;
    virtual void set_verbose(bool verbose=true) = 0;
};


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
    FilesComparator(size_t block_sz, HashKind hash_kind, bool is_verbose);

    void update(Event const& ev) override;
    Report_t report() const;

    private:
    size_t m_block_sz;
    HashKind m_hash_kind;
    bool m_verbose;
    std::unique_ptr<IFilesComparatorImpl> m_pimpl;
};

}
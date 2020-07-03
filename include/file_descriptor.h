#pragma once
#include <list>
#include <fstream>
#include <boost/crc.hpp>
#include <boost/uuid/detail/md5.hpp>
#include <array>
#include <exception>

namespace OTUS
{

template<typename T> T calcChecksum(size_t block_sz, char* buf);

template<>
unsigned int calcChecksum(size_t block_sz, char* buf)
{
    boost::crc_32_type result;
    result.process_bytes(buf, block_sz);
    return result.checksum();
}

using md5digest_t=std::array<unsigned int, 4>;

template<>
md5digest_t calcChecksum(size_t block_sz, char* buf)
{
    boost::uuids::detail::md5 hash;
    boost::uuids::detail::md5::md5::digest_type digest;
    hash.process_bytes(buf, block_sz);
    hash.get_digest(digest);
    std::array<unsigned int, 4> res{digest[0], digest[1], digest[2], digest[3]};
    return res;
}

template<typename T>
class FileDescriptor
{
    public:
    
    using BlockListIterator_t = typename std::list<T>::iterator;

    FileDescriptor(size_t blocksize, std::string path, size_t size): m_filename(std::move(path)), m_size(size), m_blocksize(blocksize){}

    bool operator==(FileDescriptor<T>& other)
    {
        if(m_size != other.m_size)
        {
            return false;
        }
        auto it_this = m_blocks.begin();
        auto it_other = other.m_blocks.begin();
        std::ifstream in_this;
        std::ifstream in_other;

        while(hasNextBlock(it_this))
        {
            if(!other.hasNextBlock(it_other))
            {
                return false;
            }
            if(nextBlock(it_this, in_this) != other.nextBlock(it_other, in_other))
            {
                return false;
            }
        }
        return !other.hasNextBlock(it_other);
    }

    std::string const& filename() const
    {
        return m_filename;
    }

    std::list<T> const& blocks() const
    {
        return m_blocks;
    }

    private:

    bool isComplete() const 
    {
        return m_blocks.size() * m_blocksize >= m_size;
    }

    bool hasNextBlock(BlockListIterator_t& it) const
    {
        return (it != m_blocks.end()) || !isComplete();
    }
    
    T& nextBlock(BlockListIterator_t& it, std::ifstream& in)
    {
        if(it == m_blocks.end() && !isComplete())
        {
            // read block and put it into the list
            char buf[m_blocksize];
            std::fill(buf, buf+m_blocksize, 0);
            if(!in.is_open())
            {
                in.open(m_filename, std::ios::binary);
            }
            in.seekg(m_blocksize*m_blocks.size());
            in.read(buf, m_blocksize); // Process read error!
            if(in.rdstate() && !in.eof())
            {
                throw std::runtime_error("Unable to read file: " + m_filename);
            }
            T csum = calcChecksum<T>(m_blocksize, buf);
            m_blocks.emplace_back(csum);
            return m_blocks.back();
        }
        return *it++;
    }


    std::string m_filename;
    size_t m_size;
    size_t m_blocksize;
    std::list<T> m_blocks;
};

}
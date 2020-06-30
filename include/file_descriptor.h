#pragma once
#include <list>
#include <fstream>

namespace OTUS
{

template<typename T> T calcChecksum(size_t block_sz, char* buf);

template<typename T>
class FileDescriptor
{
    public:
    
    using BlockListIterator_t = typename std::list<T>::iterator;

    FileDescriptor(size_t blocksize, std::string path, size_t size): m_filename(std::move(path)), m_size(size), m_blocksize(blocksize){}
    bool isComplete() const 
    {
        return m_blocks.size() * m_blocksize >= m_size;
    }
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
    std::string const& filename()
    {
        return m_filename;
    }

    std::list<T> const& blocks() const
    {
        return m_blocks;
    }

    private:

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
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
            // std::cout << "sizes differ " << m_size << " " << other.m_size << std::endl;
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
            // T bl_this = nextBlock(it_this, in_this);
            // T bl_other = other.nextBlock(it_other, in_other);
            if(nextBlock(it_this, in_this) != other.nextBlock(it_other, in_other))
            // if(bl_this != bl_other)
            {
                // std::cout << "blocks differ " << bl_this << " " << bl_other <<  std::endl;
                return false;
            }
            // std::cout << "blocks same " << std::endl;
        }
        return !other.hasNextBlock(it_other);
    }
    bool hasNextBlock(BlockListIterator_t& it) const
    {
        return (it != m_blocks.end()) || !isComplete();
    }
    T nextBlock(BlockListIterator_t& it, std::ifstream& in)
    {
        if(it == m_blocks.end() && !isComplete())
        {
            // read block and put it into the list
            char buf[m_blocksize];
            std::fill(buf, buf+m_blocksize, 0);
            if(!in.is_open())
            {
                // std::cout << "opening " << m_filename << std::endl;
                in.open(m_filename, std::ios::binary);
            }
            in.seekg(m_blocksize*m_blocks.size());
            in.read(buf, m_blocksize); // Process read error!
            // std::cout << m_filename << " putting block" << buf << std::endl;
            T csum = calcChecksum<T>(m_blocksize, buf);
            m_blocks.emplace_back(csum);
            return m_blocks.back();
        }
        return *it++;
    }
    std::string const& filename()
    {
        return m_filename;
    }

    private:
    
    std::string m_filename;
    size_t m_size;
    size_t m_blocksize;
    std::list<T> m_blocks;
};

}
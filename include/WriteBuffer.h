#pragma once

#include "Exception.h"
#include "FileBuffer.h"

namespace ExtSorter {

template<typename T>
class WriteBuffer : public FileBuffer<T>
{
public:
    WriteBuffer(const WriteBuffer&) = delete;
    void operator=(const WriteBuffer&) = delete;

    WriteBuffer(std::shared_ptr<std::fstream>& file, size_t buf_size)
        : FileBuffer<T>(file, buf_size)
    {
        if(m_file) m_file->seekp(0);
    }

    void push(T val);
    void flush();

private:
    using FileBuffer<T>::m_file;
    using FileBuffer<T>::m_type_size;
    using FileBuffer<T>::m_buf;
    using FileBuffer<T>::m_buf_pos;
};
}

template<typename T>
inline void ExtSorter::WriteBuffer<T>::push(const T val)
{
    m_buf[m_buf_pos] = val;
    if(++m_buf_pos >= m_buf.size())
    {
        flush();
    }
}

template<typename T>
inline void ExtSorter::WriteBuffer<T>::flush()
{
    if(m_buf_pos == 0)
        return;

    if(!m_file)
    {
        throw ExtSorter::Exception("write m_file is null");
    }

    if(!m_file->write((char*) m_buf.data(), m_buf_pos * m_type_size))
    {
        throw ExtSorter::Exception("can't write m_file!");
    }
    m_buf_pos = 0;
}
#pragma once

#include "Exception.h"
#include "FileBuffer.h"

namespace ExtSorter {

template<typename T>
class ReadBuffer : public FileBuffer<T>
{
public:
    ReadBuffer(const ReadBuffer&) = delete;
    void operator=(const ReadBuffer&) = delete;

    ReadBuffer(std::shared_ptr<std::fstream>& file, size_t buf_size, size_t end_pos)
        : FileBuffer<T>(file, buf_size)
        , m_end_pos(end_pos)
        , m_buf_loaded(false)
    {
        if(m_file) m_file->seekg(0);
    }

    const T get_el(size_t pos);

private:
    using FileBuffer<T>::m_file;
    using FileBuffer<T>::m_type_size;
    using FileBuffer<T>::m_buf;
    using FileBuffer<T>::m_buf_pos;
    size_t m_end_pos;
    bool m_buf_loaded;

    void load_buf(size_t pos);
};

}

template<typename T>
inline void ExtSorter::ReadBuffer<T>::load_buf(size_t pos)
{
    if(!m_file)
    {
        throw ExtSorter::Exception("read m_file is null");
    }
    m_file->seekg(static_cast<fpos_t>(pos * m_type_size));
    size_t el_read = m_buf.size();
    size_t el_left = m_end_pos - pos;
    if(el_left < el_read)
    {
        el_read = el_left;
    }

    if(!m_file->read((char*) m_buf.data(), el_read * m_type_size))
    {
        throw ExtSorter::Exception("can't read m_file!");
    }
    m_buf_pos = pos;
    m_buf_loaded = true;
}

template<typename T>
inline const T ExtSorter::ReadBuffer<T>::get_el(size_t pos)
{
    if(pos >= m_end_pos || pos < m_buf_pos)
    {
        throw ExtSorter::Exception("get_el position error!");
    }

    if(!m_buf_loaded || pos >= m_buf_pos + m_buf.size())
    {
        load_buf(pos);
    }

    return m_buf[pos - m_buf_pos];
}


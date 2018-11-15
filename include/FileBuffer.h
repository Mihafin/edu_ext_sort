#pragma once

#include <cstdlib>
#include <fstream>
#include <vector>
namespace ExtSorter {

template<typename T>
class FileBuffer
{
public:
    FileBuffer(const FileBuffer&) = delete;
    void operator=(const FileBuffer&) = delete;

    FileBuffer(std::shared_ptr<std::fstream>& file, size_t buf_size)
        : m_buf(buf_size)
        , m_file(file)
        , m_type_size(sizeof(T))
        , m_buf_pos(0)
    {};

protected:
    FileBuffer() = default;

    std::vector<T> m_buf;
    std::shared_ptr<std::fstream>& m_file;
    size_t m_type_size;
    size_t m_buf_pos;
};
}



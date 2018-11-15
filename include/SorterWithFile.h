#pragma once

#include <string>

namespace ExtSorter {

template<typename T>
class SorterWithFile
{
public:
    SorterWithFile(const SorterWithFile&) = delete;
    void operator=(const SorterWithFile&) = delete;

    SorterWithFile(std::string& file_path, size_t memory_limit)
        : m_type_size(sizeof(T))
        , m_file_path(file_path)
        , m_memory_limit(memory_limit)
    {};

    void sort();

private:
    size_t split(const size_t& buff_size, const size_t& file_size);
    void merge_chunks(const size_t& buff_size, const size_t& file_size);

    size_t m_type_size;
    std::string& m_file_path;
    size_t m_memory_limit;
};

}
#include "SorterWithFile_impl.h"

#include <sys/stat.h>
#include <fstream>
#include "Exception.h"
#include "Utils.h"

const size_t ExtSorter::Utils::get_file_size(const char* file_path)
{
    struct stat results{};
    if(stat(file_path, &results) != 0)
        return 0;

    return static_cast<const size_t>(results.st_size);
}

void ExtSorter::Utils::copy(std::shared_ptr<std::fstream>& from_file, std::shared_ptr<std::fstream>& to_file, size_t m_memory_limit, size_t file_size)
{
    if(!from_file || !to_file)
    {
        throw ExtSorter::Exception("from_file or to_file is null!");
    }
    fpos_t pos = 0;
    std::vector<char> buf(m_memory_limit);
    size_t len;

    while (pos < file_size)
    {
        if(pos + m_memory_limit > file_size)
        {
            len = file_size - pos;
        }
        else
        {
            len = m_memory_limit;
        }
        from_file->seekg(pos);
        if(!from_file->read(buf.data(), len)) throw ExtSorter::Exception("can't read from_file!");
        to_file->seekp(pos);
        if(!to_file->write(buf.data(), len)) throw ExtSorter::Exception("can't write to_file!");
        pos += len;
    }
}
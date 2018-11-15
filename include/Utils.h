#pragma once

#include <cstdlib>
#include <string>
#include <fstream>
#include <random>
#include <iostream>
#include "Exception.h"

namespace ExtSorter
{
struct Utils
{
    static const size_t get_file_size(const char* file_path);

    template <typename T>
    static void fill(const std::string& file_path, size_t file_size, bool fill_test_data = false);

    template<typename T>
    static void check_result(const std::string& file_path);

    static void copy(std::shared_ptr<std::fstream>& from_file, std::shared_ptr<std::fstream>& to_file, size_t m_memory_limit, size_t file_size);

};
}


template<typename T>
void ExtSorter::Utils::fill(const std::string& file_path, size_t file_size, bool fill_test_data)
{
    size_t type_size = sizeof(T);
    std::fstream file(file_path, std::ios::out | std::ios::binary | std::ios::trunc);
    if(!file)
    {
        throw ExtSorter::Exception("file to fill does't exist");
    }
    if (fill_test_data)
    {
//        T arr[] = {1, 1, 2, 2, 1, 1, 1, 2, 1, 2, 3, 4, 5, 6, 8, 1, 1, 9, 0};
        T arr[] = {1, 1, 2, 2,   1, 1, 1, 2,   1, 2, 3, 4,   5, 6, 8, 1};
        file_size = sizeof(arr);
        size_t arr_size = file_size / type_size;

        for(int i = 0; i < arr_size; ++i)
        {
            std::cout << arr[i];
            if(i < arr_size - 1) std::cout << ", ";
            if(!file.write((char*) &arr[i], type_size)) throw ExtSorter::Exception("can't write to filled file!");
        }
        std::cout << std::endl;
    }
    else
    {
        int trace_cnt = 5;
        size_t arr_size = file_size / type_size;
        std::default_random_engine generator;
        std::uniform_int_distribution<T> num(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
        generator.seed(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
        for (size_t i = 0; i < arr_size; ++i) {
            T a = num(generator);
            if(i < trace_cnt || i > arr_size - trace_cnt)
                std::cout << i << ": " << a << std::endl;
            if(i==trace_cnt) std::cout << "..." << std::endl;
            if(!file.write((char *) &a, type_size)) throw ExtSorter::Exception("can't write to filled file!");
        }
    }
    file.close();

}

template<typename T>
void ExtSorter::Utils::check_result(const std::string& file_path)
{
    size_t file_size = get_file_size(file_path.c_str());
    if(file_size == 0) throw ExtSorter::Exception("checked file size == 0!");
    std::fstream file(file_path, std::ios::in | std::ios::binary);
    if(!file) throw ExtSorter::Exception("checked file does't exist!");

    file.seekg(0);
    size_t type_size = sizeof(T);
    size_t el_cnt = file_size / type_size;
    size_t cnt = 0;
    T val, prev;
    for (size_t j = 0; j < el_cnt; ++j)
    {
        if(!file.read((char*) &val, type_size)) throw ExtSorter::Exception("can't reed checked file!");
        if(j==0)
        {
            prev = val;
            continue;
        }

        if(val < prev)
        {
            throw ExtSorter::Exception("check result error!");
        }
        ++cnt;
        prev = val;
    }
}





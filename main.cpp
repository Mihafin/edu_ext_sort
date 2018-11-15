#include <iostream>
#include "include/Exception.h"
#include "include/Utils.h"
#include "include/SorterWithFile.h"

//проверка памяти:
// valgrind --tool=massif ./cpp_edu_ext_sort

//todo: 2 реализация: в несколько потоков.

int main()
{
    try
    {
        std::string file_path = "bigdata.bin";

        //заполняем файл рандомными значениями
        ExtSorter::Utils::fill<int64_t>(file_path, 1000000000);

        //сортировка с использованием доп.файла тойже размерности что и исходный
        auto start_time = std::chrono::steady_clock::now();
        ExtSorter::SorterWithFile<int64_t> sorter(file_path, 100000000);
        sorter.sort();
        auto end_time = std::chrono::steady_clock::now();
        auto elapsed_ns = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        std::cout << elapsed_ns.count() << " ms\n";


        //проход по всему файлу и проверка что все числа идут по не убыванию
        ExtSorter::Utils::check_result<int64_t>(file_path);
    }
    catch(const ExtSorter::Exception& e)
    {
        std::cerr << "ExtSorter error! " << e.what() << std::endl;
        exit(1);
    }
    catch(std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        exit(1);
    }
    return 0;
}

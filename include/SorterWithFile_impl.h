#include <fstream>
#include <random>
#include "Utils.h"
#include "Exception.h"
#include "WriteBuffer.h"
#include "ReadBuffer.h"
#include "SorterWithFile.h"

template<typename T>
void ExtSorter::SorterWithFile<T>::sort()
{
    const size_t file_size = ExtSorter::Utils::get_file_size(m_file_path.c_str());
    size_t buff_size = (m_memory_limit / m_type_size) * m_type_size;

    if(buff_size == 0 || file_size == 0)
    {
        throw ExtSorter::Exception("file_size or buff_size is invalid!");
    }

    if(buff_size < m_type_size * 4)
    {
        std::string mes = "buff_size must be >= " + std::to_string(m_type_size * 4);
        throw ExtSorter::Exception(std::move(mes));
    }

    //сортируем кусками buff_size и кладем в исходный файл
    size_t chunk_cnt = split(buff_size, file_size);
    if(chunk_cnt <= 1) return; //кусок один (или менее), следовательно уже отсортирован в исходном файле

    //сливаем куски используя дополнительный файл размера исходного
    merge_chunks(buff_size, file_size);
}

template<typename T>
size_t ExtSorter::SorterWithFile<T>::split(const size_t& buff_size, const size_t& file_size)
{
    size_t chunk_cnt = (file_size + buff_size - 1) / buff_size;
    size_t buf_el_cnt = buff_size / m_type_size;

    std::cout << "SPLIT! filesize=" << file_size << ", buff_size=" << buff_size
              << ", chunk_cnt=" << chunk_cnt << ", buf_el_cnt=" << buf_el_cnt << std::endl;

    std::fstream file(m_file_path, std::ios::in | std::ios::out | std::ios::binary);
    if(!file) throw ExtSorter::Exception("split file is null");
    std::vector<T> buffer(buf_el_cnt);

    size_t cur_buf_size, cur_el_cnt;
    size_t cur_pos = 0;
    for(size_t i = 0; i < chunk_cnt; ++i)
    {
        cur_pos = i * buff_size;
        cur_buf_size = ((cur_pos + buff_size) > file_size) ? file_size % buff_size : buff_size;
        cur_el_cnt = cur_buf_size / m_type_size;

        // read
        file.seekg(static_cast<fpos_t>(cur_pos));
        if(!file.read((char*) buffer.data(), cur_buf_size)) throw ExtSorter::Exception("can't read split file!");

        // sort
        std::sort(buffer.begin(), buffer.begin() + cur_el_cnt);

        // write in src file
        file.seekp(static_cast<fpos_t>(cur_pos));
        if(!file.write((char*) buffer.data(), cur_buf_size)) throw ExtSorter::Exception("can't write split file!");
    }
    file.close();

    return chunk_cnt;
}

template<typename T>
void ExtSorter::SorterWithFile<T>::merge_chunks(const size_t& buff_size, const size_t& file_size)
{
    std::string tmp_file_path = "tmp.bin";
    size_t el_cnt = file_size / m_type_size;
    size_t buf_el_cnt = buff_size / m_type_size;
    size_t seq_buf = buf_el_cnt / 4;    // размер буфера для сливаемых последовательностей
    size_t res_buf = 2 * seq_buf;       // размер буфера для результата = x2 буфера для последовательностей

    //за основу взята итеративная реализация сортировки https://ru.wikipedia.org/wiki/Сортировка_слиянием
    size_t BlockSizeIterator;
    size_t BlockIterator;
    size_t LeftBlockIterator;
    size_t RightBlockIterator;

    size_t LeftBorder;
    size_t MidBorder;
    size_t RightBorder;

    auto src_file = std::make_shared<std::fstream>(m_file_path, std::ios::in | std::ios::out | std::ios::binary);
    auto tmp_file = std::make_shared<std::fstream>(tmp_file_path,
                                                   std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
    auto cur_file = src_file;

    std::cout << "MERGE! el_cnt=" << el_cnt << ", seq_buf=" << seq_buf << ", res_buf=" << res_buf << std::endl;
    for(BlockSizeIterator = buf_el_cnt; BlockSizeIterator < el_cnt; BlockSizeIterator *= 2)
    {
        //файл в который сливаются последовательности. используя буфер, чтобы не сразу писать в файл, что очень тормозно
        WriteBuffer<T> result((cur_file == src_file) ? tmp_file : src_file, res_buf);

        size_t last_processed_el = 0;
        for(BlockIterator = 0; BlockIterator < el_cnt - BlockSizeIterator; BlockIterator += 2 * BlockSizeIterator)
        {
            //Производим слияние с сортировкой пары блоков начинающуюся с элемента BlockIterator
            //левый размером BlockSizeIterator, правый размером BlockSizeIterator или меньше
            LeftBlockIterator = 0;
            RightBlockIterator = 0;
            LeftBorder = BlockIterator;
            MidBorder = BlockIterator + BlockSizeIterator;
            RightBorder = BlockIterator + 2 * BlockSizeIterator;
            RightBorder = (RightBorder < el_cnt) ? RightBorder : el_cnt;
            last_processed_el = RightBorder;

            //структуры в которые подтягиваются элементы из файла
            ReadBuffer<T> left(cur_file, seq_buf, MidBorder);
            ReadBuffer<T> right(cur_file, seq_buf, RightBorder);

            //Пока в обоих массивах есть элементы выбираем меньший из них и заносим в отсортированный блок
            T left_val, right_val;
            while(LeftBorder + LeftBlockIterator < MidBorder && MidBorder + RightBlockIterator < RightBorder)
            {
                left_val = left.get_el(LeftBorder + LeftBlockIterator);
                right_val = right.get_el(MidBorder + RightBlockIterator);
                if(left_val <= right_val)
                {
                    result.push(left_val);
                    ++LeftBlockIterator;
                }
                else
                {
                    result.push(right_val);
                    ++RightBlockIterator;
                }
            }
            //После этого заносим оставшиеся элементы из левого или правого блока
            while(LeftBorder + LeftBlockIterator < MidBorder)
            {
                result.push(left.get_el(LeftBorder + LeftBlockIterator));
                ++LeftBlockIterator;
            }
            while(MidBorder + RightBlockIterator < RightBorder)
            {
                result.push(right.get_el(MidBorder + RightBlockIterator));
                ++RightBlockIterator;
            }
        }

        //дозаливаем оставшиеся необработанные элементы
        if(last_processed_el < el_cnt)
        {
            ReadBuffer<T> left(cur_file, 2 * seq_buf, el_cnt);
            while(last_processed_el < el_cnt)
            {
                result.push(left.get_el(last_processed_el));
                ++last_processed_el;
            }
        }
        result.flush();

        //меняем источник
        cur_file = (cur_file == src_file) ? tmp_file : src_file;
    }

    // если текущий отсортированный файл не исходный, то просто копируем его обратно.
    // как вариант можно не копировать, а возвращать указатель на отсортированный файл
    // или переименовывать файлы
    if(cur_file != src_file)
    {
        std::cout << "copy" << std::endl;
        Utils::copy(cur_file, src_file, m_memory_limit, file_size);
    }

    src_file->close();
    tmp_file->close();

    remove(tmp_file_path.c_str());
}


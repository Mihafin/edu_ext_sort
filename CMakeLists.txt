cmake_minimum_required ( VERSION 3.8 )
project ( cpp_edu_ext_sort )

set ( CMAKE_CXX_STANDARD 11 )

set ( SOURCE_FILES
        include/Exception.h
        src/Utils.cpp include/Utils.h
        include/SorterWithFile.h include/SorterWithFile_impl.h
        include/FileBuffer.h
        include/WriteBuffer.h
        include/ReadBuffer.h )

include_directories(include)

add_executable ( cpp_edu_ext_sort ${SOURCE_FILES} main.cpp)
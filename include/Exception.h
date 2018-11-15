#pragma once

#include <exception>
#include <string>
#include <utility>

namespace ExtSorter {

class Exception : std::exception
{
public:
    explicit Exception(std::string&& message)
        :m_message(message) {}

    const char* what() const noexcept override
    {
        return m_message.c_str();
    }

private:
    std::string m_message;
};

}


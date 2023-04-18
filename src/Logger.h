#ifndef LOGGER_H
#define LOGGER_H

#include <spdlog/spdlog.h>
#include <spdlog/fmt/fmt.h>
#include <spdlog/common.h>
#include <spdlog/pattern_formatter.h>

#include "./HttpResponseHeader.h"

#include <iostream>

class Logger
{
public:
    static std::string a;
    Logger() {
        
    };

    class my_formatter_flag : public spdlog::custom_flag_formatter
    {
    public:
        void format(const spdlog::details::log_msg &, const std::tm &, spdlog::memory_buf_t &dest) override
        {
            a[0]++;
            dest.append(a.data(), a.data() + a.size());
        }

        std::unique_ptr<custom_flag_formatter> clone() const override
        {
            return spdlog::details::make_unique<my_formatter_flag>();
        }
    };

private:
    
};

#endif
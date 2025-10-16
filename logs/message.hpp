//message.hpp

#pragma once

#include "level.hpp"
#include "util.hpp"
#include <ctime>
#include <iostream>
#include <string>
#include <thread>

namespace MySpace {
    class LogMsg {
        public:
        time_t _ctime;                   // 日志产生的时间戳
        LogLevel::value _level;          // 日志等级
        std::string _file;               // 源文件名称
        size_t _line;                    // 源文件行号
        std::thread::id _tid;            // 线程ID
        std::string _payload;            // 有效载荷，日志主体消息
        std::string _logger;             // 日志器

        LogMsg(LogLevel::value level
            , size_t line
            , const std::string file
            , const std::string logger
            , const std::string msg) 
            : _level(level)
            , _ctime(util::getCurTime())
            , _line(line)
            , _file(file)
            , _logger(logger)
            , _payload(msg)
            , _tid(std::this_thread::get_id()) 
        {}
    };
}


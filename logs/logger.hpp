//logger.hpp
#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <memory>
#include <atomic>
#include <condition_variable> 
#include "buffer.hpp"
#include "format.hpp"
#include "level.hpp"
#include "looper.hpp"
#include "message.hpp"
#include "sink.hpp"
#include "util.hpp"

namespace MySpace{
    class Logger {
        public:
            Logger(const std::string &logger_name
                , MySpace::LogLevel::value limit_level
                , std::shared_ptr<MySpace::Formatter > formatter
                , std::vector<std::shared_ptr<MySpace::LogSink >> sinks)
                :_logger_name(logger_name)
                , _limit_level(limit_level)
                , _formatter(formatter)
                , _sinks(sinks.begin(), sinks.end())
            {}
            //获取日志器名称
            const std::string &name(){ return _logger_name; }
            /* 构造日志消息对象过程， 并得到格式化后的日志消息字符串-- 然后进行落地输出*/
            void debug(const std::string& file, size_t line, const std::string &fmtStr){
                logMessage(LogLevel::DEBUG, file, line, fmtStr);
            }
            void info(const std::string& file, size_t line, const std::string &fmtStr){
                logMessage(LogLevel::INFO, file, line, fmtStr);
            }
            void warn(const std::string& file, size_t line, const std::string &fmtStr){
                logMessage(LogLevel::WARN, file, line, fmtStr);
            }
            void error(const std::string& file, size_t line, const std::string &fmtStr){
                logMessage(LogLevel::ERROR, file, line, fmtStr);
            }
            void fatal(const std::string& file, size_t line, const std::string &fmtStr){
                logMessage(LogLevel::FATAL, file, line, fmtStr);
            }
        protected:
            void logMessage(MySpace::LogLevel::value level, const std::string& file, size_t line, const std::string &message) {
                /* 通过传入的参数构造出一个日志消息对象，进行日志格式化，最终落地*/
                // 1、 判断当前日志等级是否达到输出标准
                if (level < _limit_level)  
                    return;
                // 2、 构造LogMsg对象
                LogMsg msg(level, line, file, _logger_name, message);
                // 3、 通过格式化工具对LogMsg进行格式化，获得格式化后的日志字符串
                std::string real_message = _formatter->format(msg);
                // 4、 进行日志落地
                log(real_message.c_str(), real_message.size());
            }
            /* 抽象接口完成实际的落地输出 -- 不同的日志器会有不同的实际落地方式 */
            virtual void log(const std::string& data, size_t len) = 0;
        protected:
            std::mutex _mutex;
            std::string _logger_name;
            std::atomic<MySpace::LogLevel::value> _limit_level;    
            std::shared_ptr<MySpace::Formatter> _formatter;//使用shared_ptr，因为Formatter是可拷贝的
            std::vector<std::shared_ptr<MySpace::LogSink>> _sinks;
    };

    enum LoggerType {
        LOGGER_SYNCH,   //同步日志器
        LOGGER_ASYNCH   //异步日志器
    };

    class SynchLogger : public Logger {
    public:
        SynchLogger(const std::string &logger_name
            , MySpace::LogLevel::value limit_level
            , std::shared_ptr<MySpace::Formatter> formatter
            , std::vector<std::shared_ptr<MySpace::LogSink>> sinks)
            : Logger(logger_name, limit_level, formatter, sinks)
        {}
    protected:
        /* 同步日志器，是将日志直接通过落地模块 句柄进行日志落地 */
        void log(const std::string& data, size_t len) override{
            std::unique_lock<std::mutex> lock(_mutex);
            if (_sinks.empty()) return;
            for (auto &sink : _sinks) {
                sink->log(data, len);
            }
        }
    };

    class AsynchLogger : public Logger {
        public:
            AsynchLogger(const std::string &logger_name
                , LogLevel::value level
                , std::shared_ptr<Formatter> formatter
                , std::vector<std::shared_ptr<LogSink>> sinks)
                : Logger(logger_name, level, formatter, sinks)
                , _looper(std::make_shared<AsynchLooper>([this](Buffer &buf) { realLog(buf); }))
            {}

            /* 将数据写入缓冲区*/
            virtual void log(const std::string& data, size_t len) override{
                _looper->push(data.c_str(), len);
            }

            /* 设计一个实际落地函数（将缓冲区中的数据落地） */
            void realLog(Buffer &buf) {
                if (_sinks.empty()) return;
                for (auto &sink : _sinks) {
                    sink->log(buf.begin(), buf.readAbleSize());
                }
            }
        
        private: 
            std::shared_ptr<AsynchLooper> _looper;
    };
    
    
    /* 简单工厂类 - 用于创建日志器 */
    class LoggerFactory {
    public:
        // 创建同步日志器
        static std::shared_ptr<Logger> createSynchLogger(
            const std::string &name,
            LogLevel::value level = LogLevel::DEBUG,
            const std::string &pattern = "")
        {
            auto formatter = pattern.empty() ? 
                std::make_shared<Formatter>() : 
                std::make_shared<Formatter>(pattern);
            
            std::vector<std::shared_ptr<LogSink>> sinks = {
                std::make_shared<StdoutSink>()
            };
            
            return std::make_shared<SynchLogger>(name, level, formatter, sinks);
        }
        
        // 创建同步日志器（带自定义 sinks）
        static std::shared_ptr<Logger> createSynchLogger(
            const std::string &name,
            LogLevel::value level,
            const std::string &pattern,
            const std::vector<std::shared_ptr<LogSink>> &sinks)
        {
            auto formatter = pattern.empty() ? 
                std::make_shared<Formatter>() : 
                std::make_shared<Formatter>(pattern);
            
            auto final_sinks = sinks.empty() ? 
                std::vector<std::shared_ptr<LogSink>>{std::make_shared<StdoutSink>()} : 
                sinks;
            
            return std::make_shared<SynchLogger>(name, level, formatter, final_sinks);
        }
        
        // 创建异步日志器
        static std::shared_ptr<Logger> createAsynchLogger(
            const std::string &name,
            LogLevel::value level = LogLevel::DEBUG,
            const std::string &pattern = "")
        {
            auto formatter = pattern.empty() ? 
                std::make_shared<Formatter>() : 
                std::make_shared<Formatter>(pattern);
            
            std::vector<std::shared_ptr<LogSink>> sinks = {
                std::make_shared<StdoutSink>()
            };
            
            return std::make_shared<AsynchLogger>(name, level, formatter, sinks);
        }
        
        // 创建异步日志器（带自定义 sinks）
        static std::shared_ptr<Logger> createAsynchLogger(
            const std::string &name,
            LogLevel::value level,
            const std::string &pattern,
            const std::vector<std::shared_ptr<LogSink>> &sinks)
        {
            auto formatter = pattern.empty() ? 
                std::make_shared<Formatter>() : 
                std::make_shared<Formatter>(pattern);
            
            auto final_sinks = sinks.empty() ? 
                std::vector<std::shared_ptr<LogSink>>{std::make_shared<StdoutSink>()} : 
                sinks;
            
            return std::make_shared<AsynchLogger>(name, level, formatter, final_sinks);
        }
    };


    
}
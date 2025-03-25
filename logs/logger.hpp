//logger.hpp
#pragma once
#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include <mutex>
#include <memory>
#include <atomic>
#include <cstdarg>
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
            void debug(const std::string& file, size_t line, const std::string &fmt, ...){
                va_list ap;
                va_start(ap, fmt);
                logMessage(LogLevel::value::DEBUG, file, line, fmt, ap);
                va_end(ap);

            }
            void info (const std::string& file, size_t line, const std::string &fmt, ...){
                va_list ap;
                va_start(ap, fmt);
                logMessage(LogLevel::value::INFO, file, line, fmt, ap);
                va_end(ap);

            }
            void warn (const std::string& file, size_t line, const std::string &fmt, ...){
                va_list ap;
                va_start(ap, fmt);
                logMessage(LogLevel::value::WARN, file, line, fmt, ap);
                va_end(ap);

            }
            void error(const std::string& file, size_t line, const std::string &fmt, ...){
                va_list ap;
                va_start(ap, fmt);
                logMessage(LogLevel::value::ERROR, file, line, fmt, ap);
                va_end(ap);

            }
            void fatal(const std::string& file, size_t line, const std::string &fmt, ...){
                // 2、 对fmt格式化字符串和不定参进行字符串组织，得到日志消息的字符串
                va_list ap;
                va_start(ap, fmt);
                logMessage(LogLevel::value::FATAL, file, line, fmt, ap);
                va_end(ap);

            }
        protected:
            void logMessage(MySpace::LogLevel::value level, const std::string& file, size_t line, const std::string &fmt, va_list ap) {
                /* 通过传入的参数构造出一个日志消息对象，进行日志格式化，最终落地*/
                // 1、 判断当前日志等级是否达到输出标准
                if (level < _limit_level)  
                    return;
                // 2、 对fmt格式化字符串和不定参进行字符串组织，得到日志消息的字符串
                char *res = nullptr;
                int ret = vasprintf(&res, fmt.c_str(), ap);
                if (ret == -1) { 
                    std::cout << "vasprintf failed! " << std::endl; 
                    return;
                }
                // 3、 构造LogMsg对象
                LogMsg msg(level, line, file, _logger_name, res);
                // 4、 通过格式化工具对LogMsg进行格式化，获得格式化后的日志字符串
                std::string real_message = _formatter->format(msg);
                // 5、 进行日志落地
                log(real_message.c_str(), real_message.size());
                free(res);// vasprintf() 内部开辟空间了，是动态申请的，需要我们手动释放
            }
            /* 抽象接口完成实际的落地输出 -- 不同的日志器会有不同的实际落地方式 */
            virtual void log(const std::string& data, size_t len) = 0;
        protected:
            std::mutex _mutex;
            std::string _logger_name;
            std::atomic<MySpace::LogLevel::value> _limit_level;    
            std::shared_ptr<MySpace::Formatter> _formatter;
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
                , std::shared_ptr<Formatter> &formatter
                , std::vector<std::shared_ptr<LogSink>> &sinks)
                : Logger(logger_name, level, formatter, sinks)
                , _looper(std::make_shared<AsynchLooper>(std::bind(&AsynchLogger::realLog, this, std::placeholders::_1)))
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
    
    
    class LoggerBuilder {
        public:
            LoggerBuilder()
                : _logger_type(LoggerType::LOGGER_SYNCH)
                , _limit_level(LogLevel::value::DEBUG)
            {}
            void buildLoggerType(LoggerType type)                 { _logger_type = type; }
            void buildLoggerName(const std::string &name)         { _logger_name = name; }
            void buildLoggerLevel(LogLevel::value level)          { _limit_level = level;  }
            void buildLoggerFormatter(const std::string &pattern) {  _formatter.reset(new Formatter(pattern)); }
            template<typename SinkType, typename ...Args>
            void buildSink(Args&&... args) { _sinks.push_back(SinkFactory::create<SinkType>(std::forward<Args>(args)...)); }
            virtual std::shared_ptr<Logger> build() = 0; //  建造日志器
        protected:
            LoggerType       _logger_type;
            std::string      _logger_name;
            LogLevel::value  _limit_level;    // 需要频繁访问
            std::shared_ptr<Formatter>   _formatter;
            std::vector<std::shared_ptr<LogSink>>  _sinks;
   };

   class LocalLoggerBuilder : public LoggerBuilder {
        public:
            virtual std::shared_ptr<Logger> build() override {
                assert(!_logger_name.empty());      // 必须有日志器名称
                if (_formatter.get() == nullptr) { _formatter = std::make_shared<Formatter>(); }
                if (_sinks.empty()) { buildSink<StdoutSink>(); }
                if (_logger_type == LoggerType::LOGGER_ASYNCH) {//异步日志器
                    return std::make_shared<AsynchLogger>(_logger_name, _limit_level, _formatter, _sinks);
                }
                //同步日志器
                return std::make_shared<SynchLogger>(_logger_name, _limit_level, _formatter, _sinks);
            }
    };

    //日志器建造者-懒汉模式
    class LoggerManager {
        public:
            static LoggerManager& getInstance(){
                //声明，静态局部变量没有构造完成之前，其他线程就会阻塞
                static LoggerManager eton;
		        return eton;
            }
            //添加
            void addLogger(std::shared_ptr<Logger> &logger){
                //防止重复添加
                if(findLogger(logger->name()))return;
                std::unique_lock<std::mutex>(_mutex);
                _loggers.insert(std::make_pair(logger->name(), logger));
            }
            //查找
            bool findLogger(const std::string &name){
                std::unique_lock<std::mutex>(_mutex);
                //没找到
                if(_loggers.find(name) == _loggers.end()){
                    return false;
                }
                return true;
            }
            //获取
            std::shared_ptr<Logger> getLogger(const std::string &name){
                std::unique_lock<std::mutex>(_mutex);
                //没找到
                if(_loggers.find(name) == _loggers.end()){
                    return std::shared_ptr<Logger>();
                }
                return _loggers.find(name)->second;
            }
            std::shared_ptr<Logger> rootLogger() { return _root_logger; }
        private:
            //构造函数私有
            LoggerManager() {
                std::shared_ptr<LoggerBuilder> LoggerBuilder(new MySpace::LocalLoggerBuilder());
                LoggerBuilder->buildLoggerName("root");
                _root_logger = LoggerBuilder->build();
                _loggers.insert(std::make_pair("root", _root_logger));
            }                              
            LoggerManager(const LoggerManager&) = delete;   //删除拷贝构造
            std::mutex _mutex;
            std::shared_ptr<Logger> _root_logger;          // 默认日志器
            std::unordered_map<std::string, std::shared_ptr<Logger>> _loggers;
    };

    /* 全局日志器建造者 -- 在局部的基础上新增：自动添加日志器到单例对象中 */
    class GlobalLoggerBuilder : public LoggerBuilder {
        public:
            virtual std::shared_ptr<Logger> build() override {
                assert(!_logger_name.empty());      // 必须有日志器名称
                if (_formatter.get() == nullptr) { _formatter = std::make_shared<Formatter>(); }
                if (_sinks.empty()) { buildSink<StdoutSink>(); }
                std::shared_ptr<Logger> logger;
                if (_logger_type == LoggerType::LOGGER_ASYNCH) {
                    logger = std::make_shared<AsynchLogger>(_logger_name, _limit_level, _formatter, _sinks);
                } else {
                    logger = std::make_shared<SynchLogger>(_logger_name, _limit_level, _formatter, _sinks);
                }
                LoggerManager::getInstance().addLogger(logger);   // 新增
                return logger;
            }
    };


    
}
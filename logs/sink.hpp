//sink.hpp
#pragma once
#include "level.hpp"
#include "util.hpp"
#include "message.hpp"
#include <string>
#include <iostream>
#include <assert.h>
#include <fstream>

namespace MySpace{
    class LogSink {
        public:
            virtual void log(const std::string& data, size_t len) = 0;
    };
    // 落地方向： 标准输出
    class StdoutSink : public LogSink {
        public:
            // 将日志消息写到标准输出,定长输出
            void log(const std::string& data, size_t len) override {
                //从0开始截取长度为len
                std::string str(data, 0, len);
                std::cout << str.c_str()<< std::endl;
            }
    };
    // 落地方向： 指定文件
    class FileSink : public LogSink {
        public: 
            // 构造时传入文件名
            FileSink(const std::string &pathname) {
                // 1、 创建日志文件所在的目录
                util::createDirectory(util::getDirectory(pathname));
                // 2、 创建并打开日志文件
                _ofs.open(pathname, std::ios::binary | std::ios::app);
            }
            void log(const std::string& data, size_t len) override {
                _ofs.write(data.c_str(), len);
                if (_ofs.fail()) {
                    std::cerr << "Failed to write to file." << std::endl;
                }
            }
        private:
            std::string _pathname;
            std::ofstream _ofs;
    };
    // 落地方向： 滚动文件，按大小
    class RollBySizeSink : public LogSink {
        public:
            //用户决定文件基本名字和文件大小
            RollBySizeSink(const std::string &basename, size_t max_size)
                : _basename(basename)
                , _max_fsize(max_size)
                , _cur_fsize(0)
                , _name_count(0)
            {
                std::string pathname = createNewFile();
                util::createDirectory(util::getDirectory(pathname));
                _ofs.open(pathname, std::ios::binary | std::ios::app);
            }
            void log(const std::string& data, size_t len) override{
                if (_cur_fsize + len >= _max_fsize) {
                    _ofs.close();                         // 关闭原来已经打开的文件
                    std::string pathname = createNewFile();
                    _cur_fsize = 0;
                    util::createDirectory(util::getDirectory(pathname));
                    _ofs.open(pathname, std::ios::binary | std::ios::app);
                }
                _ofs.write(data.c_str(), len);
                _cur_fsize += len;
            }
        private:
            //根据时间创建新的滚动文件
            std::string createNewFile(){
                _name_count += 1;
                // 获取系统时间，以时间来构建文件扩展名
                time_t t = util::getCurTime();
                struct tm lt;
                localtime_r(&t, &lt);
                std::string filename;
                filename += _basename;
                filename += std::to_string(lt.tm_year + 1900);
                filename += "-";
                filename += std::to_string(lt.tm_mon + 1);
                filename += "-";
                filename += std::to_string(lt.tm_mday);
                filename += " ";
                filename += std::to_string(lt.tm_hour);
                filename += ":";
                filename += std::to_string(lt.tm_min);
                filename += ":";
                filename += std::to_string(lt.tm_sec);
                filename += "-";
                filename += std::to_string(_name_count);
                filename += ".log";
                return filename;
            }
        private:
            std::string _basename;   // 基础文件名   
            std::ofstream _ofs;      // 操作句柄        
            size_t _max_fsize;       // 记录文件允许存储最大数据量,超过大小就要切换文件
            size_t _cur_fsize;       // 记录当前文件已经写入数据大小
            size_t _name_count;      // 滚动文件数量
    };

    class SinkFactory {
        public:
            template<class T, class ...Args>
            static std::unique_ptr<LogSink> create(Args&&... args) {
                return std::make_unique<T>(std::forward<Args>(args)...);
            }
    };
    /*
    * 使用示例与模板实例化说明：
    * 
    * 1. 标准输出 Sink（无参数）：
    *    auto sink = SinkFactory::create<StdoutSink>();
    *    
    *    实例化后等价于：
    *    static std::unique_ptr<LogSink> create() {
    *        return std::make_unique<StdoutSink>();
    *    }
    * 
    * 2. 文件输出 Sink（1个参数）：
    *    auto sink = SinkFactory::create<FileSink>("app.log");
    *    
    *    实例化后等价于：
    *    static std::unique_ptr<LogSink> create(const char*&& arg) {
    *        return std::make_unique<FileSink>(std::forward<const char*>(arg));
    *    }
    * 
    * 3. 滚动文件 Sink（2个参数）：
    *    auto sink = SinkFactory::create<RollBySizeSink>("./logs/roll-", 1024 * 1024);
    *    
    *    实例化后等价于：
    *    static std::unique_ptr<LogSink> create(const char*&& arg0, int&& arg1) {
    *        return std::make_unique<RollBySizeSink>(
    *            std::forward<const char*>(arg0),
    *            std::forward<int>(arg1)
    *        );
    *    }
    */


}
//format.hpp
#pragma once
#include "level.hpp"
#include "util.hpp"
#include "message.hpp"
#include <vector>
#include <iostream>
#include <unordered_map>
#include <sstream>
#include <assert.h>

namespace MySpace{
    class FormatItem {
    public:
        virtual void format(std::ostream& out, LogMsg& msg) = 0;
    };
    class payloadFormatItem : public FormatItem{
        public:
        virtual void format(std::ostream& out, LogMsg& msg) override{
            out<<msg._payload;
        }
    };
    class levelFormatItem : public FormatItem{
        public:
        virtual void format(std::ostream& out, LogMsg& msg) override{
            //日志等级在LogLevel域中定义
            //不能用to_string 没有定义这个枚举类型
            out<<LogLevel::toString(msg._level);
        }
    };
    class ctimeFormatItem : public FormatItem{
        public:
        ctimeFormatItem(const std::string &fmt) 
            : fmt_time(fmt) 
        {}
        virtual void format(std::ostream& out, LogMsg& msg) override {
            struct tm t;
            localtime_r(&msg._ctime, &t);
            char buff[32] = {0};
            strftime(buff, sizeof(buff), fmt_time.c_str(), &t);
            out << buff;
        }
        private: 
            std::string fmt_time; // %H:%M:%S
    };
    class fileFormatItem : public FormatItem{
        public:
        virtual void format(std::ostream& out, LogMsg& msg) override{
            out<<msg._file;
        }
    };
    class lineFormatItem : public FormatItem{
        public:
        virtual void format(std::ostream& out, LogMsg& msg) override{
            out<<std::to_string(msg._line);
        }
    };
    class tidFormatItem : public FormatItem{
        public:
        virtual void format(std::ostream& out, LogMsg& msg) override{
            out<<msg._tid;
        }
    };
    class loggerFormatItem : public FormatItem{
        public:
        virtual void format(std::ostream& out, LogMsg& msg) override{
            out<<msg._logger;
        }
    };
    class TabFormatItem  : public FormatItem{
        public:
        virtual void format(std::ostream& out, LogMsg& msg) override{
            out<<'\t';
        }
    };
    class NewLineFormatItem  : public FormatItem{
        public:
        virtual void format(std::ostream& out, LogMsg& msg) override{
            out<<'\n';
        }
    };
    //[]
    class OtherFormatItem  : public FormatItem{
        public:
        OtherFormatItem(const std::string& str)
            :_str(str)
        {}
        virtual void format(std::ostream& out, LogMsg& msg) override{
            out<<_str;
        }
        private:
            std::string _str;
    };
    /* 
        %d  表示日期，    子格式 {%H:%M:%S} 
        %t  表示鲜橙ID 
        %c  表示日志器名称
        %f  表示源码文件名
        %l  表示源码行号
        %p  表示日志级别
        %T  表示制表符缩进
        %m  表示主体消息
        %n  表示换行
    */
    class Formatter{
        public:
            Formatter(const std::string& pattern = "[%d{%H:%M:%S}][%t][%c][%f:%l][%p]%T%m%n")
                :_pattern(pattern)
            {
                assert(parsePattern());
            }
            //对msg格式化
            void format(std::ostream& out, LogMsg& msg){
                for (auto &item : _items) {
                    item->format(out, msg);
                }
            }
            std::string format(LogMsg &msg) {
                std::ostringstream out;
                format(out, msg);
                return out.str();
            }
            //对格式化字符串进行解析
            bool parsePattern(){
                //1.对格式化字符串解析
                std::vector<std::pair<std::string, std::string>> fmt_order;
                size_t pos = 0;
                std::string key, val;
                while(pos<_pattern.size()){
                    //处理原始字符abc[]
                    if (_pattern[pos] != '%') {
                        val.push_back(_pattern[pos++]); continue;
                    }
                    //处理%%
                    if (pos + 1 < _pattern.size() && _pattern[pos + 1] == '%') {
                        val.push_back('%'); pos += 2; continue;
                    }
                    //原始字符串处理完毕
                    fmt_order.push_back(std::make_pair("", val));
                    val.clear();
                    //处理格式化字符{}
                    pos += 1;
                    if (pos == _pattern.size()) { 
                        std::cout << "%之后没有对应的格式化字符" << std::endl;
                        return false; 
                    }
                    key = _pattern[pos];
                    pos += 1;
                    if (pos < _pattern.size() && _pattern[pos] == '{') {
                        pos += 1;   
                        // 这时pos指向子规则的起始位置
                        while (pos < _pattern.size() && _pattern[pos] != '}') {
                            val.push_back(_pattern[pos++]);
                        }
                        // 若走到了末尾，还没有找到},则说明格式是错误的，跳出循环
                        if (pos == _pattern.size()) {   
                            std::cout << "子规则{}匹配出错" << std::endl;
                            return false;
                        }
                        pos += 1; // 因为pos指向的是 } 位置，向后走一步就到了下一次处理的新位置
                    }
                    fmt_order.push_back(std::make_pair(key, val));
                    key.clear(); val.clear();
                }
                //2.根据解析后的数据初始化格式化子项数组成员
                for (auto &it : fmt_order) {
                    _items.push_back(createItem(it.first, it.second));
                } 
                return true;
            }
        private:
            //根据不同的格式化字符创建不同的格式化子项对象
            std::shared_ptr<FormatItem> createItem(const std::string &key, const std::string &val){
                if (key == "d")  return std::make_shared<ctimeFormatItem>(val);
                if (key == "t")  return std::make_shared<tidFormatItem>();
                if (key == "c")  return std::make_shared<loggerFormatItem>();
                if (key == "f")  return std::make_shared<fileFormatItem>();
                if (key == "l")  return std::make_shared<lineFormatItem>();
                if (key == "p")  return std::make_shared<levelFormatItem>();
                if (key == "T")  return std::make_shared<TabFormatItem>();
                if (key == "m")  return std::make_shared<payloadFormatItem>();
                if (key == "n")  return std::make_shared<NewLineFormatItem>();
                return std::make_shared<OtherFormatItem>(val);
            }
        private:
            std::string _pattern;//格式化规则字符串
            std::vector<std::shared_ptr<FormatItem>> _items;// 格式化子项数组
    };

}
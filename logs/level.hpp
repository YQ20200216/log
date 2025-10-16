//level.hpp
#pragma once
#include <string>
namespace MySpace
{
    class LogLevel
    {
    public:
        enum value { DEBUG, INFO, WARN, ERROR, FATAL, OFF };

        static const std::string toString(value level){
            switch (level){
                case DEBUG: return "DEBUG";
                case INFO : return "INFO";
                case WARN : return "WARN";
                case ERROR: return "ERROR";
                case FATAL: return "FATAL";
                case OFF  : return "OFF";
            }
            return "UNKNOW";
        }
    };
}

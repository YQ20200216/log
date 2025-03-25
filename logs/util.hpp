//util.hpp
#ifndef UTIL_H
#define UTIL_H
#include <iostream>
#include <ctime>
#include <string>
#include <unistd.h>
#include <sys/stat.h>


// 这个文件中包含一些通用工具
namespace MySpace
{
    class util
    {
    public:
        // 1、获取当前时间戳
        static size_t getCurTime()
        {
            return (size_t)time(nullptr);
        }
        // 2、获取文件目录
        static std::string getDirectory(const std::string& pathname)
        {
            int pos = pathname.find_last_of("/\\");//查找斜杠或者反斜杠
            if (pos == std::string::npos)//没找到
                return std::string("./");
            return pathname.substr(0, pos + 1);
        }
        // 3、判断文件是否存在
        static bool isExist(const std::string& pathname)
        {
            struct stat st;
            return (stat(pathname.c_str(), &st) == 0);
            //return (access(pathname.c_str(), F_OK) == 0);//上面接口更广泛
        }
        // 4、创建一个目录
        static void createDirectory(const std::string& pathname){
            // ./abc/a
            size_t pos = 0, idx = 0;//pos:用于记录路径中分隔符（/ 或 \）位置，idx：作为循环的索引
            while (idx < pathname.size()) {
                pos = pathname.find_first_of("/\\", idx);//从索引 idx 开始，在路径中查找第一个出现的分隔符
                if (pos == std::string::npos) {
                    mkdir(pathname.c_str(), 0777);
                    break;  // 找到末尾，退出循环
                }
                //截取从路径开头到分隔符位置（包含分隔符）的子字符串
                std::string parent_dir = pathname.substr(0 , pos + 1);
                if (!isExist(parent_dir.c_str())) {
                    mkdir(parent_dir.c_str() , 0777);
                } 
                idx = pos + 1;
            }
        }
    };
}
#endif

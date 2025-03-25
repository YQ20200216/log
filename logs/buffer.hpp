//buffer.hpp
#pragma once
#include "level.hpp"
#include <vector>
#include <iostream>
#include <assert.h>

#define DEFAULT_BUFFER_SIZE (1 * 1024 * 1024)//1M大小
#define THRESHOLD_BUFFER_SIZE (8 * 1024 * 1024)//8M大小
#define INCREMENT_BUFFER_SIZE (1 * 1024 * 1024)//1M大小

namespace MySpace{

    class Buffer{
        public:
            Buffer() 
                : _buffer(DEFAULT_BUFFER_SIZE)
                , _write_idx(0)
                , _read_idx(0) 
            {}
            // 向缓冲区写入数据
            void push(const char* data, size_t len){
                // 缓冲区剩余空间不够的情况： 
                // if (len > writeAbleSize()) return; //情况一满了返回
                ensureEnoughSize(len); //情况二扩容
                // 1、将数据拷贝进缓冲区
                std::copy(data, data + len, &_buffer[_write_idx]);
                // 2、将当前写入数据向后偏移
                moveWriter(len);
            }
            // 返回可读数据的起始地址
            const char* begin() { return &_buffer[_read_idx]; }
            // 返回可读数据的长度
            size_t readAbleSize() { return _write_idx-_read_idx; }
            // 返回可写空间的长度
            size_t writeAbleSize() { return _buffer.size()-_write_idx; }
            // 对读写指针进行向后偏移操作
            void moveWriter(size_t len) { assert(len+_write_idx <= writeAbleSize()); _write_idx += len; }
            // 对读写指针进行向后偏移操作
            void moveReader(size_t len) { assert(len <= readAbleSize()); _read_idx += len; }
            // 重制读写位置，初始化缓冲区
            void bufferReset() { _read_idx = 0; _write_idx = 0; }
            // 对buffer实现交换的操作
            void bufferSwap(Buffer &buffer){
                _buffer.swap(buffer._buffer);
                std::swap(_read_idx, buffer._read_idx);
                std::swap(_write_idx, buffer._write_idx);
            }
            // 判断缓冲区是否为空
            bool bufferEmpty() { return _read_idx == _write_idx; }
            // 对空间进行扩容操作
            void ensureEnoughSize(size_t len){
                if (len <= writeAbleSize()) return;
                size_t new_size = 0;
                while (writeAbleSize() < len) {
                    if (_buffer.size() < THRESHOLD_BUFFER_SIZE) {
                        new_size = _buffer.size() * 2; // 小于阈值翻倍增长
                    } else {
                        new_size = _buffer.size() + INCREMENT_BUFFER_SIZE; // 大于阈值线性增长
                    }
                    _buffer.resize(new_size);
                }
            }
        private:
            std::vector<char> _buffer;  // 存放字符串数据缓冲区
            size_t _read_idx;           // 当前可读数据的指针
            size_t _write_idx;          // 当前可写数据的指针
        };
}
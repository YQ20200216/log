//looper.hpp
#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <memory>
#include <atomic>
#include <functional>
#include <condition_variable> 
#include "buffer.hpp"
#include "format.hpp"
#include "level.hpp"
#include "message.hpp"
#include "sink.hpp"
#include "util.hpp"


namespace MySpace{
  class AsynchLooper {
    public:
      AsynchLooper(const std::function<void(Buffer &)> &cb) 
        :_stop(false)
        , _callBack(cb)
        , _thread(std::thread(&AsynchLooper::threadEntry, this))//传入 this 指针，以便在线程中访问成员
    {}
      ~AsynchLooper(){
        _stop = true;                    // 退出标志设置为true 
        _consumer_cond.notify_all();     // 唤醒所有工作线程
        _thread.join();                  // 等待工作线程退出
      }
      //生产
      void push(const char *data, size_t len) {
        std::unique_lock<std::mutex> lock(_mutex);
        //缓冲区满了就阻塞
        _produce_cond.wait(lock, [&](){ return _produce_buffer.writeAbleSize() >= len; });
        //向缓冲区添加数据
        _produce_buffer.push(data, len);
        //唤醒消费者对缓冲区中的数据进行处理
        _consumer_cond.notify_one();   
      }
      //消费
      void threadEntry() {
        while (1) {
            //互斥锁设置生命周期，交换完后解锁，不对数据过程加锁
            {
                // 1、 判断生产缓冲区有没有数据，有则交换，无则阻塞
                std::unique_lock<std::mutex> lock(_mutex);
                //lambda返回true，wait结束等待，返回false，释放锁并阻塞等待直到被唤醒再次判断lambda返回值
                _consumer_cond.wait(lock, [&](){ return ( _stop || !_produce_buffer.bufferEmpty()); });
                //再次检查,防止有数据了，!_produce_buffer.bufferEmpty() 为真，或者要退出了，_stop 为真
                if (_stop && _produce_buffer.bufferEmpty()) {
                    break;
                }
                _produce_buffer.bufferSwap(_consumer_buffer);
                // 2、 唤醒生产者(只有安全状态生产者才会被阻塞)
                _produce_cond.notify_all();
            }
            // 3、 被唤醒后，对消费缓冲区进行数据处理(处理过程无需加锁保护)
            _callBack(_consumer_buffer);
            // 4、 初始化消费缓冲区
            _consumer_buffer.bufferReset();
        }
      }  

    private:
      //工作流程，主线程写到生产缓冲区（要加锁），工作线程空闲时，交换两个缓冲区，工作线程读（不用加锁）
      std::atomic<bool> _stop;                  // 工作器停止标志，不加锁情况下可以被多个线程访问
      std::mutex _mutex;          
      Buffer _produce_buffer;                   // 生产缓冲区
      Buffer _consumer_buffer;                  // 消费缓冲区
      std::condition_variable _produce_cond;    // 生产条件变量，生产缓冲区满时，阻塞主线程
      std::condition_variable _consumer_cond;   // 消费条件变量，消费缓冲区空时，阻塞工作线程
      std::thread _thread;        
      std::function<void(Buffer &)> _callBack;  //回调函数 具体对缓冲区数据进行处理的回调函数， 由异步工作器的使用者传入
  };
}

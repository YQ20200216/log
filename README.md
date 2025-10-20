# C++ 轻量级日志系统

## 📋 项目概述

这是一个基于 C++11 实现的轻量级、高性能日志系统，采用**木兰宽松许可证 v2（Mulan PSL v2）**开源。该项目采用纯头文件（Header-Only）方式实现，无需编译库文件，只需包含头文件即可使用。

## ✨ 核心特性

- **🚀 高性能**：支持同步和异步两种模式，异步模式采用双缓冲区设计，大幅提升吞吐量
- **🧵 线程安全**：使用互斥锁和条件变量保护共享资源，支持多线程并发日志记录
- **📦 模块化设计**：缓冲区、格式化器、落地模块、日志器等组件解耦，易于扩展
- **🎯 灵活配置**：支持自定义日志格式、日志等级、多种输出方式
- **💡 易于使用**：提供宏接口自动填充文件名和行号，简化日志记录
- **📁 多种输出**：支持控制台输出、文件输出、按大小滚动的文件输出
- **🔧 Header-Only**：纯头文件实现，无需预编译，集成简单

## 🏗️ 架构设计

### 核心组件

| 组件 | 文件 | 功能说明 |
|------|------|---------|
| **Buffer** | `buffer.hpp` | 双指针循环缓冲区，支持动态扩容和缓冲区交换 |
| **LogLevel** | `level.hpp` | 定义日志等级（DEBUG/INFO/WARN/ERROR/FATAL/OFF） |
| **LogMsg** | `message.hpp` | 封装日志消息对象（时间戳、等级、文件、行号等） |
| **Formatter** | `format.hpp` | 格式化器，支持自定义日志输出格式 |
| **Sink** | `sink.hpp` | 日志落地模块（标准输出、文件、滚动文件） |
| **AsynchLooper** | `looper.hpp` | 异步工作器，双缓冲区+独立线程处理日志 |
| **Logger** | `logger.hpp` | 日志器（同步/异步），日志器管理器 |
| **Util** | `util.hpp` | 工具类（时间、文件路径、目录创建） |
| **mylog** | `mylog.hpp` | 便捷接口和宏定义 |

### 格式化模式

支持以下格式化标记：

- `%d{时间格式}` - 日期时间，例如 `%d{%H:%M:%S}` 显示为 `14:30:25`
- `%t` - 线程ID
- `%c` - 日志器名称
- `%f` - 源文件名
- `%l` - 源码行号
- `%p` - 日志级别
- `%T` - 制表符
- `%m` - 日志消息内容
- `%n` - 换行符

**默认格式**：`[%d{%H:%M:%S}][%t][%c][%f:%l][%p]%T%m%n`

**示例输出**：
```
[14:30:25][140234567][sync_logger][main.cpp:42][INFO]	User login successful
```

## 📁 项目结构

```
log/
├── logs/                    # 核心库代码（纯头文件）
│   ├── buffer.hpp           # 缓冲区实现
│   ├── level.hpp            # 日志等级定义
│   ├── message.hpp          # 日志消息封装
│   ├── format.hpp           # 格式化器
│   ├── sink.hpp             # 落地模块（控制台/文件/滚动文件）
│   ├── looper.hpp           # 异步工作器
│   ├── logger.hpp           # 日志器核心实现
│   ├── util.hpp             # 工具函数
│   └── mylog.hpp            # 便捷接口（推荐使用）
├── bench/                   # 性能测试
│   ├── bench.cpp            # 测试程序
│   └── Makefile             # 编译脚本
├── LICENSE                  # 木兰宽松许可证 v2
└── README.md                # 本文档
```

## 📚 学习路径（从零开始）

如果您是第一次接触本项目，建议按照以下顺序学习源码，循序渐进地理解整个日志系统的设计。

### 第一阶段：基础组件（2-3小时）

#### 1️⃣ level.hpp - 日志等级
- **难度**：⭐ 
- **学习时间**：5-10分钟
- **为什么从这里开始**：代码最简单（仅32行），没有依赖其他模块，定义了核心概念
- **重点关注**：
  - `enum value` 枚举类型的定义
  - `toString()` 静态方法的实现

#### 2️⃣ util.hpp - 工具函数
- **难度**：⭐⭐
- **学习时间**：10-15分钟
- **重点关注**：
  - `getCurTime()` - 获取时间戳
  - `getDirectory()` - 路径解析
  - `createDirectory()` - 递归创建目录
  - `isExist()` - 文件存在性检查

#### 3️⃣ message.hpp - 日志消息封装
- **难度**：⭐⭐
- **学习时间**：10-15分钟
- **依赖**：level.hpp, util.hpp
- **重点关注**：
  - `LogMsg` 类包含哪些字段（时间戳、等级、文件名、行号、线程ID等）
  - 构造函数如何初始化各个字段

#### 4️⃣ buffer.hpp - 缓冲区 ⭐重要
- **难度**：⭐⭐⭐
- **学习时间**：20-30分钟
- **重点关注**：
  - 双指针设计（`_read_idx` 和 `_write_idx`）
  - `push()` - 如何写入数据
  - `ensureEnoughSize()` - 动态扩容策略（小于8M翻倍，大于8M线性增长）
  - `bufferSwap()` - 缓冲区交换（异步日志的关键）
- **学习建议**：画图理解读写指针的移动过程

#### 5️⃣ format.hpp - 格式化器
- **难度**：⭐⭐⭐⭐
- **学习时间**：30-40分钟
- **依赖**：level.hpp, util.hpp, message.hpp
- **重点关注**：
  - 各种 `FormatItem` 子类的实现（如 `timeFormatItem`、`levelFormatItem` 等）
  - `parsePattern()` - 如何解析格式化字符串（如 `%d{%H:%M:%S}`）
  - `format()` - 如何将 LogMsg 格式化成字符串
- **学习建议**：
  1. 先看各个 FormatItem 子类（简单）
  2. 再看 parsePattern() 的解析逻辑（复杂但很重要）
  3. 自己尝试写一个格式化字符串，理解解析过程

#### 6️⃣ sink.hpp - 落地模块
- **难度**：⭐⭐⭐
- **学习时间**：20-30分钟
- **依赖**：level.hpp, util.hpp, message.hpp
- **重点关注**：
  - `LogSink` 抽象基类
  - `StdoutSink` - 控制台输出（最简单）
  - `FileSink` - 文件输出
  - `RollBySizeSink` - 滚动文件（按大小切换）
  - `SinkFactory` - 工厂模式创建 Sink
- **学习建议**：按顺序看三个实现类，理解继承和多态的应用

### 第二阶段：高级特性（3-4小时）

#### 7️⃣ looper.hpp - 异步工作器 ⭐⭐难点
- **难度**：⭐⭐⭐⭐⭐
- **学习时间**：40-60分钟
- **依赖**：buffer.hpp 及其他所有基础组件
- **重点关注**：
  - 双缓冲区设计（`_produce_buffer` 和 `_consumer_buffer`）
  - 生产者-消费者模型
  - `push()` - 生产者写入数据
  - `threadEntry()` - 消费者线程处理数据
  - 条件变量 `_produce_cond` 和 `_consumer_cond` 的使用
  - 线程安全机制
- **学习建议**：
  1. 先复习多线程知识（mutex、condition_variable）
  2. 画图理解双缓冲区的交换过程
  3. 理解为什么需要双缓冲区（减少锁竞争，提高性能）

#### 8️⃣ logger.hpp - 日志器核心 ⭐⭐最复杂
- **难度**：⭐⭐⭐⭐⭐
- **学习时间**：60-90分钟
- **依赖**：所有前面的模块
- **分步学习**：
  1. **Logger 基类**（20分钟）
     - `logMessage()` - 日志记录的核心流程
     - 纯虚函数 `log()` - 抽象的落地接口
  2. **SynchLogger 同步日志器**（10分钟）
     - 直接调用 sink 进行落地，简单直接
  3. **AsynchLogger 异步日志器**（20分钟）
     - 使用 AsynchLooper 进行异步处理
     - `log()` 写入缓冲区，`realLog()` 实际落地
  4. **LoggerBuilder 建造者模式**（15分钟）
     - 如何构建日志器，各种 `build` 方法的作用
  5. **LoggerManager 单例管理器**（15分钟）
     - 懒汉单例模式，日志器的注册和获取
     - 默认 root 日志器
  6. **GlobalLoggerBuilder**（10分钟）
     - 自动注册到管理器

#### 9️⃣ mylog.hpp - 便捷接口
- **难度**：⭐⭐
- **学习时间**：10-15分钟
- **依赖**：logger.hpp
- **重点关注**：
  - 宏定义如何简化使用
  - `##__VA_ARGS__` 的作用
  - `__FILE__` 和 `__LINE__` 的自动填充

### 第三阶段：实践应用（2-3小时）

#### 🔟 bench/bench.cpp - 性能测试
- **难度**：⭐⭐⭐
- **学习时间**：30-40分钟
- **重点关注**：
  - 如何创建和配置日志器
  - 同步和异步的性能对比
  - 多线程测试场景
  - 性能统计方法
- **学习建议**：
  1. 先读代码理解测试逻辑
  2. 编译运行看实际效果
  3. 修改参数做不同场景的测试

### 学习时间规划

| 阶段 | 内容 | 预计时间 |
|------|------|---------|
| **第1天** | 基础组件（level → util → message → buffer → format → sink） | 2-3小时 |
| **第2天** | 高级特性（looper → logger → mylog） | 3-4小时 |
| **第3天** | 实践应用（bench + 自己动手写示例） | 2-3小时 |

### 💡 学习建议

1. **边看边做笔记**：记录每个类的作用和关键方法
2. **画图理解**：特别是 buffer 和 looper 的交互流程
3. **调试运行**：在关键位置加断点，观察数据流动
4. **动手修改**：尝试修改参数、添加新功能
5. **参考注释**：代码中的注释很详细，仔细阅读
6. **循序渐进**：不要跳过基础组件直接看复杂模块

### 🎯 学习检验

学完后，尝试回答以下问题来检验学习效果：

1. 日志系统的核心流程是什么？（从调用 logger->info() 到日志落地）
2. 同步日志器和异步日志器的区别是什么？各适用什么场景？
3. 双缓冲区是如何工作的？为什么能提高性能？
4. 格式化字符串 `[%d{%H:%M:%S}][%p] %m%n` 是如何被解析的？
5. 如何扩展一个新的 Sink（比如输出到网络）？
6. LoggerManager 为什么使用单例模式？

## 🚀 快速开始

### 1. 环境要求

- **编译器**：支持 C++11 的编译器（GCC 4.8+、Clang 3.4+、MSVC 2015+）
- **操作系统**：Linux、macOS、Windows
- **依赖库**：pthread（Linux/macOS）

### 2. 获取源码

```bash
git clone <仓库地址>
cd log
```

### 3. 基础使用示例

创建一个简单的测试文件 `test.cpp`：

```cpp
#include "logs/mylog.hpp"

int main() {
    // 方式1：使用默认的 root 日志器（标准输出）
    MySpace::DEBUG("程序启动");
    MySpace::INFO("这是一条信息日志");
    MySpace::WARN("这是一条警告日志，值=%d", 42);
    MySpace::ERROR("这是一条错误日志");
    
    // 方式2：创建自定义同步日志器
    std::shared_ptr<MySpace::LoggerBuilder> builder(new MySpace::GlobalLoggerBuilder());
    builder->buildLoggerName("my_logger");
    builder->buildLoggerLevel(MySpace::LogLevel::DEBUG);
    builder->buildLoggerFormatter("[%d{%H:%M:%S}][%p] %m%n");
    builder->buildLoggerType(MySpace::LoggerType::LOGGER_SYNCH);
    builder->buildSink<MySpace::StdoutSink>();  // 标准输出
    builder->buildSink<MySpace::FileSink>("./logs/app.log");  // 文件输出
    builder->build();
    
    // 使用自定义日志器
    auto logger = MySpace::getLogger("my_logger");
    logger->debug("调试信息：value=%d", 100);
    logger->info("普通信息");
    logger->error("错误信息");
    
    return 0;
}
```

### 4. 编译运行

```bash
# 编译（需要链接 pthread）
g++ -std=c++11 -o test test.cpp -pthread -I./

# 运行
./test
```

## 📝 详细使用指南

### 创建同步日志器

同步日志器会立即将日志写入目标（控制台/文件），适合对实时性要求高的场景：

```cpp
std::shared_ptr<MySpace::LoggerBuilder> builder(new MySpace::GlobalLoggerBuilder());
builder->buildLoggerName("sync_logger");
builder->buildLoggerLevel(MySpace::LogLevel::INFO);  // 只记录INFO及以上级别
builder->buildLoggerFormatter("[%d{%Y-%m-%d %H:%M:%S}][%p] %m%n");
builder->buildLoggerType(MySpace::LoggerType::LOGGER_SYNCH);  // 同步模式
builder->buildSink<MySpace::FileSink>("./logs/app.log");
builder->build();
```

### 创建异步日志器

异步日志器使用双缓冲区和独立线程处理日志，适合高性能场景：

```cpp
std::shared_ptr<MySpace::LoggerBuilder> builder(new MySpace::GlobalLoggerBuilder());
builder->buildLoggerName("async_logger");
builder->buildLoggerLevel(MySpace::LogLevel::DEBUG);
builder->buildLoggerFormatter("%m%n");
builder->buildLoggerType(MySpace::LoggerType::LOGGER_ASYNCH);  // 异步模式
builder->buildSink<MySpace::FileSink>("./logs/async.log");
builder->buildSink<MySpace::RollBySizeSink>("./logs/roll", 1024 * 1024);  // 1MB滚动
builder->build();
```

### 使用滚动文件

当日志文件超过指定大小时，自动创建新文件：

```cpp
builder->buildSink<MySpace::RollBySizeSink>(
    "./logs/app",           // 文件基础名
    10 * 1024 * 1024       // 10MB 触发滚动
);
// 生成文件示例：./logs/app2025-10-16 14:30:25-1.log
```

### 使用宏接口（推荐）

宏会自动填充文件名和行号信息：

```cpp
auto logger = MySpace::getLogger("my_logger");
logger->debug("用户ID=%d 登录成功", userId);      // 自动添加 __FILE__ 和 __LINE__
logger->info("请求处理完成，耗时=%dms", elapsed);
logger->error("数据库连接失败：%s", error.c_str());

// 或使用全局默认日志器
MySpace::DEBUG("调试信息");
MySpace::INFO("普通信息");
MySpace::WARN("警告信息");
MySpace::ERROR("错误信息");
MySpace::FATAL("致命错误");
```

## 🔄 代码执行流程详解

本章节从代码层面深入剖析不同场景下的完整执行流程，帮助您理解日志系统的内部运作机制。

> **注意**：本项目已简化设计，移除了复杂的建造者模式和单例模式，改用简单的工厂函数。

### 场景1：创建同步日志器的流程

#### 代码示例
```cpp
auto logger = LoggerFactory::createSynchLogger("mylogger", LogLevel::INFO);
```

#### 执行流程图
```
用户调用
    ↓
1. LoggerFactory::createSynchLogger() (logger.hpp:127-141)
    ├─ pattern.empty()? → 创建默认 Formatter
    │   └─ Formatter() 构造函数 (format.hpp:115)
    │       └─ parsePattern() 解析格式化字符串 "[%d{%H:%M:%S}][%t][%c][%f:%l][%p]%T%m%n"
    │           └─ 生成 FormatItem 数组 (_items)
    │               ├─ OtherFormatItem("") → "["
    │               ├─ ctimeFormatItem("%H:%M:%S") → 时间
    │               ├─ OtherFormatItem("][")
    │               ├─ tidFormatItem() → 线程ID
    │               └─ ... 其他格式项
    │
    ├─ 创建 StdoutSink (sink.hpp:17-25)
    │   └─ 添加到 sinks 向量
    │
    └─ 创建 SynchLogger (logger.hpp:76-83)
        └─ 调用父类 Logger 构造函数 (logger.hpp:21-29)
            ├─ 保存 _logger_name = "mylogger"
            ├─ 保存 _limit_level = INFO
            ├─ 保存 _formatter (shared_ptr)
            └─ 保存 _sinks 向量

返回 shared_ptr<Logger>
```

---

### 场景2：同步日志记录的完整流程

#### 代码示例
```cpp
logger->info(__FILE__, __LINE__, "用户登录成功");
```

#### 详细执行流程
```
主线程调用 logger->info()
    ↓
1. Logger::info() (logger.hpp:36-38)
   └─ 调用 logMessage(LogLevel::INFO, __FILE__, __LINE__, "用户登录成功")
       ↓
2. Logger::logMessage() (logger.hpp:49-60)
   │
   ├─ 【步骤1】日志级别过滤 (line 52-53)
   │   if (INFO < INFO) → false，继续执行
   │   └─ 这里可以快速过滤掉低优先级日志，避免不必要的开销
   │
   ├─ 【步骤2】构造 LogMsg 对象 (line 55)
   │   └─ LogMsg 构造函数 (message.hpp:23-35)
   │       ├─ _level = INFO
   │       ├─ _ctime = util::getCurTime()           → 1734700800
   │       ├─ _line = 42
   │       ├─ _file = "main.cpp"
   │       ├─ _logger = "mylogger"
   │       ├─ _payload = "用户登录成功"
   │       └─ _tid = std::this_thread::get_id()     → 140234567890
   │
   ├─ 【步骤3】格式化日志消息 (line 57)
   │   └─ _formatter->format(msg) (format.hpp:127-131)
   │       └─ 创建 ostringstream，遍历 _items 数组
   │           ├─ OtherFormatItem::format()         → 输出 "["
   │           ├─ ctimeFormatItem::format()         → 输出 "12:30:45"
   │           │   ├─ localtime_r(&msg._ctime, &t)  → 转换时间戳
   │           │   └─ strftime(buff, "%H:%M:%S")    → 格式化时间
   │           ├─ OtherFormatItem::format()         → 输出 "]["
   │           ├─ tidFormatItem::format()           → 输出 "140234567890"
   │           ├─ OtherFormatItem::format()         → 输出 "]["
   │           ├─ loggerFormatItem::format()        → 输出 "mylogger"
   │           ├─ OtherFormatItem::format()         → 输出 "]["
   │           ├─ fileFormatItem::format()          → 输出 "main.cpp"
   │           ├─ OtherFormatItem::format()         → 输出 ":"
   │           ├─ lineFormatItem::format()          → 输出 "42"
   │           ├─ OtherFormatItem::format()         → 输出 "]["
   │           ├─ levelFormatItem::format()         → 输出 "INFO"
   │           ├─ OtherFormatItem::format()         → 输出 "]"
   │           ├─ TabFormatItem::format()           → 输出 "\t"
   │           ├─ payloadFormatItem::format()       → 输出 "用户登录成功"
   │           └─ NewLineFormatItem::format()       → 输出 "\n"
   │       └─ 返回完整字符串: "[12:30:45][140234567890][mylogger][main.cpp:42][INFO]\t用户登录成功\n"
   │
   └─ 【步骤4】日志落地 (line 59)
       └─ log(real_message.c_str(), real_message.size())
           ↓
3. SynchLogger::log() (logger.hpp:86-92)
   │
   ├─ 【加锁保护】(line 87)
   │   std::unique_lock<std::mutex> lock(_mutex)
   │   └─ 确保多线程环境下的线程安全
   │
   ├─ 【检查sinks】(line 88)
   │   if (_sinks.empty()) return;
   │
   └─ 【遍历所有sink】(line 89-91)
       └─ for (auto &sink : _sinks)
           └─ sink->log(data, len)
               ↓
4. StdoutSink::log() (sink.hpp:20-24)
   ├─ 构造 string 对象: std::string str(data, 0, len)
   └─ 输出到控制台: std::cout << str << std::endl
       └─ 日志输出到标准输出 ✓

释放锁，函数返回
```

#### 输出示例
```
[12:30:45][140234567890][mylogger][main.cpp:42][INFO]	用户登录成功
```

#### 性能分析
```
操作                    耗时        说明
───────────────────────────────────────────────
等级过滤               ~1ns        快速返回
构造 LogMsg           ~50ns       创建对象
格式化日志消息         ~500ns      遍历格式化项
加锁                   ~20ns       获取互斥锁
输出到控制台          ~10μs       I/O 操作
解锁                   ~20ns       释放互斥锁
───────────────────────────────────────────────
总计                  ~11μs       主线程阻塞
```

---

### 场景3：异步日志记录的完整流程

#### 代码示例
```cpp
auto async_logger = LoggerFactory::createAsynchLogger("async", LogLevel::DEBUG);
async_logger->info(__FILE__, __LINE__, "异步日志消息");
```

#### 创建阶段流程
```
1. LoggerFactory::createAsynchLogger() (logger.hpp:179-194)
   └─ 创建 AsynchLogger (logger.hpp:97-103)
       ├─ 调用父类 Logger 构造函数
       └─ 创建 AsynchLooper (line 102)
           └─ AsynchLooper 构造函数 (looper.hpp:22-26)
               ├─ _stop = false
               ├─ _callBack = lambda [this](Buffer &buf) { realLog(buf); }
               ├─ 初始化两个缓冲区
               │   ├─ _produce_buffer (生产者写入)
               │   └─ _consumer_buffer (消费者读取)
               └─ 启动后台线程 (line 25)
                   std::thread(&AsynchLooper::threadEntry, this)
                   └─ 工作线程开始运行，等待数据...
```

#### 记录日志阶段（主线程）
```
主线程调用 async_logger->info()
    ↓
1. Logger::info() → Logger::logMessage()
   └─ 与同步日志相同的步骤1-3
       └─ 生成格式化后的日志字符串
           └─ "[12:30:45][140234567890][async][main.cpp:15][INFO]\t异步日志消息\n"
   ↓
2. AsynchLogger::log() (logger.hpp:106-108)
   └─ _looper->push(data.c_str(), len)
       ↓
3. AsynchLooper::push() (looper.hpp:33-41)
   │
   ├─ 【加锁】(line 34)
   │   std::unique_lock<std::mutex> lock(_mutex)
   │
   ├─ 【等待空间】(line 36)
   │   _produce_cond.wait(lock, [&]{ 
   │       return _produce_buffer.writeAbleSize() >= len; 
   │   })
   │   └─ 如果缓冲区满了，阻塞等待（通常不会发生）
   │
   ├─ 【写入缓冲区】(line 38)
   │   _produce_buffer.push(data, len)
   │   │
   │   └─ Buffer::push() (buffer.hpp:22-28)
   │       ├─ ensureEnoughSize(len)              → 检查并扩容
   │       ├─ std::copy(data, data+len, &_buffer[_write_idx])
   │       │   └─ 内存拷贝，速度极快！
   │       └─ _write_idx += len                  → 移动写指针
   │
   └─ 【唤醒消费者】(line 40)
       _consumer_cond.notify_one()
       └─ 通知工作线程处理数据

✅ 主线程快速返回（~1-5μs），继续执行业务逻辑
```

#### 记录日志阶段（工作线程）
```
工作线程并行运行
    ↓
1. AsynchLooper::threadEntry() (looper.hpp:43-64)
   │
   └─ while (1) 无限循环
       ↓
       
2. 【等待数据】(line 48-50)
   ├─ std::unique_lock<std::mutex> lock(_mutex)
   ├─ _consumer_cond.wait(lock, [&]{ 
   │       return (_stop || !_produce_buffer.bufferEmpty()); 
   │   })
   │   └─ 线程休眠，等待被唤醒...
   │
   └─ 被 notify_one() 唤醒！继续执行
       ↓
       
3. 【检查退出条件】(line 52-54)
   if (_stop && _produce_buffer.bufferEmpty()) break;
   └─ 如果要停止且缓冲区空了，退出循环
   ↓
   
4. 【交换缓冲区】(line 55)
   _produce_buffer.bufferSwap(_consumer_buffer)
   │
   └─ Buffer::bufferSwap() (buffer.hpp:44-47)
       │
       │  交换前:
       │  _produce_buffer: [日志数据 AAAA...] ← 主线程写入
       │  _consumer_buffer: [空]
       │
       │  交换后 (极快，~100ns):
       │  _produce_buffer: [空]               ← 主线程可以继续写入
       │  _consumer_buffer: [日志数据 AAAA...] ← 工作线程慢慢处理
       │
       └─ 实现原理: std::swap(_buffer, other._buffer)
          只交换指针，不拷贝数据！
   ↓
   
5. 【唤醒生产者】(line 57)
   _produce_cond.notify_all()
   └─ 通知主线程可以继续写入
   └─ 释放锁，退出临界区
   ↓
   
6. 【处理数据】(line 60) - 无锁操作！
   _callBack(_consumer_buffer)
   └─ 调用 lambda: [this](Buffer &buf) { realLog(buf); }
       ↓
       AsynchLogger::realLog() (logger.hpp:111-116)
       └─ 遍历所有 sink
           └─ for (auto &sink : _sinks)
               └─ sink->log(buf.begin(), buf.readAbleSize())
                   ↓
                   FileSink::log() (sink.hpp:36-41)
                   └─ _ofs.write(data, len)
                       └─ 写入磁盘文件 ✓
                           （耗时操作，但不阻塞主线程！）
   ↓
   
7. 【重置缓冲区】(line 62)
   _consumer_buffer.bufferReset()
   └─ _read_idx = _write_idx = 0
   └─ 继续循环，等待下一批数据...
```

#### 性能对比
```
操作              同步日志      异步日志（主线程）    说明
──────────────────────────────────────────────────────────
写入缓冲区        -            ~1-5μs              仅内存操作
写入文件          ~100μs       -                   I/O 阻塞主线程
主线程耗时        ~110μs       ~5μs                异步快 22 倍！
吞吐量            ~9k条/秒     ~200k条/秒          异步高 22 倍！
```

**关键优势**：
- ✅ 主线程只写缓冲区，立即返回，不阻塞业务逻辑
- ✅ 工作线程异步处理I/O，充分利用CPU
- ✅ 双缓冲机制，减少锁竞争，提高并发性能

---

### 场景4：多Sink输出流程

#### 代码示例
```cpp
std::vector<std::shared_ptr<LogSink>> sinks = {
    std::make_shared<StdoutSink>(),
    std::make_shared<FileSink>("app.log"),
    std::make_shared<RollBySizeSink>("roll-", 1024*1024)
};
auto logger = LoggerFactory::createSynchLogger("multi", LogLevel::INFO, "", sinks);
logger->info(__FILE__, __LINE__, "多目标输出");
```

#### 执行流程
```
SynchLogger::log() (logger.hpp:86-92)
    ↓
遍历 _sinks 向量 (line 89-91)
    │
    ├─ sink[0]->log(data, len)
    │   └─ StdoutSink::log() (sink.hpp:20-24)
    │       └─ std::cout << 日志 << std::endl
    │           ✅ 输出到控制台
    │
    ├─ sink[1]->log(data, len)
    │   └─ FileSink::log() (sink.hpp:36-41)
    │       ├─ _ofs.write(data, len)
    │       │   ✅ 写入 app.log 文件
    │       └─ if (_ofs.fail()) 错误处理
    │
    └─ sink[2]->log(data, len)
        └─ RollBySizeSink::log() (sink.hpp:60-70)
            ├─ 【检查文件大小】(line 61)
            │   if (_cur_fsize + len >= _max_fsize)
            │       ├─ _ofs.close()                    → 关闭当前文件
            │       ├─ createNewFile()                 → 创建新文件名
            │       │   └─ "roll-2024-10-20 12:30:45-1.log"
            │       ├─ util::createDirectory()         → 创建目录
            │       └─ _ofs.open(pathname)             → 打开新文件
            │
            ├─ 【写入数据】(line 68)
            │   _ofs.write(data, len)
            │   ✅ 写入 roll-2024-10-20 12:30:45-1.log
            │
            └─ 【更新计数】(line 69)
                _cur_fsize += len

同一条日志同时输出到3个目标！
```

#### 文件滚动机制
```
当前文件: roll-2024-10-20 12:30:45-1.log (1023KB)
    ↓
写入新日志 (2KB)
    ↓
检查: 1023KB + 2KB >= 1024KB? → true
    ↓
创建新文件: roll-2024-10-20 12:31:10-2.log
    ↓
写入新日志到新文件
    ↓
最终结果:
├─ roll-2024-10-20 12:30:45-1.log (1023KB) ← 已满
└─ roll-2024-10-20 12:31:10-2.log (2KB)    ← 当前
```

---

### 场景5：日志级别过滤流程

#### 代码示例
```cpp
auto logger = LoggerFactory::createSynchLogger("test", LogLevel::WARN);
logger->debug(__FILE__, __LINE__, "调试信息");  // ❌ 不输出
logger->info(__FILE__, __LINE__, "普通信息");   // ❌ 不输出
logger->warn(__FILE__, __LINE__, "警告信息");   // ✅ 输出
logger->error(__FILE__, __LINE__, "错误信息");  // ✅ 输出
```

#### 执行流程
```
每次调用日志方法都会经过 logMessage()
    ↓
Logger::logMessage() (logger.hpp:49-60)
    ↓
【步骤1】级别过滤 (line 52-53)
    if (level < _limit_level) return;

╔═══════════════════════════════════════════╗
║  调用1: debug                             ║
║  ───────────────────                      ║
║  level = DEBUG (0)                        ║
║  _limit_level = WARN (3)                  ║
║  判断: 0 < 3 → true                       ║
║  结果: 直接 return ❌ 不输出               ║
║  说明: 跳过了构造LogMsg、格式化等所有步骤  ║
╚═══════════════════════════════════════════╝

╔═══════════════════════════════════════════╗
║  调用2: info                              ║
║  ───────────────────                      ║
║  level = INFO (1)                         ║
║  _limit_level = WARN (3)                  ║
║  判断: 1 < 3 → true                       ║
║  结果: 直接 return ❌ 不输出               ║
╚═══════════════════════════════════════════╝

╔═══════════════════════════════════════════╗
║  调用3: warn                              ║
║  ───────────────────                      ║
║  level = WARN (3)                         ║
║  _limit_level = WARN (3)                  ║
║  判断: 3 < 3 → false                      ║
║  结果: 继续执行 ✅ 输出                    ║
║  └─→ 构造 LogMsg                          ║
║      └─→ 格式化                           ║
║          └─→ 落地输出                     ║
╚═══════════════════════════════════════════╝

╔═══════════════════════════════════════════╗
║  调用4: error                             ║
║  ───────────────────                      ║
║  level = ERROR (4)                        ║
║  _limit_level = WARN (3)                  ║
║  判断: 4 < 3 → false                      ║
║  结果: 继续执行 ✅ 输出                    ║
╚═══════════════════════════════════════════╝
```

#### 日志级别定义
```cpp
// level.hpp
enum value {
    DEBUG = 0,  // 调试信息
    INFO = 1,   // 普通信息
    WARN = 2,   // 警告信息
    ERROR = 3,  // 错误信息
    FATAL = 4,  // 致命错误
    OFF = 5     // 关闭日志
};
```

---

### 核心流程总结图

```
═══════════════════════════════════════════════
              【创建日志器】
═══════════════════════════════════════════════
用户代码
    ↓
LoggerFactory::create___Logger()
    ├─ 创建 Formatter (解析格式化字符串)
    ├─ 创建 Sink (输出目标)
    └─ 创建 Logger (同步/异步)

═══════════════════════════════════════════════
              【记录日志】
═══════════════════════════════════════════════
logger->info/debug/warn/error/fatal(__FILE__, __LINE__, message)
    ↓
Logger::logMessage()
    ├─ [1] 日志级别过滤       ← 快速过滤
    ├─ [2] 构造 LogMsg 对象   ← 收集元信息
    ├─ [3] Formatter::format() ← 格式化
    │       └─ 遍历 FormatItem 数组生成最终字符串
    └─ [4] log() 落地输出
        ↓
    ┌───┴───┐
    │       │
同步       异步
    │       │
SynchLogger::log()      AsynchLogger::log()
    │                       │
    ├─ 加锁                 ├─ push 到缓冲区
    └─ 遍历 sinks           └─ 唤醒后台线程
        ├─ StdoutSink           └─ 主线程立即返回 ✅
        ├─ FileSink             
        └─ RollSink         后台线程 threadEntry()
                                ├─ 等待数据
                                ├─ 交换缓冲区 (双缓冲)
                                └─ 遍历 sinks 输出
═══════════════════════════════════════════════
```

---

### 关键设计模式

1. **工厂模式**: `LoggerFactory` 提供简单的静态工厂方法创建日志器
2. **策略模式**: 不同的 `Sink` 实现不同的输出策略
3. **模板方法模式**: `Logger` 基类定义流程，子类实现具体落地方式
4. **生产者-消费者模式**: 异步日志的双缓冲机制

---

### 性能优化要点

1. **级别过滤前置**: 在构造LogMsg之前就过滤，避免不必要的开销
2. **双缓冲机制**: 主线程和工作线程各用一个缓冲区，减少锁竞争
3. **缓冲区交换**: 使用 `std::swap` 交换指针，而不拷贝数据
4. **无锁处理**: 工作线程处理缓冲区时不持有锁
5. **智能指针**: 使用 `shared_ptr` 管理资源，避免内存泄漏


## 🧪 性能测试

项目提供了完整的性能测试工具，可以评估同步和异步日志的性能表现。

### 编译并运行测试

```bash
cd bench
make
./bench
```

### 测试代码说明

`bench.cpp` 提供了两种测试场景：

1. **同步日志测试**（`sync_bench()`）
2. **异步日志测试**（`async_bench()`）

可以修改测试参数：

```cpp
// 参数：日志器名称、线程数、日志条数、每条日志大小
bench("async_logger", 10, 1000000, 100);
//     ↑              ↑   ↑         ↑
//     日志器名       线程 总条数    字节数
```

### 性能参考

在典型的测试环境下（具体数据取决于硬件配置）：

- **同步日志器**：适合日志量较小的场景
- **异步日志器**：在高并发场景下性能显著优于同步模式，可达数十万条/秒

## 🔧 集成到现有项目

### 方式1：直接包含头文件

```cpp
// 在你的源文件中
#include "path/to/log/logs/mylog.hpp"

// 编译时添加头文件路径和 pthread
g++ -std=c++11 your_app.cpp -pthread -I/path/to/log
```

### 方式2：子模块方式

```bash
# 将日志系统作为 git 子模块
git submodule add <仓库地址> third_party/log

# 在 CMakeLists.txt 中
include_directories(third_party/log)
target_link_libraries(your_app pthread)
```

### 方式3：拷贝到项目中

直接将 `logs/` 目录拷贝到你的项目中，然后包含头文件即可。

## 📖 API 参考

### 日志器建造者

```cpp
LoggerBuilder* builder = new GlobalLoggerBuilder();

// 设置日志器名称（必须）
builder->buildLoggerName("my_logger");

// 设置日志等级
builder->buildLoggerLevel(MySpace::LogLevel::INFO);

// 设置格式化模式
builder->buildLoggerFormatter("[%d{%H:%M:%S}][%p] %m%n");

// 设置日志器类型
builder->buildLoggerType(MySpace::LoggerType::LOGGER_ASYNCH);

// 添加输出目标（可以添加多个）
builder->buildSink<MySpace::StdoutSink>();
builder->buildSink<MySpace::FileSink>("./logs/app.log");
builder->buildSink<MySpace::RollBySizeSink>("./logs/roll", 1024*1024);

// 构建日志器
auto logger = builder->build();
```

### 日志器接口

```cpp
logger->debug(const char* fmt, ...);   // DEBUG 级别
logger->info(const char* fmt, ...);    // INFO 级别
logger->warn(const char* fmt, ...);    // WARN 级别
logger->error(const char* fmt, ...);   // ERROR 级别
logger->fatal(const char* fmt, ...);   // FATAL 级别
```

### 全局接口

```cpp
// 获取指定名称的日志器
auto logger = MySpace::getLogger("logger_name");

// 获取默认 root 日志器
auto root = MySpace::rootLogger();

// 使用默认日志器的宏
MySpace::DEBUG("fmt", ...);
MySpace::INFO("fmt", ...);
MySpace::WARN("fmt", ...);
MySpace::ERROR("fmt", ...);
MySpace::FATAL("fmt", ...);
```

## 🤝 贡献指南

欢迎提交 Issue 和 Pull Request！

1. Fork 本仓库
2. 创建特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 提交 Pull Request

## 📄 开源协议

本项目采用 **木兰宽松许可证，第2版（Mulan PSL v2）** 开源。

您可以自由地：
- 复制、使用、修改、分发本软件
- 用于商业或非商业目的

详见 [LICENSE](LICENSE) 文件。

## 📧 联系方式

如有问题或建议，欢迎通过以下方式联系：

- 提交 Issue
- 发送邮件至：[你的邮箱]

## 🙏 致谢

感谢所有为本项目做出贡献的开发者！

---

**⭐ 如果这个项目对你有帮助，请给个 Star！**

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

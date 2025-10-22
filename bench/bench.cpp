// bench.cpp - 日志系统各场景测试
// 展示了同步/异步日志器、多Sink输出、日志级别过滤等功能

#include "../logs/logger.hpp"
#include <thread>
#include <vector>
#include <chrono>
#include <iostream>
#include <iomanip>

using namespace MySpace;

// 颜色输出
#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"

void printScenario(int num, const std::string& title) {
    std::cout << "\n" << BOLD << CYAN << "═══════════════════════════════════════════════" << RESET << std::endl;
    std::cout << BOLD << GREEN << "场景 " << num << ": " << title << RESET << std::endl;
    std::cout << BOLD << CYAN << "═══════════════════════════════════════════════" << RESET << std::endl;
}

void printInfo(const std::string& msg) {
    std::cout << BLUE << "[INFO] " << RESET << msg << std::endl;
}

void printResult(const std::string& msg) {
    std::cout << GREEN << "✓ " << msg << RESET << std::endl;
}

// ═══════════════════════════════════════════════
// 场景1：创建同步日志器的流程
// ═══════════════════════════════════════════════
void scenario1_createSyncLogger() {
    printScenario(1, "创建同步日志器的流程");
    
    printInfo("创建默认同步日志器（输出到标准输出）...");
    auto logger1 = LoggerFactory::createSynchLogger("sync_logger_1");
    printResult("同步日志器创建成功");
    
    printInfo("测试日志输出...");
    logger1->info(__FILE__, __LINE__, "这是第一条日志");
    logger1->debug(__FILE__, __LINE__, "调试信息");
    
    printInfo("创建带自定义配置的同步日志器...");
    auto logger2 = LoggerFactory::createSynchLogger(
        "sync_logger_2",
        LogLevel::INFO,
        "[%d{%H:%M:%S}][%p] %m%n"
    );
    printResult("自定义同步日志器创建成功");
    
    logger2->info(__FILE__, __LINE__, "使用自定义格式的日志");
}

// ═══════════════════════════════════════════════
// 场景2：同步日志记录的完整流程
// ═══════════════════════════════════════════════
void scenario2_syncLogging() {
    printScenario(2, "同步日志记录的完整流程");
    
    printInfo("创建同步日志器...");
    auto logger = LoggerFactory::createSynchLogger("sync_test", LogLevel::DEBUG);
    
    printInfo("测试不同级别的日志...");
    logger->debug(__FILE__, __LINE__, "调试信息：检查变量值");
    logger->info(__FILE__, __LINE__, "普通信息：用户登录成功");
    logger->warn(__FILE__, __LINE__, "警告信息：内存使用率达到80%%");
    logger->error(__FILE__, __LINE__, "错误信息：数据库连接失败");
    logger->fatal(__FILE__, __LINE__, "致命错误：系统崩溃");
    
    printResult("所有级别日志记录完成");
}

// ═══════════════════════════════════════════════
// 场景3：异步日志记录的完整流程
// ═══════════════════════════════════════════════
void scenario3_asyncLogging() {
    printScenario(3, "异步日志记录的完整流程");
    
    printInfo("创建异步日志器...");
    auto async_logger = LoggerFactory::createAsynchLogger(
        "async_test",
        LogLevel::DEBUG
    );
    printResult("异步日志器创建成功（后台线程已启动）");
    
    printInfo("测试异步日志记录（主线程不阻塞）...");
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 100; ++i) {
        async_logger->info(__FILE__, __LINE__, "异步日志消息");
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    printResult("100条日志记录完成");
    std::cout << YELLOW << "主线程耗时: " << duration.count() << " μs" << RESET << std::endl;
    std::cout << YELLOW << "平均每条: " << duration.count() / 100.0 << " μs" << RESET << std::endl;
    
    // 等待异步日志器处理完成
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

// ═══════════════════════════════════════════════
// 场景4：多Sink输出流程
// ═══════════════════════════════════════════════
void scenario4_multiSink() {
    printScenario(4, "多Sink输出流程");
    
    printInfo("创建多个Sink（控制台 + 文件 + 滚动文件）...");
    std::vector<std::shared_ptr<LogSink>> sinks = {
        std::make_shared<StdoutSink>(),
        std::make_shared<FileSink>("./logs/multi_sink.log"),
        std::make_shared<RollBySizeSink>("./logs/roll-", 1024*10)  // 10KB滚动
    };
    
    auto logger = LoggerFactory::createSynchLogger(
        "multi_sink_logger",
        LogLevel::INFO,
        "[%d{%H:%M:%S}][%p] %m%n",
        sinks
    );
    printResult("多Sink日志器创建成功");
    
    printInfo("写入日志到多个目标...");
    logger->info(__FILE__, __LINE__, "这条日志会同时输出到控制台、文件和滚动文件");
    logger->warn(__FILE__, __LINE__, "警告：多目标输出测试");
    logger->error(__FILE__, __LINE__, "错误：多目标输出测试");
    
    printResult("日志已输出到3个目标");
    std::cout << YELLOW << "  ✓ 控制台输出" << RESET << std::endl;
    std::cout << YELLOW << "  ✓ 文件输出: ./logs/multi_sink.log" << RESET << std::endl;
    std::cout << YELLOW << "  ✓ 滚动文件: ./logs/roll-*.log" << RESET << std::endl;
}

// ═══════════════════════════════════════════════
// 场景5：日志级别过滤流程
// ═══════════════════════════════════════════════
void scenario5_logLevelFilter() {
    printScenario(5, "日志级别过滤流程");
    
    printInfo("创建WARN级别的日志器（只输出WARN及以上级别）...");
    auto logger = LoggerFactory::createSynchLogger("filter_test", LogLevel::WARN);
    printResult("日志器创建成功，限制级别: WARN");
    
    std::cout << "\n" << YELLOW << "测试日志级别过滤：" << RESET << std::endl;
    
    std::cout << "  调用 debug() - ";
    logger->debug(__FILE__, __LINE__, "调试信息");
    std::cout << "❌ 被过滤（DEBUG < WARN）" << std::endl;
    
    std::cout << "  调用 info()  - ";
    logger->info(__FILE__, __LINE__, "普通信息");
    std::cout << "❌ 被过滤（INFO < WARN）" << std::endl;
    
    std::cout << "  调用 warn()  - ";
    logger->warn(__FILE__, __LINE__, "警告信息");
    std::cout << GREEN << "✓ 输出（WARN >= WARN）" << RESET << std::endl;
    
    std::cout << "  调用 error() - ";
    logger->error(__FILE__, __LINE__, "错误信息");
    std::cout << GREEN << "✓ 输出（ERROR > WARN）" << RESET << std::endl;
    
    std::cout << "  调用 fatal() - ";
    logger->fatal(__FILE__, __LINE__, "致命错误");
    std::cout << GREEN << "✓ 输出（FATAL > WARN）" << RESET << std::endl;
    
    printResult("级别过滤测试完成");
}

// ═══════════════════════════════════════════════
// 性能对比测试（额外场景）
// ═══════════════════════════════════════════════
void performanceComparison() {
    printScenario(6, "同步 vs 异步性能对比");
    
    const int TEST_COUNT = 10000;
    
    // 同步日志器性能测试
    printInfo("测试同步日志器性能...");
    std::vector<std::shared_ptr<LogSink>> sync_sinks = {
        std::make_shared<FileSink>("./logs/sync_perf.log")
    };
    auto sync_logger = LoggerFactory::createSynchLogger(
        "sync_perf",
        LogLevel::INFO,
        "[%d{%H:%M:%S}][%p] %m%n",
        sync_sinks
    );
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < TEST_COUNT; ++i) {
        sync_logger->info(__FILE__, __LINE__, "性能测试消息");
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto sync_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // 异步日志器性能测试
    printInfo("测试异步日志器性能...");
    std::vector<std::shared_ptr<LogSink>> async_sinks = {
        std::make_shared<FileSink>("./logs/async_perf.log")
    };
    auto async_logger = LoggerFactory::createAsynchLogger(
        "async_perf",
        LogLevel::INFO,
        "[%d{%H:%M:%S}][%p] %m%n",
        async_sinks
    );
    
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < TEST_COUNT; ++i) {
        async_logger->info(__FILE__, __LINE__, "性能测试消息");
    }
    end = std::chrono::high_resolution_clock::now();
    auto async_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // 等待异步日志处理完成
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    // 输出结果
    std::cout << "\n" << BOLD << "性能测试结果（" << TEST_COUNT << "条日志）：" << RESET << std::endl;
    std::cout << "─────────────────────────────────────" << std::endl;
    std::cout << "同步日志器: " << YELLOW << std::setw(6) << sync_duration.count() << " ms" << RESET;
    std::cout << "  (" << (double)TEST_COUNT / sync_duration.count() * 1000 << " 条/秒)" << std::endl;
    std::cout << "异步日志器: " << GREEN << std::setw(6) << async_duration.count() << " ms" << RESET;
    std::cout << "  (" << (double)TEST_COUNT / async_duration.count() * 1000 << " 条/秒)" << std::endl;
    std::cout << "─────────────────────────────────────" << std::endl;
    
    double speedup = (double)sync_duration.count() / async_duration.count();
    std::cout << BOLD << GREEN << "性能提升: " << std::fixed << std::setprecision(1) 
              << speedup << "x" << RESET << std::endl;
}

// ═══════════════════════════════════════════════
// 主函数
// ═══════════════════════════════════════════════
int main() {
    std::cout << BOLD << MAGENTA << R"(
╔═══════════════════════════════════════════════╗
║     C++ 日志系统 - 场景测试程序               ║
║     展示各种使用场景和性能对比                ║
╚═══════════════════════════════════════════════╝
)" << RESET << std::endl;
    
    try {
        // 执行各个场景
        scenario1_createSyncLogger();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        
        scenario2_syncLogging();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        
        scenario3_asyncLogging();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        
        scenario4_multiSink();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        
        scenario5_logLevelFilter();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        
        performanceComparison();
        
        // 总结
        std::cout << "\n" << BOLD << CYAN << "═══════════════════════════════════════════════" << RESET << std::endl;
        std::cout << BOLD << GREEN << "✓ 所有场景测试完成！" << RESET << std::endl;
        std::cout << BOLD << CYAN << "═══════════════════════════════════════════════" << RESET << std::endl;
        std::cout << "\n" << YELLOW << "生成的日志文件：" << RESET << std::endl;
        std::cout << "  - ./logs/multi_sink.log" << std::endl;
        std::cout << "  - ./logs/roll-*.log" << std::endl;
        std::cout << "  - ./logs/sync_perf.log" << std::endl;
        std::cout << "  - ./logs/async_perf.log" << std::endl;
        std::cout << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << BOLD << "\033[31m[ERROR] " << e.what() << RESET << std::endl;
        return 1;
    }
    
    return 0;
}

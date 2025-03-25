#include "../logs/mylog.hpp"
#include <chrono>

void bench(const std::string &logger_name, size_t thread_count, size_t msg_count, size_t msg_len) {
    /* 1.获取日志器           */
    std::shared_ptr<MySpace::Logger> logger = MySpace::getLogger(logger_name);
    if (!logger.get()) {return ;}//需要注销测试下
    std::cout << "测试日志：" << msg_count << " 条, 总大小:" << msg_count * msg_len / 1024 << "KB" << std::endl;
    /* 2.组织指定长度的日志消息 */
    std::string msg(msg_len - 1, 'A'); // 最后一个字节是换行符，便于换行打印 
    /* 3.创建指定数量的线程    */
    std::vector<std::thread> threads;
    std::vector<double> cost_array(thread_count);//总的数值统计
    size_t msg_prt_thr = msg_count / thread_count;   // 每个线程输出的日志条数
    for (int i = 0; i < thread_count; i++) {
        //引用捕获参数( lambda 函数内部可以直接访问并修改这些变量的原始值)，传值i,lambda表达式是对象，所以直接生成对象
        threads.emplace_back([&, i](){
            /* 4.记录开始时间  */
            auto start = std::chrono::high_resolution_clock::now();
            /* 5.开始循环写日志       */
            for (int j = 0; j < msg_prt_thr; j++) {
                logger->fatal("%s", msg.c_str());
            }
            /* 6.线程函数内部结束计时  */
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> cost = end - start;
            cost_array[i] = cost.count();
            std::cout << "线程[" << i << "]: " << "  输出日志数量:" << msg_prt_thr << ", 耗时:" << cost.count()  << "s" << std::endl;
        });
    }
    for (int i = 0; i < thread_count; i++) {
        threads[i].join();
    }
    /* 7.计算总耗时  多线程中，每个线程都有自己运行的时间，但是线程是并发处理的，因此耗时最多的那个才是总时间 */
    double max_cost = cost_array[0];
    for (int i = 0; i < thread_count; i++) {
        max_cost = max_cost > cost_array[i] ? max_cost : cost_array[i];
    }
    size_t msg_prt_sec = msg_count / max_cost;
    size_t size_prt_sec = (msg_count * msg_len) / (max_cost * 1024);
    /* 8.进行输出打印 */
    std::cout << "总耗时: " << max_cost << "s" << std::endl;
    std::cout << "每秒输出日志数量: " << msg_prt_sec  << " 条"  << std::endl;
    std::cout << "每秒输出日志大小: " << size_prt_sec << " KB" << std::endl; 
}
//同步日志测试
void sync_bench() {
    std::shared_ptr<MySpace::LoggerBuilder> builder(new MySpace::GlobalLoggerBuilder());
    builder->buildLoggerName("sync_logger");
    builder->buildLoggerFormatter("%m%n");
    builder->buildLoggerType(MySpace::LoggerType::LOGGER_SYNCH);//同步日志器
    builder->buildSink<MySpace::FileSink>("./logfile/sync.log");
    builder->buildSink<MySpace::RollBySizeSink>("./logfile/roll-sync-by-size", 1024 * 1024);
    builder->build();
    // bench("sync_logger", 1, 1000000, 100);//单线程
 	bench("sync_logger", 10, 1000000, 100);//10个线程
}
//异步日志测试
void async_bench() {
    std::shared_ptr<MySpace::LoggerBuilder> builder(new MySpace::GlobalLoggerBuilder());
    builder->buildLoggerName("async_logger");
    builder->buildLoggerFormatter("%m%n");
    builder->buildLoggerType(MySpace::LoggerType::LOGGER_ASYNCH);//异步日志器
    builder->buildSink<MySpace::FileSink>("./logfile/async.log");
    builder->buildSink<MySpace::RollBySizeSink>("./logfile/roll-async-by-size", 1024 * 1024);
    builder->build();
    // bench("async_logger", 1, 100000, 10);
    bench("async_logger", 10, 100000, 10);//10个线程
}

int main() {
    // sync_bench();
    async_bench();
    return 0;
}

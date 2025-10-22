//sink.hpp
#pragma once
#include "level.hpp"
#include "util.hpp"
#include "message.hpp"
#include <string>
#include <iostream>
#include <assert.h>
#include <fstream>
#include <memory>
#include <sstream>
#include <mutex>
// MySQL Connector/C++ 头文件
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/exception.h>

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

    // 落地方向：MySQL 数据库（使用 MySQL Connector/C++）
    class MySQLSink : public LogSink {
        public:
            // 构造函数：连接到 MySQL 数据库
            // host: 数据库主机地址
            // user: 数据库用户名
            // password: 数据库密码
            // database: 数据库名称
            // table: 日志表名称（默认为 "logs"）
            // port: 数据库端口（默认为 3306）
            MySQLSink(const std::string &host, 
                     const std::string &user,
                     const std::string &password, 
                     const std::string &database,
                     const std::string &table = "logs",
                     unsigned int port = 3306)
                : _host(host)
                , _user(user)
                , _password(password)
                , _database(database)
                , _table(table)
                , _port(port)
                , _connected(false)
            {
                try {
                    // 1. 获取 MySQL 驱动实例
                    _driver = sql::mysql::get_mysql_driver_instance();
                    
                    // 2. 构建连接 URL：tcp://host:port
                    std::ostringstream url;
                    url << "tcp://" << _host << ":" << _port;
                    
                    // 3. 连接到 MySQL 服务器
                    _conn.reset(_driver->connect(url.str(), _user, _password));
                    
                    // 4. 选择要使用的数据库
                    _conn->setSchema(_database);
                    
                    // 5. 创建SQL语句执行器
                    std::unique_ptr<sql::Statement> stmt(_conn->createStatement());
                    
                    _connected = true;
                    std::cout << "MySQL连接成功，数据库: " << _database << std::endl;
                    
                    // 6. 创建日志表（如果不存在）
                    createTableIfNotExists();
                    
                } catch (sql::SQLException &e) {
                    std::cerr << "MySQL连接失败: " << e.what() << std::endl;
                    std::cerr << "错误码: " << e.getErrorCode() << std::endl;
                    std::cerr << "SQLState: " << e.getSQLState() << std::endl;
                    _connected = false;
                }
            }

            // 将日志写入 MySQL 数据库
            void log(const std::string& data, size_t len) override {
                if (!_connected || !_conn) {
                    std::cerr << "MySQL未连接，无法写入日志" << std::endl;
                    return;
                }

                // 加锁保证线程安全
                std::unique_lock<std::mutex> lock(_mutex);

                try {
                    // 提取日志内容（去除末尾换行符）
                    std::string log_content(data, 0, len);
                    if (!log_content.empty() && log_content.back() == '\n') {
                        log_content.pop_back();
                    }

                    // 使用 PreparedStatement 防止 SQL 注入
                    std::ostringstream sql;
                    sql << "INSERT INTO " << _table 
                        << " (log_content, log_time) VALUES (?, NOW())";
                    
                    std::unique_ptr<sql::PreparedStatement> pstmt(
                        _conn->prepareStatement(sql.str())
                    );
                    
                    pstmt->setString(1, log_content);
                    pstmt->executeUpdate();
                    
                } catch (sql::SQLException &e) {
                    std::cerr << "MySQL插入日志失败: " << e.what() << std::endl;
                    std::cerr << "错误码: " << e.getErrorCode() << std::endl;
                }
            }

        private:
            // 创建日志表（如果不存在）
            void createTableIfNotExists() {
                try {
                    std::ostringstream sql;
                    sql << "CREATE TABLE IF NOT EXISTS " << _table << " ("
                        << "id BIGINT AUTO_INCREMENT PRIMARY KEY, "
                        << "log_content TEXT NOT NULL, "
                        << "log_time DATETIME NOT NULL, "
                        << "INDEX idx_log_time (log_time)"
                        << ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci";

                    std::unique_ptr<sql::Statement> stmt(_conn->createStatement());
                    stmt->execute(sql.str());
                    
                    std::cout << "日志表 " << _table << " 已准备就绪" << std::endl;
                    
                } catch (sql::SQLException &e) {
                    std::cerr << "创建日志表失败: " << e.what() << std::endl;
                }
            }

        private:
            std::string _host;                          // 数据库主机地址
            std::string _user;                          // 数据库用户名
            std::string _password;                      // 数据库密码
            std::string _database;                      // 数据库名称
            std::string _table;                         // 日志表名称
            unsigned int _port;                         // 数据库端口
            sql::mysql::MySQL_Driver *_driver;         // MySQL 驱动（单例）
            std::unique_ptr<sql::Connection> _conn;     // 数据库连接（智能指针）
            bool _connected;                            // 连接状态
            std::mutex _mutex;                          // 保护线程安全的互斥锁
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
    * 
    * 4. MySQL 数据库 Sink（4-6个参数）：
    *    // 基本用法（使用默认表名 "logs" 和默认端口 3306）：
    *    auto sink = SinkFactory::create<MySQLSink>(
    *        "localhost",        // 主机地址
    *        "root",             // 用户名
    *        "password",         // 密码
    *        "logdb"             // 数据库名
    *    );
    *    
    *    // 完整用法（指定表名和端口）：
    *    auto sink = SinkFactory::create<MySQLSink>(
    *        "192.168.1.100",    // 主机地址
    *        "logger",           // 用户名
    *        "secret123",        // 密码
    *        "app_logs",         // 数据库名
    *        "application_logs", // 表名（可选，默认为 "logs"）
    *        3307                // 端口（可选，默认为 3306）
    *    );
    *    
    *    注意：
    *    - 使用 MySQL Connector/C++ 实现（现代 C++ 风格）
    *    - 需要预先创建数据库，程序会自动创建日志表
    *    - 日志表结构：id (BIGINT), log_content (TEXT), log_time (DATETIME)
    *    - 编译时需要链接 MySQL Connector/C++ 库：-lmysqlcppconn
    *    - Linux 安装依赖：sudo apt-get install libmysqlcppconn-dev
    *    - 使用 PreparedStatement 防止 SQL 注入，更加安全
    *    - 支持智能指针自动资源管理，无需手动释放内存
    */


}
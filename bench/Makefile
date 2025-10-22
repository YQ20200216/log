
bench: bench.cpp
	g++ -std=c++14 -o $@ $^ -pthread -lmysqlcppconn

# 清理
clean:
	rm -f bench

# 帮助信息
help:
	@echo "可用的编译目标："
	@echo "  make bench        - 编译基本版本（不含 MySQL）"
	@echo "  make bench-mysql  - 编译包含 MySQL 支持的版本"
	@echo "  make clean        - 清理生成的文件"

.PHONY: clean help bench-mysql

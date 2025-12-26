CXX = g++ # Компилятор С++
CXXFLAGS = -std=c++11 -Wall -Wextra -pedantic # Флаги С++11 и предупр.
TARGETS = client server # Имена исполняемых файлов

# Исходные файлы
CLIENT_SOURCES = client.cpp common.cpp
SERVER_SOURCES = server.cpp common.cpp

all: $(TARGETS)  # Цель по умолчанию

client: $(CLIENT_SOURCES)
	$(CXX) $(CXXFLAGS) -o client $(CLIENT_SOURCES)

server: $(SERVER_SOURCES)
	$(CXX) $(CXXFLAGS) -pthread -o server $(SERVER_SOURCES)

clean:
	rm -f $(TARGETS) *.o ipc_file.bin # Очистка.

.PHONY: all clean # Объявляем псевдоцели

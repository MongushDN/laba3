CXX = g++	#Компилятор С++
CXXFLAGS = -std=c++11 -Wall -Wextra -pedantic #флаги С++11 и предупр.
TARGETS = client server	#имя исполняемого файла.
CLIENT_SOURCES = common.cpp client.cpp
SERVER_SOURCES = common.cpp server.cpp		#исходные файлы(все зависимость)

all: $(TARGETS)		#разворачиваются в клиент.

client: common.cpp client.cpp	#сборка клиент.
	$(CXX) $(CXXFLAGS) -o client common.cpp client.cpp
server: $(SERVER_SOURCES)
	$(CXX) $(CXXFLAGS) -o server $(SERVER_SOURCES) 

clean:
	rm -f $(TARGETS) *.o ipc_file.bin	#очистка.

.PHONY: all clean	#не файлы

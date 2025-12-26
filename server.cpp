#include <iostream>      
#include <thread>        
#include <mutex>         
#include <queue>         
#include <cstring>       
#include <sys/socket.h>  
#include <netinet/in.h> 
#include <unistd.h>
#include "common.h" 

class ThreadSafeQueue {
private:
    std::queue<message_t> messages;
    std::mutex mtx;
    
public:
    void push(const message_t& message) {
        std::lock_guard<std::mutex> lock(mtx);
        messages.push(message);
        std::cout << "Добавлен запрос " << message.numb 
                  << ". Размер очереди: " << messages.size() << std::endl;
    }

    // TODO обработка очереди на Тёме
};

ThreadSafeQueue globalQueue;

void handleClient(int clientSocket, int clientId) {
    std::cout << "Клиент " << clientId << ". Начинаем обработку." << std::endl;
    
    while (true) {
        message_t receivedMsg;
        
        ssize_t bytes = recv(clientSocket, &receivedMsg, sizeof(receivedMsg), 0);
        
        if (bytes <= 0) {
            if (bytes == 0) {
                std::cout << "Клиент " << clientId << " отключился." << std::endl;
            } else {
                std::cerr << "Ошибка при получении данных от клиента " << clientId << std::endl;
            }
            break;
        }
        
        if (bytes != sizeof(receivedMsg)) {
            std::cerr << "Неверный размер сообщения от клиента " << clientId << std::endl;
            break;
        }
        
        globalQueue.push(receivedMsg);
        
        message_t pongMsg;
        init_message(&pongMsg, msg_PONG, receivedMsg.numb);
        
        send(clientSocket, &pongMsg, sizeof(pongMsg), 0);
    }
    
    close(clientSocket);
}


// создала сокет, приняла клиента и добавила в очередь
int main() {
    
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        std::cerr << "Ошибка создания сокета" << std::endl;
        return 1;
    }
    
 
    sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(1234);

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Ошибка bind" << std::endl;
        close(serverSocket);
        return 1;
    }
   
    if (listen(serverSocket, 5) < 0) {
        std::cerr << "Ошибка listen" << std::endl;
        close(serverSocket);
        return 1;
    }
    
    std::cout << "Сервер запущен. Порт: 1234" << std::endl;
    
    int clientId = 0;
    
    while (true) {
        sockaddr_in clientAddr;
        socklen_t clientSize = sizeof(clientAddr);
        
        int clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientSize);
        
        if (clientSocket < 0) {
            continue;  
        }
        
        clientId++;
        
        std::thread(handleClient, clientSocket, clientId).detach();
    }
    
    close(serverSocket);
    return 0;
}

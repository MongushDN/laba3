#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>
#include <cstring>
#include "common.h"

#include <thread>
#include <mutex>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

std::mutex coutMutex; // синхронизация вывода в консоль.

class TCPClient {
private:
    int sockfd;
    struct sockaddr_in server_addr;
    uint32_t client_id;
    int port =1234; //порт сервера.
public:
    TCPClient(uint32_t id, const std::string& ip="127.0.0.1"): client_id(id){
        sockfd=socket(AF_INET, SOCK_STREAM,0);
        if(sockfd<0){
            std::lock_guard<std::mutex> lock(coutMutex);
            std::cerr<<"Client "<<client_id<<": socket creation is failed."<<strerror(errno)<<std::endl; 
            return;}
            
        server_addr.sin_family=AF_INET;
        server_addr.sin_port=htons(port);

        if(inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr)<=0){
            std::lock_guard<std::mutex> lock(coutMutex);
            std::cerr<<"Client "<<client_id<<": invalid address"<<std::endl;
        }
    }

    bool connect_to_server(){
        {
        std::lock_guard<std::mutex> lock(coutMutex);
        std::cout<<"Client "<<client_id<<": is connecting..."<<std::endl;
        }
        if(connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr))<0){
            std::lock_guard<std::mutex> lock(coutMutex);
            std::cerr<<"Client "<<client_id<<": connect failed."<<strerror(errno)<<std::endl;
            return false;
        }
        {
            std::lock_guard<std::mutex> lock(coutMutex);
            std::cout<<"Client "<<client_id<<": is connected!"<<std::endl;
        }
        return true;
    }

    void send_ping(uint32_t numb) {
        message_t pingMSG;
        init_message(&pingMSG, msg_PING, numb);
        {
            std::lock_guard<std::mutex> lock(coutMutex);
            std::cout<<"Client "<<client_id<<": is sending PING #"<<numb<<std::endl;
        }
        ssize_t sentBytes=send(sockfd,&pingMSG,sizeof(pingMSG),0);
        if (sentBytes!=sizeof(pingMSG)){
            std::lock_guard<std::mutex> lock(coutMutex);
            std::cerr<<"Client "<<client_id<<": couldn't send. It failed."<<strerror(errno)<<std::endl;
            return;}
        {
            std::lock_guard<std::mutex>lock(coutMutex);
            std::cout<<"Client "<<client_id<<": PING # "<<numb<<" is sent"<<std::endl;
        }
        wait_for_pong();
    }
    void wait_for_pong(){
        message_t pongMSG;
        ssize_t recievedBytes=recv(sockfd,&pongMSG,sizeof(pongMSG),0);
        if(recievedBytes!=sizeof(pongMSG)){
            std::lock_guard<std::mutex> lock(coutMutex);
            std::cerr<<"Client "<<client_id<<": recieving is failed."<<strerror(errno)<<std::endl;
            return;}
        if(pongMSG.type==msg_PONG){
            std::lock_guard<std::mutex> lock(coutMutex);
            std::cout<<"Client "<<client_id<<": has got PONG for #"<<pongMSG.numb<<std::endl;
        } else{
            std::lock_guard<std::mutex> lock(coutMutex);
            std::cerr<<"Client "<<client_id<<": recieved wrong message type"<<std::endl;
        }
    }
    
    void disconnect(){
        close(sockfd);
        std::lock_guard<std::mutex> lock(coutMutex);
        std::cout<<"Client "<<client_id<<": Disconnected"<<std::endl;
    }
    ~TCPClient(){
    if(sockfd>=0){close(sockfd);}
    }
};

void ClientThreadFunc(uint32_t client_id, int NumMess){
    TCPClient client(client_id);
    if (!client.connect_to_server()){return;}
    for(int i=1;i<=numMess;++i){
        client.send_ping(i);
        sleep(1);
    }
    client.disconnect();
}

int main(int argc, char* argv[])
{
    int numClients=10;
    int numMess=2;
    if(argc>1) numClients=atoi(argv[1]);
    if(argc>2) numMess=atoi(argv[2]);

    std::vector<std::thread> clients;
    
    for(int i=1, i<=numClients;++i){
        clients.emplace_back(ClientThreadFunc,i,numMess);
        usleep(100000);//вроде 100миллисек.
    }
    for(auto& client: clients){
        client.join();
    }
    return 0;
}

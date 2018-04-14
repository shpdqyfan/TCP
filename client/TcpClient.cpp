/* 
    Date: 2018/04/12 
    Author: shpdqyfan 
    profile: Tcp client  
*/

#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <string.h>
#include <unistd.h>

#include "Socket/Socket.h"

using namespace Api;

static const std::string ServerIp("10.13.13.108");
static int ServerPort = 0;

int main(int argc, char* argv[])
{
    if(3 != argc)
    {
        //std::cout<<" parameter error"<<std::endl;
		//exit(1);
	}

    ServerPort = atoi(argv[1]);
    
    std::cout<<"Tcp client start"<<std::endl;

    int sfd = Socket::createSocket(AF_INET, SOCK_STREAM);
    if(-1 == sfd)
    {
        std::cout<<"Tcp client createSocket error"<<std::endl;
        exit(1);
    }

    if(-1 == Socket::setNonBlock(sfd))
    {
        std::cout<<"Tcp client setNonBlock error"<<std::endl;
        close(sfd);
        exit(1);
    } 

    struct sockaddr_in serverAddr;	
	memset(&serverAddr, 0, sizeof serverAddr);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr(ServerIp.c_str());
	serverAddr.sin_port = htons(ServerPort);
    if(-1 == Socket::connectNonblockMode(sfd, (struct sockaddr*)&serverAddr))
    {
        std::cout<<"Tcp client connectNonblockMode error"<<std::endl;
        close(sfd);
        exit(1);
    }

    if(!Socket::checkConnectNonblockMode(sfd))
    {
        std::cout<<"Tcp client checkConnectNonblockMode error"<<std::endl;
        close(sfd);
        exit(1);
    }

    while(1)
    {
        std::string buf("Coming from client");

        if(-1 == Socket::sendMsgNonblockMode(sfd, (void*)buf.c_str(), buf.size()))
        {
            std::cout<<"Tcp client sendMsgNonblockMode error"<<std::endl;
            close(sfd);
            exit(1);
        }

        sleep(2);
    }

    close(sfd);

    return 0;
}


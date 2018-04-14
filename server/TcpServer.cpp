/* 
    Date: 2018/04/12 
    Author: shpdqyfan 
    profile: Tcp server  
*/

#include <iostream>
#include <string>
#include <memory>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <string.h>
#include <unistd.h>

#include "Socket/Socket.h"
#include "Session/Session.h"

using namespace Api;

int main(int argc, char* argv[])
{
    if(3 != argc)
    {
        //std::cout<<" parameter error"<<std::endl;
		//exit(1);
	}

    std::cout<<"Tcp server start"<<std::endl;

	int sfd = Socket::createSocket(AF_INET, SOCK_STREAM);
    if(-1 == sfd)
    {
        std::cout<<"Tcp server createSocket error"<<std::endl;
        exit(1);
    }

    //std::string listenIp = argv[1];
	//int listenPort = atoi(argv[2]);

    std::string listenIp("10.13.13.108");
	int listenPort = atoi(argv[1]);

    std::cout<<"Tcp server ip="<<listenIp<<", port="<<listenPort<<std::endl;
	
    struct sockaddr_in listenAddress;
	struct sockaddr_in remoteAddress;
    memset(&listenAddress, 0, sizeof listenAddress);
	memset(&remoteAddress, 0, sizeof remoteAddress);
	
    listenAddress.sin_family = AF_INET;
    listenAddress.sin_port = htons(listenPort);
	listenAddress.sin_addr.s_addr = inet_addr(listenIp.c_str());
 
    if(-1 == Socket::bindTo(sfd, (struct sockaddr*)&listenAddress, sizeof listenAddress))
    {
        std::cout<<"Tcp server bindTo error"<<std::endl;
        close(sfd);
        exit(1);
    }

    if(-1 == Socket::listenTo(sfd))
    {
	    std::cout<<"Tcp server listenTo error"<<std::endl;
        close(sfd);
        exit(1);
    }

    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(sfd, &rfds);
    while(-1 != select(sfd + 1, &rfds, NULL, NULL, NULL))
    {
        if(!FD_ISSET(sfd, &rfds))
        {
            continue;
        }

        int cfd = Socket::acceptFrom(sfd, (struct sockaddr*)&remoteAddress);
        if(-1 == cfd)
        {
            std::cout<<"Tcp server acceptFrom error"<<std::endl;
            continue;
        }

        std::string clientIp(inet_ntoa(remoteAddress.sin_addr));
        int clientPort = ntohs(remoteAddress.sin_port);
        std::string clientInfo = clientIp + ":" + std::to_string(clientPort);
        Session* s = new Session(clientInfo, cfd);
        s->initSession();
        std::cout<<"Tcp server session has been created"<<std::endl;

        sleep(20);
    }
    
    close(sfd);

    return 0;
}



/*
    Date: 2018/01/18
    Author: shpdqyfan
    profile: define basic behavior of socket   
*/

#ifndef SOCKET_H
#define SOCKET_H

#include <sys/socket.h>

namespace Api{
namespace Socket{
    const int MAX_CONN_NUM = 5;
    
    int createSocket(sa_family_t family, int stype);
    int setNonBlock(int sfd);
    int bindTo(int sfd, const struct sockaddr* saddr, int len);
    int listenTo(int sfd);
    int acceptFrom(int sfd, struct sockaddr* saddr);
    int connectBlockMode(int sfd, const struct sockaddr* saddr);
    int connectNonblockMode(int sfd, const struct sockaddr* saddr);
    void closeFd(int sfd);
    void shutdownFd(int sfd, int flag);
    ssize_t recvMsgFrom(int sfd, void* buf, int size);
    ssize_t sendMsgToUds(int sfd, const char* sockFile, void* buf, int size);
    //this check is only for unblock connect
    bool checkConnectNonblockMode(int sfd);
    int sendMsgNonblockMode(int sfd, void* buf, int len);
    int recvMsgNonblockMode(int sfd, void* buf, int len);
}
};

#endif


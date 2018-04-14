/*
    Date: 2018/01/18
    Author: shpdqyfan
    profile: define basic behavior of socket   
*/

#include <iostream>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/un.h>
#include <string.h>

#include "Socket/Socket.h"

using namespace Api;

int Socket::createSocket(sa_family_t family, int stype)
{
    int sfd = socket(family, stype, 0);
    if(0 > sfd)
    {
        std::cout<<"createSocket, errno="<<errno<<", "<<strerror(errno)<<std::endl;
        return -1;
    }

    std::cout<<"createSocket, succ, sfd="<<sfd<<std::endl;
    
    return sfd;
}

int Socket::setNonBlock(int sfd)
{
    int flags = fcntl(sfd, F_GETFL, 0);
    if(0 > fcntl(sfd, F_SETFL, flags | O_NONBLOCK))
    {
        std::cout<<"setNonBlock, sfd="<<sfd<<", errno="<<errno<<", "<<strerror(errno)<<std::endl;
        return -1;
    }

    return 0;
}

int Socket::bindTo(int sfd, const struct sockaddr* saddr, int len)
{
    if(0 > bind(sfd, saddr, len))
    {
        std::cout<<"bindTo, sfd="<<sfd<<", errno="<<errno<<", "<<strerror(errno)<<std::endl;
        return -1;
    }

    return 0;
}

int Socket::listenTo(int sfd)
{
    if(0 > listen(sfd, MAX_CONN_NUM))
    {
        std::cout<<"listenTo, sfd="<<sfd<<", errno="<<errno<<", "<<strerror(errno)<<std::endl;
        return -1;
    }

    return 0;
}

int Socket::acceptFrom(int sfd, struct sockaddr* saddr)
{
    socklen_t slen = sizeof(sockaddr);
    int newfd = accept(sfd, saddr, &slen);
    if(0 > newfd)
    {
        std::cout<<"acceptFrom, sfd="<<sfd<<", errno="<<errno<<", "<<strerror(errno)<<std::endl;
        return -1;
    }

    return newfd;
}

int Socket::connectBlockMode(int sfd, const struct sockaddr* saddr)
{
    int rlt = connect(sfd, saddr, sizeof (*saddr));
    if(0 > rlt)
    {
        std::cout<<"connectBlockMode, sfd="<<sfd<<", errno="<<errno<<", "<<strerror(errno)<<std::endl;
        return -1;
    }

    return 0;
}

int Socket::connectNonblockMode(int sfd, const struct sockaddr* saddr)
{
    int rlt = connect(sfd, saddr, sizeof (*saddr));
    int connErrno = (0 == rlt) ? 0 : errno;
    std::cout<<"connectNonblockMode, sfd="<<sfd<<", errno="<<errno<<", "<<strerror(errno)<<std::endl;

	switch(connErrno)
	{
    case 0:
    case EINPROGRESS:
	case EISCONN:
	case EINTR:
		return 0;
	case EAGAIN:
	case EADDRINUSE:
	case EADDRNOTAVAIL:
	case ECONNREFUSED:
	case ENETUNREACH:
        //for "connect", we can't recall immediately, or error would happened
		return -1;
	default:
		return -1;
	}

    return -1;
}

void Socket::closeFd(int sfd)
{
    close(sfd);
    
    std::cout<<"closeFd, sfd="<<sfd<<", errno="<<errno<<", "<<strerror(errno)<<std::endl;
}

void Socket::shutdownFd(int sfd, int flag)
{
    shutdown(sfd, flag);
    
    std::cout<<"shutdownFd, sfd="<<sfd<<", flag="<<flag<<", errno="<<errno<<", "<<strerror(errno)<<std::endl;
}

ssize_t Socket::recvMsgFrom(int sfd, void* buf, int size)
{
    struct iovec ioBuf[1];
    struct msghdr mhdr;

    ioBuf[0].iov_base = buf;
    ioBuf[0].iov_len = size;
    
    mhdr.msg_name = NULL;
    mhdr.msg_namelen = 0;
    mhdr.msg_iov = ioBuf;
    mhdr.msg_iovlen = 1;
    mhdr.msg_control = NULL;
    mhdr.msg_controllen = 0;
    mhdr.msg_flags = 0;
    
    int len = recvmsg(sfd, &(mhdr), 0);
    std::cout<<"recvMsgFrom, sfd="<<sfd<<", len="<<len<<", size="<<size<<std::endl;
    if(0 > len)
    {
        std::cout<<"recvMsgFrom, error1, sfd="<<sfd<<", errno="<<errno<<", "<<strerror(errno)<<std::endl;
        return -1;
    }
    else if(len != size)
    {
        std::cout<<"recvMsgFrom, error2, sfd="<<sfd<<", errno="<<errno<<", "<<strerror(errno)<<std::endl;
        return -1;
    }

    return len;
}

ssize_t Socket::sendMsgToUds(int sfd, const char* sockFile, void* buf, int size)
{
    struct sockaddr_un sUnAddr;
    struct iovec ioBuf[1];
    struct msghdr mhdr;

    sUnAddr.sun_family = AF_UNIX;
    strcpy(sUnAddr.sun_path, sockFile);

    ioBuf[0].iov_base = buf;
    ioBuf[0].iov_len = size;
    
    mhdr.msg_name = &sUnAddr;
    mhdr.msg_namelen = sizeof(sockaddr_un);
    mhdr.msg_iov = ioBuf;
    mhdr.msg_iovlen = 1;
    mhdr.msg_control = NULL;
    mhdr.msg_controllen = 0;
    mhdr.msg_flags = 0;
    
    int len = sendmsg(sfd, &(mhdr), 0);
    std::cout<<"sendMsgToUds, sfd="<<sfd<<", len="<<len<<std::endl;
    if(0 > len)
    {
        std::cout<<"sendMsgToUds, sfd="<<sfd<<", errno="<<errno<<", "<<strerror(errno)<<std::endl;
        return -1;
    }

    return len;
}

bool Socket::checkConnectNonblockMode(int sfd)
{
    fd_set rfds, wfds;
    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    FD_SET(sfd, &rfds);
    FD_SET(sfd, &wfds);

    while(1)
    {
        
        int rlt = select(sfd + 1, &rfds, &wfds, NULL, NULL);
        std::cout<<"checkConnectNonblockMode, sfd="<<sfd<<", errno="<<errno<<", "<<strerror(errno)<<std::endl;

        if(-1 == rlt)
        {
            if(errno == EINTR)
            {
                //we can set retry times
                continue;
            }
            else
            {
                return false;
            }
        }
    }

    if(FD_ISSET(sfd, &rfds) || FD_ISSET(sfd, &wfds))
    {
        int error = 0;
        socklen_t len = sizeof error;
        getsockopt(sfd, SOL_SOCKET, SO_ERROR, &error, &len);
        std::cout<<"checkConnectNonblockMode, sfd="<<sfd<<", error="<<error<<std::endl;
        if(error)  	
        {
            return false;
        }
        else
        {
            return true;
        }
    }

	return false;
}

int Socket::sendMsgNonblockMode(int sfd, void* buf, int len)
{
    int bufSize = len;
    int sentSize = 0;
    int tmpSize = 0;
    char* bufPtr = (char*)buf;

    do
    {
        //MSG_NOSIGNAL: avoid generate SIGPIPE(PIPE broken would trigger process exit) 
        //when re-"send" data to peer which was already closed
        tmpSize = send(sfd, bufPtr, bufSize, MSG_NOSIGNAL);
        std::cout<<"sendMsgNonblockMode, sfd="<<sfd<<", errno="<<errno<<", "<<strerror(errno)<<std::endl;
        if(0 > tmpSize)
        {
            tmpSize = 0;
            if(errno == EINTR)
            {
                //retry
                continue;
            }
            else if((errno == EAGAIN) || (errno == EWOULDBLOCK))
            {
                //sleep ?
                continue;
            }
            else
            {
                //error happened, reconnect ?
                return -1;
            }
        }

        bufSize = bufSize - tmpSize;
        bufPtr = bufPtr + tmpSize;
        sentSize = sentSize + tmpSize;
    }while(0 < bufSize);

    return sentSize;
}

int Socket::recvMsgNonblockMode(int sfd, void* buf, int len)
{
    int bufSize = len;
    int recvSize = 0;
    int tmpSize = 0;
    char* bufPtr = (char*)buf;

    do
    {
        tmpSize = recv(sfd, bufPtr, bufSize, 0);
        std::cout<<"recvMsgNonblockMode, sfd="<<sfd<<", errno="<<errno<<", "<<strerror(errno)<<std::endl;
        if(0 == tmpSize)
        {
            std::cout<<"recvMsgNonblockMode, sfd="<<sfd<<" normal closed"<<std::endl;
            return 0;
        }
        else if(0 > tmpSize)
        {
            if(errno == EINTR)
            {
                //retry
                continue; 
            }
            else if((errno == EAGAIN) || (errno == EWOULDBLOCK))
            {
                //sleep ?
                continue;
            }
            else
            {
                //error happened
                return -1;
            }
        }

        recvSize = recvSize + tmpSize;
        bufPtr = bufPtr + tmpSize;

    }while(recvSize < bufSize);

    return recvSize;
}


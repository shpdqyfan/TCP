/* 
    Date: 2018/04/12 
    Author: shpdqyfan 
    profile: Common session
*/

#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include "Session/Session.h"
#include "Socket/Socket.h"

using namespace Api;

Session::Session(const std::string& clientInfo, int sfd)
    : myClientInfo(clientInfo)
    , mySocketFd(sfd)
    , myWakeFd(-1)
    , myState(INIT)
{
    std::cout<<"Session, construct, clientInfo="<<clientInfo<<", fd="<<mySocketFd<<std::endl;
}

Session::~Session()
{
    std::cout<<"Session, deconstruct"<<std::endl;
    close(mySocketFd);
    join();
}

const std::string& Session::getClientInfo() const
{
    return myClientInfo;
}

void Session::initSession()
{
    start();
}

void Session::closeSession()
{
    if(CLOSED != myState)
    {
        myState = CLOSED;
        //write sth to wake up session thread to exit the main IO loop
        char x(1);
        write(myWakeFd, &x, 1);
    }
}

void Session::setSessionState(SessionState state)
{
    myState = state;
}

Session::SessionState Session::getSessionState() const
{
    return myState;
}

void Session::mainIO()
{
    int fds[2];
    fd_set readSet;

    //create pipe
    if(-1 == pipe(fds))
    {
        std::cout<<"Session, mainIO, pipe error"<<std::endl;
        return;
    }

    //set read fd
    fcntl(fds[0], F_SETFL, FD_CLOEXEC | O_NONBLOCK);
    //set write fd
    fcntl(fds[1], F_SETFL, FD_CLOEXEC | O_NONBLOCK);

    //use write fd of pipe to wake up session thread
    myWakeFd = fds[1];
    int maxFd = std::max(fds[0], mySocketFd);
    
    while(ENABLED == myState)
    {
        //init fd_set
        FD_ZERO(&readSet);
        //add socket fd into read set to watch for input event
        FD_SET(mySocketFd, &readSet);
        //add read fd of pipe into read set to watch for wakeup event
        FD_SET(fds[0], &readSet);

        //watch for readable event
        if(-1 == select(maxFd + 1, &readSet, NULL, NULL, NULL))
        {
            //for "accept, read, write, select, open, etc", we can  recall immediately
            if(errno == EINTR)
            {
                continue;
            }
            else
            {
                std::cout<<"Session, mainIO, select errno"<<errno<<", strerror="<<strerror(errno)<<std::endl;
                break;
            }
        }

        //wake up event received, exit main IO thread
        if(FD_ISSET(fds[0], &readSet))
        {
            std::cout<<"Session, mainIO, wake up"<<std::endl;
            break;
        }

        //input event received from socket
        if(FD_ISSET(mySocketFd, &readSet) && (ENABLED == myState))
        {
            //ignore inactivity timer of this session, we can add it in the future
            handleIO();
        }
    }

    close(fds[0]);
    close(fds[1]);
}

void Session::handleIO()
{
    //just for test, actually we should have a header which carry the length of buf sent from client
    char buf[18];
    memset(buf, 0, 18);
    Socket::recvMsgNonblockMode(mySocketFd, (void*)buf, 18);
    std::string content(buf);
    std::cout<<"Session, handleIO buf="<<content<<std::endl;
}

void Session::run()
{
    std::cout<<"Session, run start"<<std::endl;
    
    myState = ENABLED;
    mainIO();

    std::cout<<"Session, run end"<<std::endl;
}


/* 
    Date: 2018/04/12 
    Author: shpdqyfan 
    profile: Common session  
*/

#ifndef SESSION_H
#define SESSION_H

#include <string>

#include "Thread/Thread.h"

class Session : public MyThread
{
public:
    enum SessionState
    {
        INIT = 0,
        ENABLED = 1,
        CLOSED = 2,
    };
    
    Session(const std::string& clientInfo, int sfd);
    ~Session();

    const std::string& getClientInfo() const;
    void initSession();
    void closeSession();
    void setSessionState(SessionState state);
    Session::SessionState getSessionState() const;

protected:
    void run();

private:
    void mainIO();
    void handleIO();
    
    Session(const Session& s);
    Session& operator=(const Session& s);

    std::string myClientInfo;
    int mySocketFd;
    int myWakeFd;
    SessionState myState;
};

#endif



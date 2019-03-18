#ifndef MSGMGR_H
#define MSGMGR_H

#include <queue>
#ifndef __LINUX
#include <windows.h>
#else

#include <pthread.h>

#define HANDLE pthread_mutex_t

#define TRUE    1
#define FALSE   0


#endif
#include "socketmessage.h"

class MsgMgr
{
private:
    HANDLE m_hMutex;
    std::queue<ComMsg> m_msgqueue;

public:
    MsgMgr();
    ~MsgMgr();
    void Push(ComMsg msg);
    void Pop(ComMsg* msg);
    int GetSize();
};

#endif // MSGMGR_H

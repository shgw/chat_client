#ifndef MSGMGR_H
#define MSGMGR_H

#include <queue>
#include <windows.h>
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

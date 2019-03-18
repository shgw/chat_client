#include "msgmgr.h"

MsgMgr::MsgMgr()
{
    m_hMutex = CreateMutex(NULL, FALSE, NULL);

}

MsgMgr::~MsgMgr()
{
    CloseHandle(m_hMutex);
}


void MsgMgr::Push(ComMsg msg)
{
    WaitForSingleObject(m_hMutex, INFINITE);
    m_msgqueue.push( msg );
    ReleaseMutex(m_hMutex);
}

void MsgMgr::Pop(ComMsg* msg)
{
    WaitForSingleObject(m_hMutex, INFINITE);
    ComMsg popMsg = m_msgqueue.front();
    memcpy( msg, &popMsg, sizeof(ComMsg));
    m_msgqueue.pop();
    ReleaseMutex(m_hMutex);
}


int MsgMgr::GetSize()
{
    return m_msgqueue.size();
}

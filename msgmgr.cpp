#include "msgmgr.h"

#ifdef __LINUX
#include <string.h>
#endif

MsgMgr::MsgMgr()
{
#ifndef __LINUX
    m_hMutex = CreateMutex(NULL, FALSE, NULL);
#else
    m_hMutex = PTHREAD_MUTEX_INITIALIZER;
#endif

}

MsgMgr::~MsgMgr()
{
#ifndef __LINUX
    CloseHandle(m_hMutex);
#endif
}


void MsgMgr::Push(ComMsg msg)
{
#ifndef __LINUX
    WaitForSingleObject(m_hMutex, INFINITE);
    m_msgqueue.push( msg );
    ReleaseMutex(m_hMutex);
#else
    pthread_mutex_lock(&m_hMutex);
    m_msgqueue.push( msg );
    pthread_mutex_unlock(&m_hMutex);
#endif
}

void MsgMgr::Pop(ComMsg* msg)
{
#ifndef __LINUX
    WaitForSingleObject(m_hMutex, INFINITE);
    ComMsg popMsg = m_msgqueue.front();
    memcpy( msg, &popMsg, sizeof(ComMsg));
    m_msgqueue.pop();
    ReleaseMutex(m_hMutex);
#else
    pthread_mutex_lock(&m_hMutex);
    ComMsg popMsg = m_msgqueue.front();
    memcpy( msg, &popMsg, sizeof(ComMsg));
    m_msgqueue.pop();
    pthread_mutex_unlock(&m_hMutex);
#endif
}


int MsgMgr::GetSize()
{
    return m_msgqueue.size();
}

#include "clientsocket.h"
#ifdef __LINUX
#include <errno.h>
#endif

int CClientSocket::ConnectSocket()
{
    int nRet = CSocket::CreateSocket();
    if( CSocket::CreateSocket() < 0 )
    {
        return CSOCKET_FAIL;
    }

    nRet = connect( m_sock, (sockaddr*)&m_sockaddr, sizeof(m_sockaddr) );

    if( nRet == SOCKET_ERROR)
    {
        return CSOCKET_FAIL;
    }

    return CSOCKET_SUCC;

}
int CClientSocket::RecvMsg(char* szMsg, int nLen)
{
    int nMsgLen = 0;
    int nTmp = 0;

    for( ; nMsgLen < nLen ; )
    {
        nMsgLen = rcv( m_sock, szMsg+nTmp, nLen );

        if( nMsgLen <= 0 )
        {
#ifndef __LINUX
            if( GetLastError() == WSAETIMEDOUT )
#else
            if( errno == ETIMEDOUT )
#endif
            {
                return CSOCKET_CONTINUE;
            }
            else
            {
                return CSOCKET_FAIL;
            }
        }
        else if( nMsgLen < nLen)
        {
            nLen -= nMsgLen;
            nTmp += nMsgLen;
            nMsgLen = 0;
        }
    }

    return CSOCKET_SUCC;

}
int CClientSocket::SendMsg(char* szMsg, int nLen)
{
    int nMsgLen = 0;
    int nTmp = 0;

    for( ; nMsgLen < nLen ; )
    {
        nMsgLen = snd( m_sock, szMsg+nTmp, nLen );
        if( nMsgLen == SOCKET_ERROR )
        {
            return CSOCKET_FAIL;
        }
        else
        {
            nLen -= nMsgLen;
            nTmp += nMsgLen;
            nMsgLen = 0;
        }

    }
    return CSOCKET_SUCC;
}

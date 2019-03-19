#include "socket.h"

#ifdef __LINUX
#include <string.h>
#endif


CSocket::CSocket()
{
    m_nSndTimeout=-1;
    m_nRcvTimeout=-1;
}
CSocket::~CSocket()
{
    CloseSocket();
}
void CSocket::SetSocket( std::string strIp, int nPort )
{    
    m_strIp = strIp;
    m_nPort = nPort;
}
void CSocket::SetSocketEx( std::string strIp, int nPort, int rcvTimeout, int sndTimeout)
{    
    m_strIp = strIp;
    m_nPort = nPort;
    m_nSndTimeout = sndTimeout;
    m_nRcvTimeout = rcvTimeout;
}

int CSocket::CreateSocket()
{
    m_sock = socket( AF_INET, SOCK_STREAM, 0);
    if( m_sock == INVALID_SOCKET)
    {
        return CSOCKET_FAIL;
    }

    if( m_nRcvTimeout > 0 )
        setsockopt(m_sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&m_nRcvTimeout, sizeof(m_nRcvTimeout));
    if( m_nSndTimeout > 0 )
        setsockopt(m_sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&m_nSndTimeout, sizeof(m_nSndTimeout));

    memset(&m_sockaddr, 0x00, sizeof(m_sockaddr));

    m_sockaddr.sin_family = AF_INET;
    m_sockaddr.sin_addr.s_addr = inet_addr( m_strIp.c_str());
    m_sockaddr.sin_port = htons(m_nPort);

    return CSOCKET_SUCC;

}

int CSocket::snd(SOCKET cltsock, char* szMsg, int nLen)
{
    return send( cltsock, szMsg, nLen, 0 );
}

int CSocket::rcv(SOCKET sock, char* szMsg, int nLen)
{
    return recv( sock, szMsg, nLen, 0 );
}

SOCKET CSocket::GetSock()
{
    return m_sock;
}

void CSocket::CloseSocket()
{
    closesocket( m_sock );
}

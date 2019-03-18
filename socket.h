#ifndef SOCKET_H
#define SOCKET_H


#include <string>
#ifdef __LINUX
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#define SOCKET  int
#define SD_BOTH SHUT_RDWR
#define INVALID_SOCKET  -1
#define SOCKET_ERROR    -1
#define GetLastError()  errno
#define closesocket     close
#else
#include <winsock2.h>
#endif

#define CSOCKET_SUCC         0
#define CSOCKET_FAIL        -1
#define CSOCKET_CONTINUE    -2


class CSocket
{
protected:
    SOCKET m_sock;
    struct sockaddr_in m_sockaddr;
    std::string m_strIp;
    int m_nPort;
    int m_nSndTimeout;
    int m_nRcvTimeout;
    int snd(SOCKET cltsock, char* szMsg, int nLen);
    int rcv(SOCKET sock, char* szMsg, int nLen);
public:
    CSocket();
    ~CSocket();
    void SetSocket( std::string strIp, int nPort);
    void SetSocketEx( std::string strIp, int nPort, int rcvTimeout, int sndTimeout);
    int CreateSocket();
    void CloseSocket();

    SOCKET GetSock();

};

#endif // SOCKET_H


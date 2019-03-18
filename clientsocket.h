#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#include "socket.h"

class CClientSocket : public CSocket
{
private:
public:
    int ConnectSocket();
    int RecvMsg(char* szMsg, int nLen);
    int SendMsg(char* szMsg, int nLen);

};

#endif // CLIENTSOCKET_H

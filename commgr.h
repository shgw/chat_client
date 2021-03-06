#ifndef COMMGR_H
#define COMMGR_H

#include <qstring.h>
#include "clientsocket.h"
#include "log.h"
#include "socketmessage.h"
#include "msgmgr.h"

class MainWindow;

class ComMgr
{
private:
    QString m_svrIP;
    CClientSocket m_cltsock;
    QString m_strUserName;
    //MainWindow* m_interface;
    MsgMgr m_MsgMgr;

    bool m_bConnect;
    bool m_bRunThread;
public:
    CLog m_log;

public:
    ComMgr();
    ~ComMgr();
    int ConnectSocket(QString strIP, QString strUserName, char* szMsg);
    int RecvMsg(char* szBuff);
    int SendMsg(ComMsg* msg, int nMsgLen);
    int DisconnectSocket();
    void SetSvrIP(QString strIP);
    void SetUserName(QString strName);

    void SetMsg( ComMsg* msg );
    int GetMsgSize();
    void GetMsg( ComMsg* msg );

    bool GetConnectionState();
    void SetConnectionState( bool bConnect);
    QString GetSvrIP();
    QString GetUserName();

    void SetThreadState( bool bState );
    bool GetThreadState();


#ifndef __LINUX
    static unsigned int ProcThread(void* arg);
#else
    static void* ProcThread(void* arg);
#endif

    void SetLength(char* szSendMsg, char* szMsg, int nLen);
    void MakeHeader( ComMsg* msg, const char* szSrc, const char* szDest, const char* szKind, const char* szId);

};

#endif // COMMGR_H

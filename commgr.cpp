#include "commgr.h"
#include "mainwindow.h"

#define MSG_MAX_BUFF    2048
#define SERVER_PORT     6788

#ifdef __LINUX
#include <unistd.h>
#define Sleep   usleep
#endif

ComMgr::ComMgr()
{

    m_log.SetOption( LOG_LEVEL_DETAIL, "./", "ComMgr.txt");
    m_log.WriteLog(LOG_LEVEL_NOTICE, "==========START ComMgr=============");

}

ComMgr::~ComMgr()
{

}

int ComMgr::ConnectSocket( QString strIP, QString strUserName, char* szMsg )
{
    SetSvrIP( strIP );
    SetUserName( strUserName );


    m_cltsock.SetSocketEx(strIP.toStdString(), SERVER_PORT, 100, 100);

    int nRet = m_cltsock.CreateSocket();

    if( nRet == CSOCKET_SUCC)
    {
        nRet = m_cltsock.ConnectSocket();
        if( nRet == CSOCKET_SUCC )
        {
            ComMsg msg;
            MakeHeader( &msg, strUserName.toStdString().c_str(), SERVER_NAME, MSGKIND_RQST, MSGID_CONN );

            nRet = SendMsg( &msg, HEADER_SIZE );
            if( nRet == CSOCKET_FAIL )
            {
                //
                m_log.WriteLog( LOG_LEVEL_ERROR, "ConnectSocket : send fail error[%d]", GetLastError());
            }
            else
            {
                char szBuff[MSG_MAX_BUFF] = { 0 };

                nRet = CSOCKET_CONTINUE;
                while( nRet == CSOCKET_CONTINUE )
                {
                    nRet = RecvMsg( szBuff );
                    if( nRet == CSOCKET_FAIL )
                    {
                        //
                        m_log.WriteLog( LOG_LEVEL_ERROR, "ConnectSocket : Recv Connect msg fail error[%d]", GetLastError());
                        break;
                    }
                    else if ( nRet == CSOCKET_CONTINUE)
                    {
                        continue;
                    }
                    else
                    {
                        if( szMsg != NULL )
                            memcpy( szMsg, szBuff, nRet);

                        m_log.WriteLog( LOG_LEVEL_NORMAL, "ConnectSocket : Recv Connect msg success");
                        return CSOCKET_SUCC;
                    }
                }

            }
        }
        else
        {
            m_log.WriteLog( LOG_LEVEL_ERROR, "ConnectSocket : Connect fail error[%d]", GetLastError());

        }

    }
    else
    {
        m_log.WriteLog( LOG_LEVEL_ERROR, "ConnectSocket : Create Socket fail error[%d]", GetLastError());
    }


    return CSOCKET_FAIL;
}
int ComMgr::SendMsg(ComMsg* msg, int nMsgLen)
{
    char szSendMsg[MSG_MAX_BUFF] = { 0 };
    SetLength( szSendMsg, (char*)msg, nMsgLen);

    int nRet = m_cltsock.SendMsg( (char*)szSendMsg, nMsgLen + MSG_LENGTH );

    if ( nRet == CSOCKET_FAIL )
    {
        m_log.WriteLog(LOG_LEVEL_ERROR, "SendMsg : send fail error[%d] ", GetLastError() );
        m_log.WriteHEX(LOG_LEVEL_DETAIL, (char*)szSendMsg, nMsgLen + MSG_LENGTH );
        return CSOCKET_FAIL;
    }
    else
    {
        m_log.WriteLog(LOG_LEVEL_NORMAL, "SendMsg : send success " );
        m_log.WriteHEX(LOG_LEVEL_DETAIL, (char*)szSendMsg, nMsgLen + MSG_LENGTH );
        return CSOCKET_SUCC;
    }



}

int ComMgr::RecvMsg(char* szBuff)
{
    int nRet = m_cltsock.RecvMsg( szBuff, MSG_LENGTH );

    int nSize = 0;

    if( nRet == CSOCKET_SUCC)
    {
        nSize = atoi( szBuff );

        if( nSize > 0 )
        {
            nRet = m_cltsock.RecvMsg( szBuff, nSize );
            if ( nRet == CSOCKET_FAIL )
            {
                m_log.WriteLog( LOG_LEVEL_ERROR, "RecvMsg : recv fail error[%d] ", GetLastError());
            }
            else
            {
                m_log.WriteLog( LOG_LEVEL_NORMAL, "RecvMsg : recv success");
                m_log.WriteHEX( LOG_LEVEL_DETAIL, szBuff, nSize);
                return nSize;
            }
        }
        else
        {
            m_log.WriteLog( LOG_LEVEL_ERROR, "RecvMsg : wrong length fail size[%d] ", nSize);

        }

    }
    else if ( nRet == CSOCKET_CONTINUE)
    {
        return nRet;
    }
    else
    {
        m_log.WriteLog( LOG_LEVEL_ERROR, "RecvMsg : recv length fail error[%d] ", GetLastError());

    }

    return CSOCKET_FAIL;
}
int ComMgr::DisconnectSocket()
{
    m_log.WriteLog( LOG_LEVEL_NORMAL, "DisconnectSocket : socket close");
    m_cltsock.CloseSocket();
}
void ComMgr::SetInterface( MainWindow* inter)
{
    m_interface = inter;
}



void ComMgr::SetSvrIP(QString strIP)
{
    m_svrIP = strIP;
}
void ComMgr::SetUserName(QString strName)
{
    m_strUserName = strName;
}


void ComMgr::SetLength(char* szSendMsg, char* szMsg, int nLen)
{
    char szSize[MSG_LENGTH + 1] = { 0 };
    sprintf( szSize, "%04d", nLen);
    memcpy( szSendMsg, szSize, MSG_LENGTH );
    memcpy( (szSendMsg + MSG_LENGTH), szMsg, nLen );
}

void ComMgr::MakeHeader( ComMsg* msg, const char* szSrc, const char* szDest, const char* szKind, const char* szId)
{
    memset( msg, 0x00, sizeof( ComMsg ));
    strcpy( msg->src, szSrc );
    strcpy( msg->dest, szDest );
    strcpy( msg->msgkind, szKind);
    strcpy( msg->msgid, szId );
}

QString ComMgr::GetUserName()
{
    return m_strUserName;
}

QString ComMgr::GetSvrIP()
{
    return m_svrIP;
}

MainWindow* ComMgr::GetInterface()
{
    return m_interface;
}

#ifndef __LINUX
unsigned int ComMgr::ProcThread(void* arg)
#else
void* ComMgr::ProcThread(void* arg)
#endif
{
    int nSleepTime = 1;
    ComMgr* mgr = (ComMgr*)arg;

    char szBuff[MSG_MAX_BUFF];
    int nRet;

    bool bConn = true;

    while(1)
    {
        if( bConn == true )
        {
            memset( szBuff, 0x00, sizeof(szBuff) );
            nRet = mgr->RecvMsg( szBuff );
            if( nRet == CSOCKET_CONTINUE )
            {
                Sleep(nSleepTime);
                continue;
            }
            else if (nRet == CSOCKET_FAIL )
            {
                //ÀçÁ¢
                bConn = false;
                mgr->m_log.WriteLog( LOG_LEVEL_ERROR, "ProcThread : recv error [%d]", GetLastError() );
                mgr->DisconnectSocket();

            }
            else
            {
                mgr->SetMsg( (ComMsg*)szBuff );
            }
        }
        else
        {
            mgr->m_log.WriteLog( LOG_LEVEL_NORMAL, "ProcThread : Reconnect" );
            if( mgr->ConnectSocket( mgr->GetSvrIP(), mgr->GetUserName(), NULL ) == CSOCKET_SUCC )
            {
                nSleepTime = 1;
                bConn = true;
                mgr->m_log.WriteLog( LOG_LEVEL_NORMAL, "ProcThread : Reconnect Success." );
            }
            else
            {
                nSleepTime = 5000;
                mgr->m_log.WriteLog( LOG_LEVEL_ERROR, "ProcThread : Reconnect Fail error [%d]", GetLastError() );
            }

        }
        Sleep(nSleepTime);
    }

}


void ComMgr::SetMsg( ComMsg* msg )
{
    m_MsgMgr.Push( *msg );
}


int ComMgr::GetMsgSize()
{
    return m_MsgMgr.GetSize();
}


void ComMgr::GetMsg( ComMsg* msg )
{
    m_MsgMgr.Pop( msg );
}


















#include "uimgr.h"

#include <string>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qlabel.h>

#include "mainwindow.h"


#define LINE_LIMIT  10
#define WORD_LIMIT  42

UIMgr::UIMgr()
{
    m_log.SetOption( LOG_LEVEL_DETAIL, "./", "UIMGR.txt");
    m_log.WriteLog(LOG_LEVEL_NOTICE, "==========START UIMGR=============");

}
UIMgr::~UIMgr()
{
    std::map<QPushButton*, ChatData>::iterator it = m_ChatMap.begin();
    std::map<QPushButton*, ChatData>::iterator end = m_ChatMap.end();

    for ( ; it != end ; )
    {
        delete it->second.namelabel;
        delete it->second.pBtn;
        delete it->second.layout;
        delete it->second.widget;
    }

}

void UIMgr::RecvChat(ComMsg* msg)
{
    ChatData data = { 0 };
    bool bMyMsg = false;

    if( strcmp( msg->msgkind, MSGKIND_RESP) == 0 )
    {
        bMyMsg = true;
    }

    CreateChat( msg, &data );
    CreateChatBox( &data, bMyMsg);

}


void UIMgr::CreateChat(ComMsg* msg, ChatData* data)
{
    data->state = SPREAD_CHAT_BOX;

    if( strcmp( msg->msgkind, MSGKIND_RESP) == 0 )
    {
        strcpy( data->username, msg->dest );
        RESP_SENDMSG* body = (RESP_SENDMSG*)msg->body;
        strcpy( data->chat, body->reason );
    }
    else
    {
        EVT_SENDMSG* body = (EVT_SENDMSG*)msg->body;
        strcpy( data->username, body->username );
        strcpy( data->chat, body->msg );
    }
}

void UIMgr::CreateChatBox( ChatData* data, bool bMyMsg)
{
    QString strUserName = data->username;

    QString strMessage;
    int nState = ConvertMsg( data->chat, true, strMessage );

    data->state = nState;

    QWidget* p1 = new QWidget;
    QVBoxLayout *hl = new QVBoxLayout(p1);

    QLabel* nm = new QLabel(strUserName, p1);
    QPushButton* pb = new QPushButton(p1);

    QFont font;
    font.setFamily("D2Coding");
    pb->setFont(font);
    nm->setFont(font);

    hl->addWidget(nm);
    hl->addWidget(pb);
    hl->setAlignment(Qt::AlignTop);

    if ( bMyMsg == true )
    {
        pb->setFlat(true);

        pb->setStyleSheet("background-color: #fcf978; border-style: outset;Text-align:left;");
        hl->setAlignment(Qt::AlignRight);
    }
    else
    {
        pb->setFlat(true);
        pb->setStyleSheet("background-color: #a4dbff; border-style: outset;Text-align:left;");

        hl->setAlignment(Qt::AlignLeft);

    }


    pb->setText( strMessage );

    data->layout = hl;
    data->widget = p1;
    data->namelabel = nm;
    data->pBtn = pb;

    m_pWin->AddChatBox( p1, data );
    m_ChatMap[pb] = *data;
}


int UIMgr::ConvertMsg(char* msg, bool omit, QString& retMsg)
{    
    QString str = msg;

    int nState = SPREAD_CHAT_BOX;

    int nLen = str.length();

    if( nLen > 10)
    {
        for( int i = 0 ; i < (nLen-1)/LINE_LIMIT ; i++)
        {
            str.insert( ((i+1)*LINE_LIMIT)+(i), "\n");
        }
        if( omit == true )
        {
            nLen = str.size();
            if( nLen > WORD_LIMIT )
            {
                nState = FOLD_CHAT_BOX;
                str.remove(WORD_LIMIT, nLen - WORD_LIMIT);
                str.insert(WORD_LIMIT,"...");
            }

        }
    }

    str = "\n" + str + "\n";
    retMsg = str;

    return nState;

}
void UIMgr::SetInterface(MainWindow* pInterface)
{
    m_pWin = pInterface;
}


void UIMgr::ConnClient(ComMsg* msg)
{
    EVT_CONN* body = (EVT_CONN*)msg->body;
    char szBuff[1024] = { 0 };
    sprintf( szBuff, " 님이 접속하셨습니다.");

    QWidget* p1 = new QWidget;
    QVBoxLayout *hl = new QVBoxLayout(p1);

    //QString str = QString::fromLocal8Bit(szBuff);
    QString str = szBuff;
    str = body->username + str;
    QLabel* pb = new QLabel(str, p1);
    pb->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);

    hl->addWidget(pb);
    hl->setAlignment(Qt::AlignCenter);

    m_pWin->AddMsgBox( p1 );

}

void UIMgr::DisconClient(ComMsg* msg)
{
    EVT_CONN* body = (EVT_CONN*)msg->body;
    char szBuff[1024] = { 0 };
    sprintf( szBuff, " 님이 나가셨습니다.");

    QWidget* p1 = new QWidget;
    QVBoxLayout *hl = new QVBoxLayout(p1);    
    //QString str = QString::fromLocal8Bit(szBuff);
    QString str = szBuff;
    str = body->username + str;
    QLabel* pb = new QLabel(str, p1);
    pb->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);


    hl->addWidget(pb);
    hl->setAlignment(Qt::AlignCenter);

    m_pWin->AddMsgBox( p1 );

}


ChatData* UIMgr::FindChat(QPushButton* pBtn )
{
    std::map<QPushButton*,ChatData>::iterator iter;

    iter = m_ChatMap.find( pBtn );

    if( iter == m_ChatMap.end() )
    {
        return NULL;
    }

    return &(iter->second);


}

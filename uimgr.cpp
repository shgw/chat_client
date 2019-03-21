#include "uimgr.h"

#include <string>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qlabel.h>

#include "mainwindow.h"



#define LINE_LIMIT  10
#define WORD_LIMIT  43

UIMgr::UIMgr()
{
}
UIMgr::~UIMgr()
{
    DeleteConnectionGUIData();
    DeleteChatData();
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
    font.setFamily(FONT_FAMILY);
    nm->setFont(font);
    font.setPointSize(10);
    pb->setFont(font);

    hl->addWidget(nm);
    hl->addWidget(pb);
    hl->setAlignment(Qt::AlignTop);

    if ( bMyMsg == true )
    {
        nm->setAlignment(Qt::AlignRight);
        pb->setFlat(true);
        pb->setStyleSheet("border-image:url(:/mychat.png);Text-align:left;padding:10px;");
        hl->setAlignment(Qt::AlignRight);
    }
    else
    {
        pb->setFlat(true);
        pb->setStyleSheet("border-image:url(:/yourchat.png);Text-align:left;padding:10px;");
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
    char szNL[] = "\n";
    int nNLlen = strlen(szNL);
    QString str = msg;

    int nState = SPREAD_CHAT_BOX;

    int nLen = str.length();

    if( nLen > LINE_LIMIT )
    {
        for( int i = 0 ; i < (nLen-1)/LINE_LIMIT ; i++)
        {
            str.insert( ((i+1)*LINE_LIMIT)+(i*nNLlen), szNL);
        }
        if( omit == true )
        {
            nLen = str.size();
            if( nLen > WORD_LIMIT )
            {
                nState = FOLD_CHAT_BOX;
                str.remove(WORD_LIMIT, nLen - WORD_LIMIT);
                str.insert(WORD_LIMIT,"\n...");
            }

        }
    }

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

    QFont font;
    font.setFamily(FONT_FAMILY);
    font.setBold(true);
    font.setPointSize(10);

    pb->setFont(font);

    hl->addWidget(pb);
    hl->setAlignment(Qt::AlignCenter);

    m_pWin->AddMsgBox( p1 );

    SetConnectionGUIData( p1, hl, pb );

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


    QFont font;
    font.setFamily(FONT_FAMILY);
    font.setBold(true);
    font.setPointSize(10);

    pb->setFont(font);

    hl->addWidget(pb);
    hl->setAlignment(Qt::AlignCenter);

    m_pWin->AddMsgBox( p1 );

    SetConnectionGUIData( p1, hl, pb );

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
void UIMgr::DeleteChatData()
{
    std::map<QPushButton*, ChatData>::iterator it = m_ChatMap.begin();
    std::map<QPushButton*, ChatData>::iterator end = m_ChatMap.end();

    for ( ; it != end ; )
    {
        delete it->second.namelabel;
        delete it->second.pBtn;
        delete it->second.layout;
        delete it->second.widget;
        m_ChatMap.erase( it );
        it = m_ChatMap.begin();

    }


}

void UIMgr::SetConnectionGUIData( QWidget* widget, QVBoxLayout* layout, QLabel* label )
{
    ConUIData data = { 0 };
    data.widget = widget;
    data.layout = layout;
    data.label = label;

    m_lstConnectionGUIData.push_back(data);
}
void UIMgr::DeleteConnectionGUIData()
{
    std::list<ConUIData>::iterator it = m_lstConnectionGUIData.begin();
    std::list<ConUIData>::iterator end = m_lstConnectionGUIData.end();

    for( ; it != end ; )
    {
        delete it->label;
        delete it->layout;
        delete it->widget;
        m_lstConnectionGUIData.pop_front();
        it = m_lstConnectionGUIData.begin();
    }
}

void UIMgr::SaveMessage()
{
    std::map<QPushButton*, ChatData>::iterator it = m_ChatMap.begin();
    std::map<QPushButton*, ChatData>::iterator end = m_ChatMap.end();

    std::string str = "";

    for( ; it != end ; it++ )
    {
        str += it->second.username;
        str += " : ";
        str += it->second.chat;
        str += "\n";
    }

    FILE* f;

    time_t cur_time;
    struct tm* cur_tm;
    cur_time = time(NULL);
    cur_tm = localtime( &cur_time );
    char fname[128] = { 0 };
    sprintf( fname, "Chat%04d%02d%02d%02d%02d%02d.txt ",
             cur_tm->tm_year+1900, cur_tm->tm_mon+1, cur_tm->tm_mday, cur_tm->tm_hour, cur_tm->tm_min, cur_tm->tm_sec);

    f = fopen( fname, "a+");
    fwrite( str.c_str(), sizeof(char), str.length(), f);
    fclose(f);




}


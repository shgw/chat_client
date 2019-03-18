#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qmessagebox.h>
#include "socketmessage.h"
#include <qscrollbar.h>
#include <qsettings.h>
#include <qlayout.h>

#define TIMER           100
#define MSG_MAX_BUFF    2048
#define INI_FILENAME    "./ChatClient.ini"

#define LIMIT_USERNAME  20

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_bScroll = false;

    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);

    m_ComMgr.SetInterface( this );

    QSettings readini( INI_FILENAME, QSettings::IniFormat );

    m_strUserName = readini.value( "USER/DEFAULT_NAME").toString();
    m_strSvrIP = readini.value("NETWORK/IP").toString();



    ui->lineEdit->setText( m_strUserName);

    m_log = new CLog();
    m_log->SetOption( LOG_LEVEL_NORMAL, "./", "MainWindow.txt");
    m_log->WriteLog(LOG_LEVEL_NOTICE, "==========START MAINWINDOW=============");

    m_central = new QWidget;
    m_layout = new QVBoxLayout(m_central);
    m_layout->setAlignment(Qt::AlignTop);
    ui->scrollArea->setWidget(m_central);
    ui->scrollArea->setWidgetResizable(true);
    ui->scrollArea->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );

    m_UIMgr.SetInterface( this );
    m_timer = new QTimer();
    connect( m_timer, SIGNAL(timeout()), this, SLOT(OnTimer()));

    ui->sendBtn->setShortcut(Qt::Key_Return);
    ui->sendBtn->setShortcut(Qt::Key_Enter);
}

MainWindow::~MainWindow()
{
    delete m_log;
    delete ui;
}


void MainWindow::on_settingBtn_clicked()
{
    m_setDlg.SetParents(this);
    m_setDlg.show();
}

void MainWindow::on_connectBtn_clicked()
{
    if( m_strSvrIP.length() <= 0)
    {
        QMessageBox box;
        box.information(this, "Error", QString::fromLocal8Bit("IP�� �Է����ּ���."));
        return;
    }

    m_strUserName = ui->lineEdit->text();    

    if( m_strUserName.length() <= 0 )
    {
        QMessageBox box;
        box.information(this, "Error", QString::fromLocal8Bit("�̸��� �Է����ּ���."));
        return;
    }

    if(  m_strUserName.toStdString().length() >= 20)
    {
        QMessageBox box;
        box.information(this, "Error", QString::fromLocal8Bit("�̸��� �ʹ� ��ϴ�.(�ѱ� 6�� ����)"));
        return;
    }

    char szBuff[MSG_MAX_BUFF] = { 0 };

    if ( m_ComMgr.ConnectSocket( m_strSvrIP, m_strUserName, szBuff ) == CSOCKET_SUCC )
    {
        ComMsg* msg = (ComMsg*)szBuff;

        BODY_RESP_CONN* body = (BODY_RESP_CONN*)msg->body;

        if( strcmp( body->result, MSG_RESULT_FAIL ) == 0 )
        {
            QMessageBox box;
            box.information(this, QString::fromLocal8Bit("���ӽ���"), QString::fromLocal8Bit(body->reason));
        }
        else
        {
            _beginthreadex( NULL, 0, m_ComMgr.ProcThread, (void*)&m_ComMgr, 0, NULL );
            m_timer->start( TIMER );
            m_log->WriteLog( LOG_LEVEL_NORMAL, "on_connectBtn_clicked : ���� ���� IP[%s] ID[%s]",
                            m_strSvrIP.toStdString().c_str(),
                            m_strUserName.toStdString().c_str());

            QWidget* p1 = new QWidget;
            QVBoxLayout *hl = new QVBoxLayout(p1);

            QLabel* pb = new QLabel(QString::fromLocal8Bit("�����ϼ̽��ϴ�."), p1);
            pb->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);

            hl->addWidget(pb);
            hl->setAlignment(Qt::AlignCenter);
            m_layout->addWidget(p1);

        }


    }
    else
    {
        QMessageBox box;
        box.information(this, "Error", QString::fromLocal8Bit("�������� ���߽��ϴ�."));
    }


}

void MainWindow::on_sendBtn_clicked()
{
    QString strMsg = ui->chatline->text();
    std::string str = strMsg.toStdString();

    ComMsg msg = { 0 };
    m_ComMgr.MakeHeader( &msg, m_strUserName.toStdString().c_str(), SERVER_NAME, MSGKIND_RQST, MSGID_SENDMSG );


    strcpy( msg.body, str.c_str());

    int nlength = str.length();
    if ( m_ComMgr.SendMsg( &msg, HEADER_SIZE + nlength ) == CSOCKET_FAIL )
    {
        m_log->WriteLog( LOG_LEVEL_ERROR, "on_sendBtn_clicked : Send Error");
        QMessageBox box;
        box.information( this, "Error", QString::fromLocal8Bit("�������� ���߽��ϴ�."));
    }
    else
    {
        m_log->WriteLog( LOG_LEVEL_NORMAL, "on_sendBtn_clicked : Send Success");
        ui->chatline->setText("");
    }
}

void MainWindow::SetServerIP( QString strIP )
{
    m_strSvrIP = strIP;
}

void MainWindow::SetUserName()
{
    m_strUserName = ui->lineEdit->text();
}

void MainWindow::ProcRecv( ComMsg* msg)
{
    m_log->WriteLog( LOG_LEVEL_NORMAL, "ProcRecv : Recv src[%s] dest[%s] msgkind [%s] msgid[%s]", msg->src, msg->dest, msg->msgkind, msg->msgid);

    if( strcmp( msg->msgkind, MSGKIND_RESP ) == 0 )
    {
        ProcResp( msg );
    }
    else if (strcmp ( msg->msgkind, MSGKIND_EVT) == 0 )
    {
        ProcEvt( msg );
    }
    else
    {
        m_log->WriteLog( LOG_LEVEL_WARN, "Unknown msgkind [%s]", msg->msgkind);
    }
}


void MainWindow::ProcResp( ComMsg* msg)
{
    if( strcmp( msg->msgid, MSGID_SENDMSG ) == 0 )
    {
        //ui->lineEdit->setText(msg->msgid);
        m_UIMgr.RecvChat( msg );
    }
    else
    {
        m_log->WriteLog( LOG_LEVEL_WARN, "ProcResp : Unknown msg id [%s]", msg->msgid);
    }

}


void MainWindow::ProcEvt( ComMsg* msg)
{
    if( strcmp( msg->msgid, MSGID_CONN ) == 0 )
    {
        m_UIMgr.ConnClient( msg );
    }
    else if( strcmp( msg->msgid, MSGID_DISCON ) == 0 )
    {
        m_UIMgr.DisconClient( msg );
    }
    else if( strcmp( msg->msgid, MSGID_SENDMSG ) == 0 )
    {
        m_UIMgr.RecvChat( msg );
    }
    else
    {
        m_log->WriteLog( LOG_LEVEL_WARN, "Unknown msgk id [%s]", msg->msgid);
    }

}


void MainWindow::AddChatBox(QWidget* pb, ChatData* data)
{

    m_layout->addWidget(pb);
    m_bScroll = true;

    if( data->state < 0 )
    {
        connect(data->pBtn, SIGNAL(clicked()), this, SLOT(ProcLongMsg()));
    }
}

void MainWindow::AddMsgBox(QWidget* pb )
{

    m_layout->addWidget(pb);
    m_bScroll = true;
}



void MainWindow::OnTimer()
{
    if ( m_bScroll == true )
    {
        ui->scrollArea->verticalScrollBar()->setValue(ui->scrollArea->verticalScrollBar()->maximum());
        m_bScroll = false;
    }

    if( m_ComMgr.GetMsgSize() > 0 )
    {
        ComMsg msg = { 0 };
        m_ComMgr.GetMsg(&msg);
        ProcRecv( &msg );
    }

}

void MainWindow::ProcLongMsg()
{
    QPushButton* pButton = qobject_cast<QPushButton*>(sender());

    ChatData* data = m_UIMgr.FindChat(pButton);

    if( data == NULL )
    {
        m_log->WriteLog( LOG_LEVEL_ERROR , "cant find btn");
        return;
    }

    if( data->state > 0 )
    {
        FoldLongMsg( data );
    }
    else
    {
        SpreadLongMsg(data);
    }

}



void MainWindow::SpreadLongMsg(ChatData* data)
{
    QString str;
    m_UIMgr.ConvertMsg( data->chat, false, str);
    data->pBtn->setText( str );
    data->state *= -1;

}
void MainWindow::FoldLongMsg(ChatData* data)
{

    QString str;
    m_UIMgr.ConvertMsg( data->chat, true, str);
    data->pBtn->setText( str );
    data->state *= -1;

}





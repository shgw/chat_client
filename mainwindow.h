#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "settingdialog.h"
#include "commgr.h"
#include "uimgr.h"
#include <qtimer.h>



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_settingBtn_clicked();

    void on_connectBtn_clicked();

    void on_sendBtn_clicked();

    void OnTimer();

    void ProcLongMsg();

private:
    Ui::MainWindow *ui;
    SettingDialog m_setDlg;
    CLog* m_log;

    ComMgr m_ComMgr;
    UIMgr m_UIMgr;

    QString m_strSvrIP;
    QString m_strUserName;

    QWidget* m_central;
    QVBoxLayout* m_layout;

    QTimer* m_timer;
    bool m_bScroll;

    void SpreadLongMsg(ChatData* data);
    void FoldLongMsg(ChatData* data);

public:
    void SetUserName();
    void SetServerIP( QString strIP );
    void ProcRecv( ComMsg* msg);
    void ProcResp( ComMsg* msg);
    void ProcEvt( ComMsg* msg);
    void AddChatBox(QWidget* pb, ChatData* data);
    void AddMsgBox(QWidget* pb );

};

#endif // MAINWINDOW_H

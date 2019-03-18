#ifndef UIMGR_H
#define UIMGR_H


#include <map>
#include "socketmessage.h"
#include "log.h"
#include <qstring.h>


#define FOLD_CHAT_BOX       -1
#define SPREAD_CHAT_BOX     1


class QVBoxLayout;
class QWidget;
class QLabel;
class MainWindow;
class QPushButton;

typedef struct _ST_CHAT_DATA
{
    int state;
    char username[20];
    char chat[1024];
    QPushButton* pBtn;
    QWidget* widget;
    QVBoxLayout *layout;
    QLabel* namelabel;

}ChatData;

class UIMgr
{
private:


    MainWindow* m_pWin;
    std::map<QPushButton*, ChatData> m_ChatMap;

    CLog m_log;

public:
    UIMgr();
    ~UIMgr();
    void CreateChat(ComMsg* msg, ChatData* data);
    void CreateChatBox( ChatData* data, bool bMyMsg);
    void RecvChat(ComMsg* msg);
    int ConvertMsg(char* msg, bool omit, QString& retMsg);
    void ConnClient(ComMsg* msg);
    void DisconClient(ComMsg* msg);
    ChatData* FindChat(QPushButton* pBtn);
    void SetInterface(MainWindow* pInterface);
};

#endif // UIMGR_H

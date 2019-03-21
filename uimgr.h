#ifndef UIMGR_H
#define UIMGR_H


#include <map>
#include <list>
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
    char username[SOURCE_LENGTH];
    char chat[CHAT_LENGTH];
    QPushButton* pBtn;
    QWidget* widget;
    QVBoxLayout *layout;
    QLabel* namelabel;

}ChatData;

typedef struct _ST_CONNECTION_GUI_DATA
{
    QWidget* widget;
    QVBoxLayout* layout;
    QLabel* label;
}ConUIData;

class UIMgr
{
private:


    MainWindow* m_pWin;
    std::map<QPushButton*, ChatData> m_ChatMap;

    std::list<ConUIData> m_lstConnectionGUIData;

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
    void DeleteChatData();
    void SetConnectionGUIData( QWidget* widget, QVBoxLayout* layout, QLabel* label );
    void DeleteConnectionGUIData();
    void SaveMessage();
};

#endif // UIMGR_H

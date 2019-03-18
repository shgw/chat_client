#ifndef CHATDATA_H
#define CHATDATA_H

typedef struct _ST_CHAT_DATA
{
    int state;
    char username[20];
    char chat[1024];

}ChatData;

#endif // CHATDATA_H

#ifndef SOCKETMESSAGE_H
#define SOCKETMESSAGE_H

#define MSG_LENGTH      4
#define SOURCE_LENGTH   20
#define DEST_LENGTH     20
#define MSGKIND_LENGTH  5
#define MSGID_LENGTH    8
#define BODY_LENGTH     2048

#define RESULT_LENGTH   5
#define REASON_LENGTH   1024
#define CHAT_LENGTH   1024


#define SERVER_NAME     "SERVER"

#define MSGKIND_RQST    "RQST"
#define MSGKIND_RESP    "RESP"
#define MSGKIND_EVT     "EVT"

#define MSGID_CONN      "CONN"
#define MSGID_DISCON    "DISCON"
#define MSGID_SENDMSG   "SENDMSG"

#define MSG_RESULT_SUCC "SUCC"
#define MSG_RESULT_FAIL "FAIL"

#define MSG_REASON_SN   "SAME_NAME"
#define MSG_REASON_NR   "NONREG"

#define HEADER_SIZE     53


typedef struct _ST_COM_MSG
{
    char src[SOURCE_LENGTH];
    char dest[DEST_LENGTH];
    char msgkind[MSGKIND_LENGTH];
    char msgid[MSGID_LENGTH];
    char body[BODY_LENGTH];
}ComMsg;

typedef struct _ST_BODY_RESP_CONN
{
    char result[RESULT_LENGTH];
    char reason[REASON_LENGTH];

}BODY_RESP_CONN;

typedef struct _ST_EVT_CONN
{
    char username[SOURCE_LENGTH];
}EVT_CONN;

typedef struct _ST_EVT_DICON
{
    char username[SOURCE_LENGTH];
}EVT_DICON;

typedef struct _ST_RQST_SENDMSG
{
    char msg[CHAT_LENGTH];
}RQST_SENDMSG;

typedef struct _ST_RESP_SENDMSG
{
    char result[RESULT_LENGTH];
    char reason[REASON_LENGTH];
}RESP_SENDMSG;

typedef struct _ST_EVT_SENDMSG
{
    char username[SOURCE_LENGTH];
    char msg[CHAT_LENGTH];
}EVT_SENDMSG;




#endif // SOCKETMESSAGE_H

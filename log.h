#ifndef LOG_H
#define LOG_H

#define LOG_LEVEL_NOTICE    0
#define LOG_LEVEL_FATAL     1
#define LOG_LEVEL_ERROR     100
#define LOG_LEVEL_WARN      500
#define LOG_LEVEL_NORMAL    1000
#define LOG_LEVEL_DETAIL    10000

class CLog
{
private:
    int m_level;
    char m_path[256];
    char m_fileName[256];
    char m_fullPath[512];    

public:
    CLog();
    ~CLog();
    void SetOption(int nLevel, char* path, char* fileName);
    int WriteLog( int nLevel, char* log, ...);
    int WriteHEX( int nLevel, char* log, int nLen);

private:
    int GetLogLevelString( int nLevel, char* buf);



};

#endif // LOG_H


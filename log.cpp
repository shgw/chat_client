#include "log.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string>
#ifndef __LINUX
#include <windows.h>
#else
#include <sys/time.h>
#endif




CLog::CLog()
{

}
CLog::~CLog()
{

}

void CLog::SetOption(int nLevel, char* path, char* fileName)
{
    m_level = nLevel;
    strcpy( m_path , path);
    strcpy( m_fileName , fileName);

    sprintf( m_fullPath, "%s/%s", m_path, m_fileName);

}

int CLog::WriteLog( int nLevel, char* log, ...)
{
    if( nLevel > m_level)   return 0;

    char buf[1024] = { 0 };

    int nStartPos = GetLogLevelString( nLevel, buf);

    va_list ap;
    va_start(ap, log);
    vsprintf(buf + nStartPos, log, ap);
    va_end(ap);

    //file
    FILE* pFile = NULL;

    pFile = fopen( m_fullPath, "a+");
    if( pFile == NULL ) return -1;

    int nRet = fwrite( buf, sizeof(char), strlen(buf), pFile);
    if ( nRet < strlen(buf) )
    {
        fclose( pFile );
        return -1;
    }
    fwrite( "\n", 1, 1, pFile);


    fclose( pFile );


    return nRet;


}

int CLog::WriteHEX( int nLevel, char* log, int nLen)
{
    if( nLevel > m_level)   return 0;
    char buf[64] = { 0 };
    char tmp[128] = { 0 };
    std::string strLog;

    const int nRow = 10;
    const int nBlank = 10;
    const int nHex = 3;
    int nIdx = 0;
    int npos = 0;

    GetLogLevelString( nLevel, buf);
    strLog = buf;
    strLog += "\n";

    for( int i = 0 ; i < nLen / nRow ; i ++)
    {
        npos = 0;
        memset( tmp, ' ', sizeof(tmp) );
        for(int j = 0 ; j < nRow ; j++ )
        {
            nIdx = i*10+j;

            if( log[nIdx] == 0)
                tmp[npos] = '.';
            else
                tmp[npos] = log[nIdx];

            sprintf( buf, "%02X ", (unsigned char)log[nIdx]);
            strcpy( tmp+(nRow+nBlank+(npos*nHex)), buf );

           ++npos;
        }
        strLog += tmp;
        strLog += "\n";
    }

    if(nIdx > 0 )nIdx++;

    npos = 0;
    memset( tmp, ' ', sizeof(tmp) );

    for( int i = 0 ; i < nLen % nRow ; i++ )
    {

        if( log[nIdx] == 0)
            tmp[npos] = '.';
        else
            tmp[npos] = log[nIdx];

        sprintf( buf, "%02X ", (unsigned char)log[nIdx]);
        strcpy( tmp+(nRow+nBlank+(npos*nHex)), buf );

       ++npos;
       ++nIdx;

    }
    strLog += tmp;
    strLog += "\n";

    //file
    FILE* pFile = NULL;

    pFile = fopen( m_fullPath, "a+");
    if( pFile == NULL ) return -1;

    int nRet = fwrite( strLog.c_str(), sizeof(char), strLog.size(), pFile);
    if ( nRet < strLog.size() )
    {
        fclose( pFile );
        return -1;
    }


    fclose( pFile );


    return nRet;



}

int CLog::GetLogLevelString( int nLevel, char* buf)
{
    if ( buf == NULL )  return 0;

    char* pLevel;
    char detail[] = "DETAIL";
    char normal[] = "NORMAL";
    char warn[] = "WARNING";
    char error[] = "ERROR";

    if ( nLevel >= LOG_LEVEL_DETAIL)
    {
       pLevel = detail;
    }
    else if ( nLevel >= LOG_LEVEL_NORMAL )
    {
        pLevel = normal;
    }
    else if ( nLevel >= LOG_LEVEL_WARN )
    {
        pLevel = warn;
    }
    else
    {
        pLevel = error;
    }
    time_t cur_time;
    struct tm* cur_tm;

    cur_time = time(NULL);

    cur_tm = localtime( &cur_time );
    int nMilSec;
#ifndef __LINUX
    SYSTEMTIME t;
    GetLocalTime(&t);
    nMilSec = t.wMilliseconds;
#else
    struct timeval tv = { 0 };
    gettimeofday(&tv, 0);
    nMilSec = tv.tv_usec;
#endif

    sprintf( buf, "[%-7s %04d/%02d/%02d %02d:%02d:%02d.%03d] ",
             pLevel, cur_tm->tm_year+1900, cur_tm->tm_mon+1, cur_tm->tm_mday, cur_tm->tm_hour, cur_tm->tm_min, cur_tm->tm_sec, nMilSec);

    return strlen(buf);
}

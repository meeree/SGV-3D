#include "logger.h"
#include <cstdio>
#include <stdarg.h>
#include <algorithm>
#include <time.h>

unsigned Logger::msMaxRecord = 20000;
unsigned Logger::msMaxDuplicates = msMaxRecord; //FIX THIS
unsigned const k_maxMsgLen = 128;

bool Logger::init (const char* logFileName) 
{
    mLogFile.open(logFileName, std::ios::out | std::ios::trunc);
    return mLogFile.is_open();
}

std::string Logger::codeToString (eSeverity const& code)
{
    switch (code)
    {
        case INFO   : return "INFO"   ;
        case DEBUG  : return "DEBUG"  ;
        case WARNING: return "WARNING";
        case ERROR  : return "ERROR"  ;
        case LETHAL : return "LETHAL" ;
    }
}

Logger& Logger::singleton () 
{
    static Logger logger;
    return logger;
}

bool Logger::log (char const* fileName, char const* func, int const& line, Logger::eSeverity const& code, char const* msg,...)
{
    std::pair<std::string,unsigned> sig{std::make_pair(std::string(fileName),line)};
    auto lookup = mRecord.find(sig);
    unsigned& cnt{mRecord[sig]};
    if (lookup == mRecord.end())
        cnt = 1;
    else 
        if (++cnt > msMaxDuplicates)
            return true;

    char msgCstring[k_maxMsgLen];

    va_list args;
    va_start(args, msg);
    vsprintf(msgCstring, msg, args);
    va_end(args);

    mLogFile<<"[";
    
    mLogFile.width(3); 
    mLogFile<<std::left<<cnt<<"]";

    mLogFile.width(10); 
    mLogFile<<"("+codeToString(code)<<") ";

    mLogFile.width(15);
    mLogFile<<std::string(func)+"()";
    
    mLogFile.width(25);
    mLogFile<<std::string(fileName);
        
    mLogFile.width(5);
    mLogFile<<line;
    
    mLogFile.width(k_maxMsgLen);
    mLogFile<<std::left<<msgCstring<<std::endl<<std::right; 

    if (mLogFile.bad())
        return false;

#if EXIT_ON_ERR 
    if (code > WARNING) 
    {
        msLogFl.close();
        exit(0); //Maybe add exit codes??
    }
#else
    return true;
#endif
}

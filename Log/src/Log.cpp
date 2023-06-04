#include "Log.h"

namespace w{
    const char* ToString(LogLevel::Level level)
    {
        switch (level)
        {
        #define XX(name) \
            case LogLevel::name: \
                return #name; \
                break; 
                XX(DEBUG);
                XX(INFO);
                XX(WARN);
                XX(ERROR);
                XX(FALAT);
        #undef XX
        default:
            return "UNKNOW";
            break;
        }
        return "UNKNOW";
    }

    LogLevel::Level FromString(const std::string& str)
    {
        #define XX(level, v) \
        if(str == #v) \
        {return LogLevel::level;}
        XX(DEBUG, debug);
        XX(INFO, info);
        XX(WARN, warn);
        XX(ERROR, error);
        XX(FALAT, falat);

        XX(DEBUG, DEBUG);
        XX(INFO, INFO);
        XX(WARN, WARN);
        XX(ERROR, ERROR);
        XX(FALAT, FALAT);
        
        #undef XX
        return LogLevel::UNKNOW;
    }



}
/*
 * @Author: w 2652289370@qq.com
 * @Date: 2023-06-06 09:27:36
 * @LastEditors: w 2652289370@qq.com
 * @LastEditTime: 2023-06-06 11:24:04
 * @FilePath: /cilent/Log/src/Log.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
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

    void LogEvent::format(const char* fmt, ...)
    {
        va_list al;
        va_start(al, fmt);
        format(fmt, al);
        va_end(al);
    }
    void LogEvent::format(const char* fmt, va_list al)
    {
        char* buf = nullptr;
        int len = vasprintf(&buf, fmt, al);
        if (len != -1)
        {
            m_ss << std::string(buf, len);
            free(buf);
        }
        
    }


    class MessageFormatItem : public LogFormatter::LogFormatItem
    {
    private:
        /* data */
    public:
        MessageFormatItem(const std::string& str = ""){}
        ~MessageFormatItem(){}
        void format(std::ostream& os, Logger::Ptr logger, LogLevel::Level level, LogEvent::Ptr event) override{
            os << event->getContent();
        }
    };

    class LevelFormatItem : public LogFormatter::LogFormatItem
    {
    public:
        LevelFormatItem(const std::string& str = ""){}
        ~LevelFormatItem(){}
        void format(std::ostream& os, Logger::Ptr logger, LogLevel::Level level, LogEvent::Ptr event) override{
            os << LogLevel::ToString(level);
        }
    };
    
    


    LogFormatter::LogFormatter(const std::string& pattern):m_pattern(pattern)
    {
        init();
    }
    
    LogFormatter::~LogFormatter()
    {
    }

    void LogFormatter::init()
    {
        std::vector<std::tuple<std::string, std::string, int>> vec;
        std::string nstr;
        for (size_t i = 0; i < m_pattern.size(); i++)
        {
            if (m_pattern[i] != '%')
            {
                nstr.append(1, m_pattern[i]);
                continue;
            }
            if ((i + 1) < m_pattern.size())
            {
                if(m_pattern[i + 1] == '%')
                {
                    nstr.append(1, '%');
                    continue;
                }
            }

            if
            
            
        }
        
    }

       
    Logger::Logger(/* args */)
    {
    }
    
    Logger::~Logger()
    {
    }

}
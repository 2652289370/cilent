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
                if(m_pattern[i + 1] == '%' || !isalpha(m_pattern[i + 1]))
                {
                    nstr.append(1, '%');
                    continue;
                }
                 
            }
            if(i == m_pattern.size() - 1) 
            {
                nstr.append(1, m_pattern[i]);
                continue;
            }


            size_t n = i + 2;

            bool have_args = false;

            std::string fmt;

            while (n < m_pattern.size())
            {
                if (m_pattern[i + 2] != '{' && !have_args)
                {
                    break;
                }
                else
                {
                    have_args = true;
                }
                if (m_pattern[n] == '}' && have_args)
                {
                    fmt = m_pattern.substr(i + 3, n - i - 3);
                    break;
                }
                n++;
            }

            std::string str;
            str.append(1, m_pattern[i + 1]);

            if (!nstr.empty())
            {
                vec.push_back(std::make_tuple(nstr, std::string(), 0));
                vec.push_back(std::make_tuple(str, fmt, 1));
            }
            else
            {
                vec.push_back(std::make_tuple(str, fmt, 1));
            }
            i += 1 + fmt.size();
            if (fmt.size() > 0)
            {
                i += 2;
            }    
            nstr.clear();
        }
        if (!nstr.empty())
        {
            vec.push_back(std::make_tuple(nstr, std::string(), 0));
        }


        std::map<std::string, std::function<LogFormatItem::Ptr(const std::string& str)>> s_format_items = {
            #define XX(str, C) \
            {#str, [](const std::string& fmt){return LogFormatItem::Ptr(new C(fmt)); }}

            XX(m, MessageFormatItem),

            #undef XX
        };

        for (auto& i : vec)
        {
            std::cout << std::get<0>(i) << std::endl;
            // std::cout << std::get<1>(i) << std::endl;
        }
        
        
    }

       
    Logger::Logger(/* args */)
    {
    }
    
    Logger::~Logger()
    {
    }

}
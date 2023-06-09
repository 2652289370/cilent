/*
 * @Author: w 2652289370@qq.com
 * @Date: 2023-06-06 09:27:36
 * @LastEditors: w 2652289370@qq.com
 * @LastEditTime: 2023-06-09 17:10:56
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

    class ElapseFormatItem : public LogFormatter::LogFormatItem
    {
    public:
        ElapseFormatItem (const std::string& str = ""){}
        ~ElapseFormatItem (){}
        void format(std::ostream& os, Logger::Ptr logger, LogLevel::Level level, LogEvent::Ptr event) override{
            os << event->getElapse();
        }
    };
    
    class StringFormatItem: public LogFormatter::LogFormatItem
    {
    public:
        StringFormatItem(const std::string& str):m_string(str)
        {}
        ~StringFormatItem(){}
         void format(std::ostream& os, Logger::Ptr logger, LogLevel::Level level, LogEvent::Ptr event) override{
            os << m_string;
        }
    private:
        std::string m_string;
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
            if (std::get<2>(i) == 1)
            {
                auto it = s_format_items.find(std::get<0>(i));
                if (it == s_format_items.end())
                {
                    m_item.push_back(LogFormatItem::Ptr(new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
                    m_error = true;
                }
                else
                {
                    m_item.push_back(it->second(std::get<1>(i)));
                }
                
            }
            else
            {
                m_item.push_back(LogFormatItem::Ptr(new StringFormatItem(std::get<0>(i))));
            }
            
        }
        
        
    }

    std::string LogFormatter::format(Logger::Ptr logger, LogLevel::Level level, LogEvent::Ptr event)
    {
        std::stringstream ss;
        for (auto& i : m_item)
        {
           i->format(ss, logger, level, event);
        }
        return ss.str();
    }
    std::ostream& LogFormatter::format(std::ostream& ofs, Logger::Ptr logger, LogLevel::Level level, LogEvent::Ptr event)
    {
        for (auto& i : m_item)
        {
            i->format(ofs, logger, level, event);
        }
        return ofs;
    }

       
    Logger::Logger() 
    : m_level(LogLevel::DEBUG)
    {
        m_formatter.reset(new LogFormatter("%m"));
    }
    
    Logger::~Logger()
    {
    }


    void Logger::log(LogLevel::Level level, std::shared_ptr<LogEvent> event)
    {
        if (m_level <= level)
        {
            auto self = shared_from_this();
            
            if (!m_appenders.empty())
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                for (auto& i : m_appenders)
                {
                    i->log(self, level, event);
                }
            }
            
        }
        
    }

    void Logger::setLogFormat(const std::string& format){
            std::unique_lock<std::mutex> lock(m_mutex);
            m_formatter.reset(new LogFormatter(format));
        }

    void Logger::addLogAppender(std::shared_ptr<LogAppender> appdener)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        appdener->m_formatter = this->m_formatter;
        m_appenders.push_back(appdener);
    }

    void Logger::delAppender(std::shared_ptr<LogAppender> appender) 
    {
        std::unique_lock<std::mutex> lock();
        for(auto it = m_appenders.begin();
                it != m_appenders.end(); ++it) {
            if(*it == appender) {
                m_appenders.erase(it);
                break;
            }
        }
    }



    void StdoutLogAppender::log(Logger::Ptr logger, LogLevel::Level level, LogEvent::Ptr event)
    {   
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_formatter->format(std::cout, logger, level, event);
        }      
    }

}
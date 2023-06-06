#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <memory>
#include <stdarg.h>
#include <vector>

namespace w{
    
    // class LogLevel;
    // class Logger;
    

    /**
     * @brief 日志等级
    */
    class LogLevel
    {
    public:
        typedef std::shared_ptr<LogLevel> Ptr;
        enum Level{
            UNKNOW = 0,
            DEBUG = 1,
            INFO = 2,
            WARN = 3,
            ERROR = 4,
            FALAT = 5
        };
        static const char* ToString(LogLevel::Level level);
        static LogLevel::Level FromString(const std::string& str);
    };

    class LogAppender
    {
    public:
        typedef std::shared_ptr<LogAppender> Ptr;
        LogAppender(/* args */);
        ~LogAppender();
    private:
        LogLevel::Level m_level;
    };
    

    /**
     * @brief 日志管理器
    */
    class Logger
    {
    public:
        typedef std::shared_ptr<Logger> Ptr;
        Logger(/* args */);
        ~Logger();

    private:

    };

    class LogEvent
    {
    public:
        typedef std::shared_ptr<LogEvent> Ptr;
        LogEvent(Logger::Ptr logger, LogLevel::Level level, 
        const char* file, int32_t line, uint32_t elapse, uint32_t threadID,
        uint32_t fiberID, uint64_t time, std::string threadName):
        m_logger(logger), m_level(level),m_file(file), m_line(line),
        m_elapse(elapse), m_threadID(threadID), m_fiberID(fiberID),
        m_time(time), m_threadName(threadName)
        {}
        ~LogEvent(){}
        void format(const char* fmt, ...);
        void format(const char* fmt, va_list al);

        std::string getContent(){
            return m_ss.str();
        }
    private:
        /// @brief 文件名
        const char * m_file = nullptr;
        /// @brief 行号
        int32_t m_line = 0;
        /// @brief 程序启动开始到现在的毫秒数
        uint32_t m_elapse = 0;
        /// @brief 线程ID
        uint32_t m_threadID = 0;
        /// @brief 协程ID
        uint32_t m_fiberID = 0;
        /// @brief 时间戳
        uint64_t m_time = 0;
        /// @brief 线程名称
        std::string m_threadName;
        /// @brief 日志内容流
        std::stringstream m_ss;
        /// @brief 日志器
        Logger::Ptr m_logger;
        /// @brief 日志等级
        LogLevel::Level m_level;
    };

    class Logger
    {
    public:
        typedef std::shared_ptr<Logger> Ptr;
        Logger(/* args */);
        ~Logger();

    };
 
    
    
    class LogFormatter
    {
    public:
        LogFormatter(const std::string& pattern);
        ~LogFormatter();
        
        class LogFormatItem
        {
        public:
            typedef std::shared_ptr<LogFormatItem> Ptr;
            virtual ~LogFormatItem() {}
            virtual void format(std::ostream& os, Logger::Ptr logger, LogLevel::Level level, LogEvent::Ptr event) = 0;
        };
    
    private:
        void init();
        
    private:
        std::string m_pattern;  
        std::vector<LogFormatItem::Ptr> m_item;
    };
    
  
    
    

    

}




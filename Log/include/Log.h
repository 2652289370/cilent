#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <memory>
#include <stdarg.h>
#include <vector>
#include <map>
#include <functional>
#include <mutex>
#include <thread>
#include <list>

namespace w{

    class LogFormatter;
    
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
    

    /**
     * @brief 日志管理器
    */
    class Logger
    {
    public:
        typedef std::shared_ptr<Logger> Ptr;
        Logger(const std::string& name);
        ~Logger();

    private:
        /// @brief 日志器名称
        std::string m_name;
        /// @brief 日志级别
        LogLevel::Level m_level;
        /// @brief 日志格式器
        std::shared_ptr<LogFormatter> m_formatter;
        /// @brief 日志输出地集合
        std::list<std::shared_ptr<LogAppender>> m_appenders;
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

        std::string getElapse()
        {
            return std::to_string(m_elapse);
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
 
    
    /**
    * @brief 日志格式器
    */
    class LogFormatter
    {
    public:
        typedef std::shared_ptr<LogFormatter> Ptr;

        LogFormatter(const std::string& pattern);
        ~LogFormatter();
        

        std::string format(Logger::Ptr logger, LogLevel::Level level, LogEvent::Ptr event);
        std::ostream& format(std::ostream& ofs, Logger::Ptr logger, LogLevel::Level level, LogEvent::Ptr event);

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
        bool m_error = false;
    };
    
  
  class LogAppender
  {
  public:
    LogAppender(/* args */){}
    virtual ~LogAppender(){}

    virtual void log(Logger::Ptr logger, LogLevel::Level level, LogEvent::Ptr) = 0;
    protected:
        /// @brief 互斥锁
        std::mutex m_mutex;
        /// @brief 日志格式
        LogFormatter::Ptr m_formatter;
        /// @brief 日志级别
        LogLevel::Level m_level = LogLevel::DEBUG;
  };


    class StdoutLogAppender : public LogAppender
    {
    public:
        typedef std::shared_ptr<StdoutLogAppender> Ptr;
        void log(Logger::Ptr logger, LogLevel::Level level, LogEvent::Ptr event)override;
    };
    
    

    

}




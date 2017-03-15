#ifndef LOGGER_H
#define LOGGER_H
#include <sstream>
#include <functional>
//logging 代码来自muduo，作者陈硕
//我默写了一遍，记不清，有些地方可能有差别
namespace logging {

class Logger {
public:
    //用来设置不同的日志级别,从muduo拷贝过来的
    enum LogLevel {
        eTRACE,
        eDEBUG,
        eINFO,
        eWARN,
        eERROR,
        eFATAL,
        eNUM_LOG_LEVELS,//级别个数
    };
    typedef std::function<void(std::string)> OutputCallback;
public:
    Logger(const char* filename, int line, const char* func_name, LogLevel level);
    ~Logger();

    std::stringstream& stream();

    static LogLevel log_level();
    static void set_level(LogLevel level);
    static void set_output(OutputCallback cb);
    static void set_no_display_function(bool value) { no_display_function_ = value; }
private:
    std::stringstream sstream_;

private:
    static LogLevel level_limit_;
    static OutputCallback output_callback_;
    static bool no_display_function_;
};


#define LOG_TRACE if (logging::Logger::log_level() <= logging::Logger::eTRACE) \
    logging::Logger(__FILE__, __LINE__, __func__,logging::Logger::eTRACE).stream()

#define LOG_DEBUG if (logging::Logger::log_level() <= logging::Logger::eDEBUG) \
    logging::Logger(__FILE__, __LINE__, __func__, logging::Logger::eDEBUG).stream()

#define LOG_INFO if (logging::Logger::log_level() <= logging::Logger::eINFO) \
    logging::Logger(__FILE__, __LINE__, __func__, logging::Logger::eINFO).stream()

#define LOG_WARN logging::Logger(__FILE__, __LINE__, __func__, logging::Logger::eWARN).stream()

#define LOG_ERROR logging::Logger(__FILE__, __LINE__, __func__, logging::Logger::eERROR).stream()

#define LOG_FATAL logging::Logger(__FILE__, __LINE__, __func__, logging::Logger::eFATAL).stream()



} // namespace
#endif // LOGGER_H

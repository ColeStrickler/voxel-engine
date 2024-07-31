#ifndef LOGGER_H
#define LOGGER_H
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>
#include "profiler.h"
#define LOGFILE "log.txt"

enum LOGLEVEL
{
    LEVEL_INFO,
    LEVEL_WARNING,
    LEVEL_ERROR,
    LEVEL_OFF
};

enum LOGTYPE
{
    INFO,
    WARNING,
    ERROR,
    NONE=4
};


/*

*/
typedef struct LogTarget
{
    std::mutex lock;
    std::condition_variable cv;
    std::vector<std::string> LogQueue;
    /* keep only the last count entries  */
    void ResizeQueue(int count);
}LogTarget;


class Logger
{
public:
    Logger();
    ~Logger();

    void WriteLogs();
    void Log(LOGTYPE type, const std::string& log_msg);
    void SetLogLevel(LOGLEVEL level) {m_LogLevel = level;}
    void RegisterTarget(LogTarget* target);
private:
    void TryDeleteFile(const std::string& file);
    std::string GetLogLabel(LOGTYPE type);
    std::ofstream m_LogFile;
    std::vector<std::string> m_InternalLogBuffer;
    LOGLEVEL m_LogLevel;
    std::string m_LogFileName;
    std::vector<LogTarget*> m_LogTargets;
};

#endif
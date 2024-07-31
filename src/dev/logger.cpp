#include "logger.h"

Logger logger;


Logger::Logger() : m_LogFileName(LOGFILE), m_LogLevel(LOGLEVEL::LEVEL_ERROR)
{
    TryDeleteFile(m_LogFileName);
    m_LogFile = std::ofstream(m_LogFileName, std::ofstream::app);
    if (!m_LogFile.is_open())
        Log(LOGTYPE::WARNING, "Logger::Logger() --> failed to open log file.\n");
}

Logger::~Logger()
{
}

void Logger::WriteLogs()
{
    EMIT_PROFILE_TOKEN
    bool written_outfile = false;
    for (auto& t: m_LogTargets)
    {
        // lock target mutex
        std::unique_lock lock(t->lock);
        for (auto& log: m_InternalLogBuffer)
        {
            if (!written_outfile)
                m_LogFile << log << std::endl;
            t->LogQueue.push_back(log);
        }
        // unlock mutex
        lock.unlock();
        // signal condition variable that logging is done
        t->cv.notify_one();
        written_outfile = true;
    }
    m_InternalLogBuffer.clear();
    m_InternalLogBuffer.shrink_to_fit();
}

void Logger::Log(LOGTYPE type, const std::string &log_msg)
{
    /* Only add to log queue if type meets logging level standard */
    if (m_LogLevel <= type)
    {
        auto log = GetLogLabel(type) + log_msg;
        m_InternalLogBuffer.push_back(log);
        std::cout << log << std::endl;
    }
}

void Logger::RegisterTarget(LogTarget *target)
{
    m_LogTargets.push_back(target);
}

void Logger::TryDeleteFile(const std::string& filename)
{
    if (std::filesystem::exists(filename)) { // Check if the file exists
        try {
            std::filesystem::remove(filename);
        } 
        catch (const std::filesystem::filesystem_error& err) {
            return;
        }
    } 
}

std::string Logger::GetLogLabel(LOGTYPE type)
{
    switch(type)
    {
        case LOGTYPE::INFO: return "[INFO]: "; break;
        case LOGTYPE::WARNING: return "[WARNING]: "; break;
        case LOGTYPE::ERROR: return "[ERROR]: "; break;
        default:
            break;
    }
    return "";
}

void LogTarget::ResizeQueue(int count)
{
    if (LogQueue.size() < count)
    {
        logger.Log(LOGTYPE::INFO, "LogTarget::ResizeQueue() being needlessly called\n");
        return;
    }
    std::vector<std::string> tmp;

    for (int i = 0; i < count; i++)
    {
        auto item = LogQueue.back();
        tmp.push_back(item);
        LogQueue.pop_back();
    }
    LogQueue = tmp;
}

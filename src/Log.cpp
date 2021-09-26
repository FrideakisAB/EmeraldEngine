#include "Log.h"

#include <ctime>
#include <sstream>

Log::Log()
{
    logFile.open("LatestLog.txt", std::ios::trunc);
}

Log::~Log()
{
    if (logFile.is_open())
        logFile.close();
}

void Log::Send(const std::string &name, const std::string &msg)
{
    time_t now = time(nullptr);

    tm localTime{};
#ifdef __linux__
    localtime_r(&now, &localTime);
#elif _WIN32
    localtime_s(&localTime, &now);
#endif

    std::stringstream resultString;
    resultString << "[" << name << "]"
                 << "[" << 1900 + localTime.tm_year << "-" << localTime.tm_mon + 1 << "-" << localTime.tm_mday
                 << " " << localTime.tm_hour << ":" << localTime.tm_min << ":" << localTime.tm_sec << "]: "
                 << msg << std::endl;

    Write(resultString.str());
}

void Log::Write(const std::string &msg)
{
    lock.lock();
    logFile << msg;
    lock.unlock();
}

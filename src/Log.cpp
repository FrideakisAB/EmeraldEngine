#include "Log.h"

#include <ctime>
#include <chrono>
#include <sstream>
#include <iomanip>

Log *logger = nullptr;

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
    using clock = std::chrono::system_clock;
    std::time_t currentTime = clock::to_time_t(clock::now());

    tm localTime{};
#ifdef __linux__
    localtime_r(&currentTime , &localTime);
#elif _WIN32
    localtime_s(&localTime, &currentTime);
#endif

    std::stringstream resultString;
    resultString << "[" << name << "]"
                 << std::put_time(&localTime, "[%F %T]: ")
                 << msg << std::endl;

    Write(resultString.str());
}

void Log::Write(const std::string &msg)
{
    lock.lock();
    logFile << msg;
    lock.unlock();
}

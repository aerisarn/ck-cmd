#pragma once

#include "ILogger.h"

#include <QString>

#include <sstream>
#include <map>
#include <thread>

namespace ckcmd {

    std::ostream& log_endl(std::ostream& out);

    class Log : public std::ostringstream {
        friend std::ostream& log_endl(std::ostream& out);
        static ILogger* _backend;

    public:

        static void setBackend(ILogger* backend);
        static Log& Get();



    private:

        //std::stringstream stream;

        void setImplementation(ILogger* backend);
        
        Log() = default;

        // Delete copy/move so extra instances can't be created/moved.
        Log(const Log&) = delete;
        Log& operator=(const Log&) = delete;
        Log(Log&&) = delete;
        Log& operator=(Log&&) = delete;
    };

    Log& operator<<(Log& stream, const QString& in) {
        stream << std::string(in.toUtf8().constData());
        return stream;
    }
}



#define LOG Log::Get() << __FILE__ << " "
#define LOGINFO Log::Get() << "[INFO] " 
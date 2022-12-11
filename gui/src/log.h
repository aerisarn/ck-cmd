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

    inline Log& operator<<(Log& stream, const char* in) {
        stream.operator<<(in);
        return stream;
    }

    //inline Log& operator<<(Log& stream, const std::string& in) {
    //    std::ostream::operator<<(in);
    //    return stream;
    //}

    inline Log& operator<<(Log& stream, const QString& in) {
        stream.operator<<(in.toUtf8().constData());
        return stream;
    }

    //inline Log& operator<<(Log& stream, const fs::path& in) {
    //    stream << in.string();
    //    return stream;
    //}
}



#define LOG Log::Get() << __FILE__ << " "
#define LOGINFO Log::Get() << "[INFO] " 
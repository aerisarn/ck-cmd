#pragma once

#include "ILogger.h"

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



        //template<typename T>
        //Log& operator<<(T& in) {
        //    stream << in;
        //    return *this;
        //}

        //template<>
        //Log& operator<<(Log& in) {
        //    return log::endl(in);
        //}

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
}

#define LOG Log::Get() << __FILE__ << " "
#include "log.h"

using namespace ckcmd;

ILogger* Log::_backend = NULL;

void Log::setBackend(ILogger* backend) {
    _backend = backend;
}

Log& Log::Get() {
    static std::map<std::thread::id, Log*> logs;
    if (logs.find(std::this_thread::get_id()) == logs.end())
        logs[std::this_thread::get_id()] = new Log();
    return *logs[std::this_thread::get_id()];
}

void Log::setImplementation(ILogger* backend) {
    _backend = backend;
}

std::ostream& ckcmd::log_endl(std::ostream& out)
{
    Log* log = dynamic_cast<Log*>(&out);
    out << std::endl;
    log->_backend->print(log->str());
    log->str(std::string());
    return out;
}
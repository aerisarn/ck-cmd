#pragma once

//Here just because there's no better place
#include <filesystem>

#if _MSC_VER < 1920
namespace fs = std::experimental::filesystem;
#else
namespace fs = std::filesystem;
#endif

#include <string>

class ILogger {
public:
    virtual void print(const std::string& string) = 0;
};
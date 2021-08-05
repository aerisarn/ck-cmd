#include "LogControl.h"
#include "../config.h"
#include <qscrollbar>

using namespace ckcmd::GUI;

LogControl::LogControl() {
    std::string logfile_path = Settings.get<std::string>("global/logfile_path");
    if (!fs::exists(logfile_path) || !fs::is_directory(logfile_path))
    {
        logfile_path = ".";
        Settings.set("global/logfile_path", "");
    }
    logfile_path = (fs::path(logfile_path) / "ckcmd-log.txt").string();
    if (fs::exists(logfile_path)) 
    {
        fs::copy_file
        (
            logfile_path, 
            fs::path(logfile_path).replace_extension(".old"), 
            fs::copy_options::overwrite_existing 
        );
    }
    _logFile.setFileName(logfile_path.c_str());
    _logFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
    print(std::string("CK-CMD GUI logfile: ") + logfile_path);
}

LogControl::~LogControl() {
    _logFile.flush();
    _logFile.close();
}

void LogControl::print(const QString& text) {
    appendPlainText(text);
    verticalScrollBar()->setValue(this->verticalScrollBar()->maximum());
    _logFile.write(text.toUtf8()); // Logs to file
}
void LogControl::print(const std::string& text) {
    print(QString::fromStdString(text));
}

void LogControl::print(const std::wstring& text) {
    print(QString::fromStdWString(text));
}

void LogControl::print(const char* text) {
    print(QString::fromStdString(text));
}
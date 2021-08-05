#pragma once

#include <src/ILogger.h>

#include <qplaintextedit>
#include <string>

namespace ckcmd {
    namespace GUI {

        class LogControl : public QPlainTextEdit, public ILogger
        {
            Q_OBJECT

            QFile _logFile;
        public slots:
            void print(const QString& text);
            virtual void print(const std::string& text) override;
            void print(const std::wstring& text);
            void print(const char* text);

        public:
            LogControl();
            ~LogControl();
        };

    }
}
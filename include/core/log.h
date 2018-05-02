/**********************************************************************
*<
FILE: Log.h

DESCRIPTION:	 File Formats

HISTORY:
*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/

#pragma once

#include <tchar.h>
#include <stdarg.h>

enum LogLevel
{
	LOG_ALL = 0,
	LOG_VERBOSE,
	LOG_DEBUG,
	LOG_INFO,
	LOG_WARN,
	LOG_ERROR,
	LOG_NONE,
};

// CHAR version
class ILogListenerA
{
public:
	virtual void Message( LogLevel level, const char* strMessage ) = NULL;
};

// WCHAR version
class ILogListenerW
{
public:
	virtual void Message( LogLevel level, const wchar_t* strMessage ) = NULL;
};

#ifdef _UNICODE
#define ILogListener ILogListenerW
#else
#define ILogListener ILogListenerA
#endif

class Log
{
	Log();
public:

	static void EnableLogging( bool enable );

	static void SetLogLevel( LogLevel uLevel );
	static LogLevel GetLogLevel();

	static void AddListener( ILogListenerA* pListener );
	static void AddListener( ILogListenerW* pListener );
	static void RemoveListener( ILogListenerA* pListener );
	static void RemoveListener( ILogListenerW* pListener );
	static void ClearListeners();

	static bool IsErrorEnabled();
	static bool IsWarnEnabled();
	static bool IsInfoEnabled();
	static bool IsDebugEnabled();
	static bool IsVerboseEnabled();

	static void Error( const char* strFormat, ... );
	static void Warn( const char* strFormat, ... );
	static void Info( const char* strFormat, ... );
	static void Debug( const char* strFormat, ... );
	static void Verbose( const char* strFormat, ... );
	static void Msg( LogLevel level, const char* strFormat, ... );
	static void MsgV( LogLevel level, const char* strFormat, va_list argptr );

	static void Error( const wchar_t* strFormat, ... );
	static void Warn( const wchar_t* strFormat, ... );
	static void Info( const wchar_t* strFormat, ... );
	static void Debug( const wchar_t* strFormat, ... );
	static void Verbose( const wchar_t* strFormat, ... );
	static void Msg( LogLevel level, const wchar_t* strFormat, ... );
	static void MsgV( LogLevel level, const wchar_t* strFormat, va_list argptr );
};


#define LOG_ERROR(x) { if (Log::IsErrorEnabled()) { Log::Error(x); } }
#define LOG_WARN(x) { if (Log::IsWarnEnabled()) { Log::Warn(x); } }
#define LOG_INFO(x) { if (Log::IsInfoEnabled()) { Log::Info(x); } }
#define LOG_DEBUG(x) { if (Log::IsDebugEnabled()) { Log::Debug(x); } }
#define LOG_VERBOSE(x) { if (Log::IsVerboseEnabled()) { Log::Verbose(x); } }

#define LOG_ASSERT(x) { if ( if (!(x) && Log::IsInfoEnabled()) { Log::Info(##x); } }
#define LOG_ASSERT_WARN(x) { if (!(x) && Log::IsWarnEnabled()) { Log::Warn(##x); } }
#define LOG_ASSERT_ERROR(x) { if (!(x) && Log::IsErrorEnabled()) { Log::Error(##x); } }
#define LOG_ASSERT_DEBUG(x) { if (!(x) && Log::IsDebugEnabled()) { Log::Debug(##x); } }
#define LOG_ASSERT_VERBOSE(x) { if (!(x) && Log::IsVerboseEnabled()) { Log::Verbose(##x); } }

class ConsoleLogger : public ILogListener
{
	HANDLE  hError;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
public:
	ConsoleLogger();
	void Message( LogLevel level, const TCHAR* strMessage );
};

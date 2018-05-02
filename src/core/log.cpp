#include <stdafx.h>

#include <core/log.h>

#include <malloc.h>
#include <list>

static std::list<ILogListenerA*> listenersA;
static std::list<ILogListenerW*> listenersW;

static bool logEnabled = true;
static LogLevel logLevel = LOG_NONE;

static void DispatchMessage(LogLevel level, const char *message, int len);
static void DispatchMessage(LogLevel level, const wchar_t *message, int len);

void DispatchMessage(LogLevel level, const char *message, int len)
{
	for (std::list<ILogListenerA*>::iterator itr = listenersA.begin(); itr != listenersA.end(); ++itr) {
		(*itr)->Message(level, message);
	}
	if (!listenersW.empty()) {
		wchar_t* pbuf = (wchar_t*)_alloca((len+1) * sizeof(wchar_t));
		size_t n;
		mbstowcs_s(&n, pbuf, len+1, message, len);
		pbuf[len-1] = 0;
		for (std::list<ILogListenerW*>::iterator itr = listenersW.begin(); itr != listenersW.end(); ++itr) {
			(*itr)->Message(level, pbuf);
		}
	}
}

void DispatchMessage(LogLevel level, const wchar_t *message, int len)
{
	for (std::list<ILogListenerW*>::iterator itr = listenersW.begin(); itr != listenersW.end(); ++itr) {
		(*itr)->Message(level, message);
	}
	if (!listenersA.empty()) {
		char* pbuf = (char*)_alloca((len+1) * sizeof(char));
		size_t n;
		wcstombs_s(&n, pbuf, len+1, message, len);
		pbuf[len-1] = 0;
		for (std::list<ILogListenerA*>::iterator itr = listenersA.begin(); itr != listenersA.end(); ++itr) {
			(*itr)->Message(level, pbuf);
		}
	}
}

void Log::EnableLogging( bool enable )
{
	logEnabled = enable;
}

void Log::SetLogLevel( LogLevel level )
{
	logLevel = level;
}

LogLevel Log::GetLogLevel()
{
	return logLevel;
}

void Log::AddListener( ILogListenerA* pListener )
{
	listenersA.push_back(pListener);
}

void Log::AddListener( ILogListenerW* pListener )
{
	listenersW.push_back(pListener);
}


void Log::ClearListeners()
{
	listenersA.clear();
	listenersW.clear();
}

bool Log::IsErrorEnabled()
{
	return logEnabled && logLevel <= LOG_ERROR && (!listenersA.empty() || !listenersW.empty());
}

bool Log::IsWarnEnabled()
{
	return logEnabled && logLevel <= LOG_WARN && (!listenersA.empty() || !listenersW.empty());
}

bool Log::IsInfoEnabled()
{
	return logEnabled && logLevel <= LOG_INFO && (!listenersA.empty() || !listenersW.empty());
}

bool Log::IsDebugEnabled()
{
	return logEnabled && logLevel <= LOG_DEBUG && (!listenersA.empty() || !listenersW.empty());
}

bool Log::IsVerboseEnabled()
{
	return logEnabled && logLevel <= LOG_VERBOSE && (!listenersA.empty() || !listenersW.empty());
}

void Log::Error( const char* format, ... )
{
	if (!IsErrorEnabled()) return;
	va_list args;
	va_start(args, format);
	MsgV(LOG_ERROR, format, args);
	va_end(args);		
}

void Log::Warn( const char* format, ... )
{
	if (!IsWarnEnabled()) return;
	va_list args;
	va_start(args, format);
	MsgV(LOG_WARN, format, args);
	va_end(args);	
}

void Log::Info( const char* format, ... )
{
	if (!IsInfoEnabled()) return;
	va_list args;
	va_start(args, format);
	MsgV(LOG_INFO, format, args);
	va_end(args);	
}

void Log::Debug( const char* format, ... )
{
	if (!IsDebugEnabled()) return;
	va_list args;
	va_start(args, format);
	MsgV(LOG_DEBUG, format, args);
	va_end(args);	
}

void Log::Verbose( const char* format, ... )
{
	if (!IsVerboseEnabled()) return;
	va_list args;
	va_start(args, format);
	MsgV(LOG_VERBOSE, format, args);
	va_end(args);
}

void Log::Msg( LogLevel level, const char* format, ... )
{
	if (!IsVerboseEnabled()) return;
	va_list args;
	va_start(args, format);
	MsgV(LOG_VERBOSE, format, args);
	va_end(args);
}

void Log::MsgV( LogLevel level, const char* format, va_list args )
{
	char buffer[512];
	int nChars = _vsnprintf_s(buffer, _countof(buffer), format, args);
	if (nChars != -1) {
		DispatchMessage(level, buffer, nChars);
	} else {
		size_t Size = _vscprintf(format, args);
		char* pbuf = (char*)_alloca(Size * sizeof(char));
		nChars = _vsnprintf_s(pbuf, Size, Size, format, args);
		DispatchMessage(level, pbuf, nChars);
	}
}

void Log::Error( const wchar_t* format, ... )
{
	if (!IsErrorEnabled()) return;
	va_list args;
	va_start(args, format);
	MsgV(LOG_ERROR, format, args);
	va_end(args);		
}

void Log::Warn( const wchar_t* format, ... )
{
	if (!IsWarnEnabled()) return;
	va_list args;
	va_start(args, format);
	MsgV(LOG_WARN, format, args);
	va_end(args);	
}

void Log::Info( const wchar_t* format, ... )
{
	if (!IsInfoEnabled()) return;
	va_list args;
	va_start(args, format);
	MsgV(LOG_INFO, format, args);
	va_end(args);	
}

void Log::Debug( const wchar_t* format, ... )
{
	if (!IsDebugEnabled()) return;
	va_list args;
	va_start(args, format);
	MsgV(LOG_DEBUG, format, args);
	va_end(args);	
}

void Log::Verbose( const wchar_t* format, ... )
{
	if (!IsVerboseEnabled()) return;
	va_list args;
	va_start(args, format);
	MsgV(LOG_VERBOSE, format, args);
	va_end(args);
}

void Log::Msg( LogLevel level, const wchar_t* format, ... )
{
	if (!IsVerboseEnabled()) return;
	va_list args;
	va_start(args, format);
	MsgV(LOG_VERBOSE, format, args);
	va_end(args);
}

void Log::MsgV( LogLevel level, const wchar_t* format, va_list args )
{
	wchar_t buffer[512];
	int nChars = _vsnwprintf_s(buffer, _countof(buffer), format, args);
	if (nChars != -1) {
		DispatchMessage(level, buffer, nChars);
	} else {
		size_t Size = _vscwprintf(format, args);
		wchar_t* pbuf = (wchar_t*)_alloca((Size+1) * sizeof(wchar_t));
		nChars = _vsnwprintf_s(pbuf, Size, Size, format, args);
		DispatchMessage(level, pbuf, nChars);
	}
}

void Log::RemoveListener( ILogListenerA* pListener )
{
	listenersA.remove(pListener);
}

void Log::RemoveListener( ILogListenerW* pListener )
{
	listenersW.remove(pListener);
}


ConsoleLogger::ConsoleLogger()
{
	hError = GetStdHandle( STD_ERROR_HANDLE );
	GetConsoleScreenBufferInfo( hError, &csbi );
}
void ConsoleLogger::Message( LogLevel level, const TCHAR* strMessage )
{
	if (strMessage == NULL || strMessage[0] == 0)
		return;
	size_t n = _tcslen(strMessage);
	bool hasCRLF = (strMessage[n-1] == '\n' || strMessage[n-1] == '\r');

	switch (level)
	{
	case LOG_ERROR:
		SetConsoleTextAttribute( hError, FOREGROUND_RED | FOREGROUND_INTENSITY | (csbi.wAttributes & 0x00F0) );
		//_ftprintf( stderr, "ERROR:   %s\n", strMessage );
		fputs(strMessage, stderr); if (!hasCRLF) fputs("\n", stderr);
		SetConsoleTextAttribute( hError, csbi.wAttributes );
		break;
	case LOG_WARN: 
		SetConsoleTextAttribute( hError, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY | (csbi.wAttributes & 0x00F0) );
		//_ftprintf( stderr, "WARN:    %s\n", strMessage );
		fputs(strMessage, stderr); if (!hasCRLF) fputs("\n", stderr);
		SetConsoleTextAttribute( hError, csbi.wAttributes );
		break;
	case LOG_INFO:
		SetConsoleTextAttribute( hError, csbi.wAttributes | FOREGROUND_INTENSITY);
		//_ftprintf( stderr, "INFO:    %s\n", strMessage );
		fputs(strMessage, stderr); if (!hasCRLF) fputs("\n", stderr);
		SetConsoleTextAttribute( hError, csbi.wAttributes );
		break;
	case LOG_DEBUG: 
		SetConsoleTextAttribute( hError,  (csbi.wAttributes & ~FOREGROUND_INTENSITY) );
		//_ftprintf( stderr, "DEBUG:   %s\n", strMessage );
		fputs(strMessage, stderr); if (!hasCRLF) fputs("\n", stderr);
		SetConsoleTextAttribute( hError, csbi.wAttributes );
		break;
	case LOG_VERBOSE: 
		SetConsoleTextAttribute( hError, FOREGROUND_BLUE | FOREGROUND_GREEN | (csbi.wAttributes & 0x00F0) );
		//_ftprintf( stderr, "VERBOSE: %s\n", strMessage );
		fputs(strMessage, stderr); if (!hasCRLF) fputs("\n", stderr);
		SetConsoleTextAttribute( hError, csbi.wAttributes );
		break;
	}
}

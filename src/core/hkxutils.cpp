#include "stdafx.h"

#include <io.h>
#include <core/hkxutils.h>

FILE _iob[] = { *stdin, *stdout, *stderr };

extern "C" FILE * __cdecl __iob_func(void)
{
	return _iob;
}

// Macro to create a dynamically allocated strdup on the stack
#define STRDUPA(p) (_tcscpy((TCHAR*)alloca((_tcslen(p)+1)*sizeof(*p)),p))

extern int wildcmp(const TCHAR *wild, const TCHAR *string);
extern int wildcmpi(const TCHAR *wild, const TCHAR *string);

// Original Source: Jack Handy www.codeproject.com
int wildcmp(const TCHAR *wild, const TCHAR *string) {
   const TCHAR *cp, *mp;

   while ((*string) && (*wild != '*')) {
      if ((*wild != *string) && (*wild != '?')) {
         return 0;
      }
      wild++;
      string++;
   }

   while (*string) {
      if (*wild == '*') {
         if (!*++wild) {
            return 1;
         }
         mp = wild;
         cp = string+1;
      } else if ((*wild == *string) || (*wild == '?')) {
         wild++;
         string++;
      } else {
         wild = mp;
         string = cp++;
      }
   }

   while (*wild == '*') {
      wild++;
   }
   return !*wild;
}

// Same as above but case insensitive
int wildcmpi(const TCHAR *wild, const TCHAR *string) {
   const TCHAR *cp, *mp;
   int f,l;
   while ((*string) && (*wild != '*')) {
      f = _totlower( (_TUCHAR)(*string) );
      l = _totlower( (_TUCHAR)(*wild) );
      if ((f != l) && (l != '?')) {
         return 0;
      }
      wild++, string++;
   }
   while (*string) {
      if (*wild == '*') {
         if (!*++wild) return 1;
         mp = wild, cp = string+1;
      } else {
         f = _totlower( (_TUCHAR)(*string) );
         l = _totlower( (_TUCHAR)(*wild) );
         if ((f == l) || (l == '?')) {
            wild++, string++;
         } else {
            wild = mp, string = cp++;
         }
      }
   }
   while (*wild == '*') wild++;
   return !*wild;
}

bool wildmatch(const string& match, const std::string& value) 
{
   return (wildcmpi(match.c_str(), value.c_str())) ? true : false;
}

bool wildmatch(const stringlist& matches, const std::string& value)
{
   for (stringlist::const_iterator itr=matches.begin(), end=matches.end(); itr != end; ++itr){
      if (wildcmpi((*itr).c_str(), value.c_str()))
         return true;
   }
   return false;
}

// Tokenize a string using strtok and return it as a stringlist
stringvector TokenizeString(LPCTSTR str, LPCTSTR delims, bool trim = true)
{
   stringvector values;
   LPTSTR buf = STRDUPA(str);
   for (LPTSTR p = _tcstok(buf, delims); p && *p; p = _tcstok(NULL, delims)){
      if (trim) Trim(p);
      values.push_back(string(p));
   }
   return values;
}

// sprintf for std::string without having to worry about buffer size.
extern
std::string FormatString(const TCHAR* format,...)
{
	TCHAR buffer[512];
	std::string text;
	va_list args;
	va_start(args, format);
	int nChars = _vsntprintf(buffer, _countof(buffer), format, args);
	if (nChars != -1) {
		text = buffer;
	} else {
		size_t Size = _vsctprintf(format, args);
		TCHAR* pbuf = (TCHAR*)_alloca(Size);
		nChars = _vsntprintf(pbuf, Size, format, args);
		text = pbuf;
	}
	va_end(args);
	return text;
}

string replaceSubstring(string instr, string match, string repl) {

   string retval = instr;
   for (int pos = retval.find(match, 0); pos != string::npos; pos = retval.find(match, 0)) 
   {
      retval.erase(pos, match.length());
      retval.insert(pos, repl);
   }
   return retval;
}

// Enumeration Support
string EnumToString(int value, const EnumLookupType *table) {
	for (const EnumLookupType *itr = table; itr->name != NULL; ++itr) {
		if (itr->value == value) return string(itr->name);
	}
	return FormatString("%x", value);
}

int EnumToIndex(int value, const EnumLookupType *table) {
	int i = 0;
	for (const EnumLookupType *itr = table; itr->name != NULL; ++itr, ++i) {
		if (itr->value == value) return i;
	}
	return -1;
}

int StringToEnum(string value, const EnumLookupType *table, int defaultValue) {
	//Trim(value);
	if (value.empty()) return defaultValue;

	for (const EnumLookupType *itr = table; itr->name != NULL; ++itr) {
		if (0 == _tcsicmp(value.c_str(), itr->name)) return itr->value;
	}
	char *end = NULL;
	return (int)_tcstol(value.c_str(), &end, 0);
}

string FlagsToString(int value, const EnumLookupType *table) {
	string sstr;
	for (const EnumLookupType *itr = table; itr->name != NULL; ++itr) {
		if (itr->value && (itr->value & value) == itr->value) {
			if (!sstr.empty()) sstr += " | ";
			sstr += itr->name;
			value ^= itr->value;
		}
	}
	if (value == 0 && sstr.empty()) {
		return EnumToString(value, table);
	}
	if (value != 0) {
		if (!sstr.empty()) sstr += "|";
		sstr += EnumToString(value, table);
	}
	return sstr;
}

int StringToFlags(string value, const EnumLookupType *table, int defaultValue) {
	int retval = defaultValue;
	LPCTSTR start = value.data();
	LPCTSTR end = value.data() + value.length();
	while(start < end) {
		LPCTSTR bar = _tcspbrk(start, ",;+.|");
		int len = (bar != NULL) ?  bar-start : end-start;
		string subval = value.substr(start-value.data(), len);
		retval |= StringToEnum(subval, table);
		start += (len + 1);
	}
	return retval;
}


bool FindMatchingFiles( LPCTSTR match, stringlist& result )
{
	bool ok = false;
	TCHAR resultbuf[MAX_PATH];
	if (NULL != strpbrk(match, "?*"))
	{
		WIN32_FIND_DATA FindFileData;
		ZeroMemory(&FindFileData, sizeof(FindFileData));

		TCHAR path[_MAX_PATH], filebuf[_MAX_PATH];
		strcpy(path, match);
		PathRemoveFileSpec(path);

		HANDLE hFind = FindFirstFile(match, &FindFileData);
		if (hFind != INVALID_HANDLE_VALUE) {
			for (BOOL next = TRUE ; next ; next = FindNextFile(hFind, &FindFileData)) {
				PathCombine(filebuf, path, FindFileData.cFileName);
				GetFullPathName(filebuf, _countof(resultbuf), resultbuf, NULL);
				result.push_back(string(resultbuf));
				ok = true;
			}
			FindClose(hFind);
		}
	}
	else
	{
		if ( 0 == _access(match, 04) )
		{
			GetFullPathName(match, _countof(resultbuf), resultbuf, NULL);
			result.push_back(string(resultbuf));
			ok = true;
		}
	}
	return ok;
}


void FindFiles(std::vector<string>& collection, const TCHAR *path, bool recursive /*= true*/)
{
	stringlist excludes;
	stringlist includes;
	FindFiles(collection, path, excludes, recursive, includes);
}

void FindFiles(vector<string>& collection, const TCHAR *path, stringlist& excludes, bool recursive /*= true*/, stringlist& includes /*= {}*/)
{
	TCHAR buffer[MAX_PATH], dir[MAX_PATH], search[MAX_PATH];
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	bool needrecursion = false;
	ZeroMemory(&FindFileData, sizeof(FindFileData));
	if (path == NULL || path[0] == 0)
		return;
	//PathCanonicalize(search, path);
	strcpy(search, path);
	if ( _tcspbrk(search, _T("*?")) == NULL ) {
		if (PathIsDirectory(search))
		{
			_tcscpy(dir, path);
			PathAddBackslash(search);
			_tcscat(search, _T("*"));
			needrecursion = true;
		}
		else
		{
			_tcscpy(dir, path);
			PathRemoveFileSpec(dir);
		}
	} else {
		_tcscpy(dir, path);
		PathRemoveFileSpec(dir);
		needrecursion = true;
	}

	hFind = FindFirstFile(search, &FindFileData);
	if (hFind != INVALID_HANDLE_VALUE) {
		for (BOOL ok = TRUE ; ok ; ok = FindNextFile(hFind, &FindFileData)) {
			if (FindFileData.cFileName[0] == '.' || (FindFileData.dwFileAttributes & (FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM)))
				continue;
			if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				PathCombine(buffer, dir, FindFileData.cFileName);
				//GetLongPathName(buffer, buffer, MAX_PATH);

				if (!excludes.empty())
				{
					if ( wildmatch(excludes, buffer) )
						continue;
				}

				if (!includes.empty())
				{
					if ( !wildmatch(includes, buffer))
						continue;
				}

				collection.push_back( string(buffer) );					
			}
		}
		FindClose(hFind);
	}
	if (needrecursion && recursive)
	{
		//PathCanonicalize(search, path);
		strcpy(search, path);
		PathRemoveFileSpec(search);
		PathAddBackslash(search);
		_tcscat(search, _T("*"));

		hFind = FindFirstFile(search, &FindFileData);
		if (hFind != INVALID_HANDLE_VALUE) {
			vector<string> l;
			for (BOOL ok = TRUE ; ok ; ok = FindNextFile(hFind, &FindFileData)) {
				if (FindFileData.cFileName[0] == '.' || (FindFileData.dwFileAttributes & (FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM)))
					continue;
				if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					PathCombine(buffer, dir, FindFileData.cFileName);
					PathAddBackslash(buffer);
//					PathAppend(buffer, PathFindFileName(path));
					l.push_back( string(buffer) );
				}
			}
			FindClose(hFind);
			for (vector<string>::iterator itr = l.begin(), end = l.end(); itr != end; ++itr) {
				FindFiles(collection, (*itr).c_str(), excludes, true, includes);
			}
		}
	}
}

void CreateDirectories(LPCTSTR path)
{
	TCHAR tmp[260];
	GetFullPathName(path, _countof(tmp), tmp, NULL);

	if ( !CreateDirectory(tmp, NULL) ) {
		if (GetLastError() == ERROR_PATH_NOT_FOUND ) {
			PathRemoveFileSpec(tmp);
			CreateDirectories(tmp);
			CreateDirectory(path, NULL);
		}
	}
}


/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the w64 mingw-runtime package.
 * No warranty is given; refer to the file DISCLAIMER within this package.
 */
#include <math.h>

float roundf (float x)
{
  float res;
  if (x >= 0.0F) { res = ceilf (x); if (res - x > 0.5F) res -= 1.0F; }
  else { res = ceilf (-x); if (res + x > 0.5F) res -= 1.0F; res = -res; }
  return res;
}



extern "C" void PrintV(FILE* hFile, LPCSTR lpszFormat, va_list argptr)
{
   vfprintf_s(hFile, lpszFormat, argptr);
}

extern "C" void PrintLineV(FILE* hFile, LPCSTR lpszFormat, va_list argptr)
{
   PrintV(hFile, lpszFormat, argptr);
   fprintf_s(hFile, "\n");
}

extern "C" void PrintLine(FILE* hFile, LPCSTR lpszFormat, ...)
{
   va_list argList;
   va_start(argList, lpszFormat);
   PrintLineV(hFile, lpszFormat, argList);
   va_end(argList);
}

extern "C" void Print(FILE* hFile, LPCSTR lpszFormat, ...)
{
   va_list argList;
   va_start(argList, lpszFormat);
   PrintV(hFile, lpszFormat, argList);
   va_end(argList);
}

string GetFileVersion(const char *fileName)
{
   string retval;
   char fileVersion[MAX_PATH];
   if (fileName == NULL)
   {
      GetModuleFileName(NULL, fileVersion, MAX_PATH);
      fileName = fileVersion;
   }
   HMODULE ver = GetModuleHandle("version.dll");
   if (!ver) ver = LoadLibrary("version.dll");
   if (ver != NULL)
   {
      DWORD (APIENTRY *GetFileVersionInfoSize)(LPCTSTR, LPDWORD) = NULL;
      BOOL (APIENTRY *GetFileVersionInfo)(LPCTSTR, DWORD, DWORD, LPVOID) = NULL;
      BOOL (APIENTRY *VerQueryValue)(const LPVOID, LPTSTR, LPVOID *, PUINT) = NULL;
      *(FARPROC*)&GetFileVersionInfoSize = GetProcAddress(ver, "GetFileVersionInfoSizeA");
      *(FARPROC*)&GetFileVersionInfo = GetProcAddress(ver, "GetFileVersionInfoA");
      *(FARPROC*)&VerQueryValue = GetProcAddress(ver, "VerQueryValueA");
      if (GetFileVersionInfoSize && GetFileVersionInfo && VerQueryValue)
      {
         DWORD vLen = 0;
         DWORD vSize = GetFileVersionInfoSize(fileName,&vLen);
         if (vSize) 
         {
            LPVOID versionInfo = malloc(vSize+1);
            if (GetFileVersionInfo(fileName,vLen,vSize,versionInfo))
            {            
               LPVOID version=NULL;
               if (VerQueryValue(versionInfo,"\\VarFileInfo\\Translation",&version,(UINT *)&vLen) && vLen==4) 
               {
                  DWORD langD = *(DWORD*)version;
                  sprintf(fileVersion, "\\StringFileInfo\\%02X%02X%02X%02X\\ProductVersion",
                     (langD & 0xff00)>>8,langD & 0xff,(langD & 0xff000000)>>24, (langD & 0xff0000)>>16);            
               }
               else 
               {
                  sprintf(fileVersion, "\\StringFileInfo\\%04X04B0\\ProductVersion", GetUserDefaultLangID());
               }
               LPCTSTR value = NULL;
               if (VerQueryValue(versionInfo,fileVersion,&version,(UINT *)&vLen))
                  value = LPCTSTR(version);
               else if (VerQueryValue(versionInfo,"\\StringFileInfo\\040904B0\\ProductVersion",&version,(UINT *)&vLen))
                  value = LPCTSTR(version);
               if (value != NULL)
               {
                  stringvector val = TokenizeString(value, ",", true);
                  if (val.size() >= 4){
                     retval = FormatString("%s.%s.%s.%s", val[0].c_str(), val[1].c_str(), val[2].c_str(), val[3].c_str());
                  }
               }
               free(versionInfo);
            }
         }
      }
   }
   return retval;
}
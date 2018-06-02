#pragma once
#ifndef _WINDOWS_
#  include <windows.h>
#endif
#include <tchar.h>
#include <string>
#include <map>
#include <vector>
#include <list>
#include <map>

#include <string.h>
#include <ctype.h>
#include <locale.h>
#include <malloc.h>
#include <sstream>

#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;
// Macro to create a dynamically allocated strdup on the stack
#define STRDUPA(p) (_tcscpy((TCHAR*)alloca((_tcslen(p)+1)*sizeof(*p)),p))

static inline float TODEG(float x) { return x * 180.0f / (float)M_PI; }
static inline float TORAD(float x) { return x * (float)M_PI / 180.0f; }

// Trim whitespace before and after a string
inline TCHAR *Trim(TCHAR*&p) { 
   while(_istspace(*p)) *p++ = 0; 
   TCHAR *e = p + _tcslen(p) - 1;
   while (e > p && _istspace(*e)) *e-- = 0;
   return p;
}

// Case insensitive string equivalence test for collections
struct ltstr
{
   bool operator()(const char* s1, const char* s2) const
   { return stricmp(s1, s2) < 0; }

   bool operator()(const string& s1, const string& s2) const
   { return stricmp(s1.c_str(), s2.c_str()) < 0; }

   bool operator()(const string& s1, const char * s2) const
   { return stricmp(s1.c_str(), s2) < 0; }

   bool operator()(const char * s1, const string& s2) const
   { return stricmp(s1, s2.c_str()) >= 0; }
};


// Case insensitive string equivalence but numbers are sorted together
struct NumericStringEquivalence
{
   bool operator()(const TCHAR* s1, const TCHAR* s2) const
   { return numstrcmp(s1, s2) < 0; }

   bool operator()(const std::string& s1, const TCHAR* s2) const
   { return numstrcmp(s1.c_str(), s2) < 0; }

   bool operator()(const TCHAR* s1, const std::string& s2) const
   { return numstrcmp(s1, s2.c_str()) < 0; }

   bool operator()(const std::string& s1, const std::string& s2) const
   { return numstrcmp(s1.c_str(), s2.c_str()) < 0; }

   static int numstrcmp(const TCHAR *str1, const TCHAR *str2)
   {
      TCHAR *p1, *p2;
      int c1, c2, lcmp;
      for(;;)
      {
         c1 = tolower(*str1), c2 = tolower(*str2);
         if ( c1 == 0 || c2 == 0 )
            break;
         else if (isdigit(c1) && isdigit(c2))
         {			
            lcmp = strtol(str1, &p1, 10) - strtol(str2, &p2, 10);
            if ( lcmp == 0 )
               lcmp = (p2 - str2) - (p1 - str1);
            if ( lcmp != 0 )
               return (lcmp > 0 ? 1 : -1);
            str1 = p1, str2 = p2;
         }
         else
         {
            lcmp = (c1 - c2);
            if (lcmp != 0)
               return (lcmp > 0 ? 1 : -1);
            ++str1, ++str2;
         }
      }
      lcmp = (c1 - c2);
      return ( lcmp < 0 ) ? -1 : (lcmp > 0 ? 1 : 0);
   }
};

typedef std::map<std::string, std::string, ltstr> NameValueCollection;
typedef std::pair<std::string, std::string> KeyValuePair;
typedef std::list<std::string> stringlist;
typedef std::vector<std::string> stringvector;


extern int wildcmp(const TCHAR *wild, const TCHAR *string);
extern int wildcmpi(const TCHAR *wild, const TCHAR *string);

inline bool strmatch(const string& lhs, const std::string& rhs) {
   return (0 == _tcsicmp(lhs.c_str(), rhs.c_str()));
}
inline bool strmatch(const TCHAR* lhs, const std::string& rhs) {
   return (0 == _tcsicmp(lhs, rhs.c_str()));
}
inline bool strmatch(const string& lhs, const TCHAR* rhs) {
   return (0 == _tcsicmp(lhs.c_str(), rhs));
}
inline bool strmatch(const TCHAR* lhs, const TCHAR* rhs) {
   return (0 == _tcsicmp(lhs, rhs));
}

extern bool wildmatch(const string& match, const std::string& value);
extern bool wildmatch(const stringlist& matches, const std::string& value);

extern std::string FormatString(const TCHAR* format,...);
extern std::string replaceSubstring(std::string instr, std::string match, std::string repl);

#pragma region Enumeration support
// Enumeration support
typedef struct EnumLookupType {
	int value;
	const char *name;
} EnumLookupType;

extern string EnumToString(int value, const EnumLookupType *table);
extern int StringToEnum(string value, const EnumLookupType *table, int defaultValue=0);
extern int EnumToIndex(int value, const EnumLookupType *table);

extern string FlagsToString(int value, const EnumLookupType *table);
extern int StringToFlags(const vector<string>& value, const EnumLookupType *table, int defaultValue=0);
extern int StringToFlags(string value, const EnumLookupType *table, int defaultValue=0);
#pragma endregion

bool FindMatchingFiles( LPCTSTR match, stringlist& result );

void FindFiles(std::vector<string>& collection, const TCHAR *path, stringlist&excludes, bool recursive = true, stringlist&includes = stringlist());
void FindFiles(std::vector<string>& collection, const TCHAR *path, bool recursive = true);

void CreateDirectories(LPCTSTR path);

extern float roundf (float x);



EXTERN_C
{
   void PrintV(FILE* hFile, LPCSTR lpszFormat, va_list argptr);
   void PrintLineV(FILE* hFile, LPCSTR lpszFormat, va_list argptr);
   void PrintLine(FILE* hFile, LPCSTR lpszFormat, ...);
   void Print(FILE* hFile, LPCSTR lpszFormat, ...);
}
string GetFileVersion(const char *fileName);
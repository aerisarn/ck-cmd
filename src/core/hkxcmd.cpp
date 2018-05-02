// hkxcmd.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <core/hkxcmd.h>
#include <core/hkxutils.h>
#include <core/log.h>
using namespace std;

//#pragma comment(lib, "shlwapi.lib")

using namespace std;

#pragma warning(disable : 4073)
#pragma init_seg(lib)
hkxcmd::hkxcmdListType hkxcmd::hkxcmdList;

// routine for parsing white space separated lines.  Handled like command line parameters w.r.t quotes.
void hkxcmd::ParseLine (
                        const char *start,
                        char **argv,
                        char *args,
                        int *numargs,
                        int *numchars
                        )
{
   const char NULCHAR    = '\0';
   const char SPACECHAR  = ' ';
   const char TABCHAR    = '\t';
   const char RETURNCHAR = '\r';
   const char LINEFEEDCHAR = '\n';
   const char DQUOTECHAR = '\"';
   const char SLASHCHAR  = '\\';
   const char *p;
   int inquote;                    /* 1 = inside quotes */
   int copychar;                   /* 1 = copy char to *args */
   unsigned numslash;              /* num of backslashes seen */

   *numchars = 0;
   *numargs = 0;                   /* the program name at least */

   p = start;

   inquote = 0;

   /* loop on each argument */
   for(;;) 
   {
      if ( *p ) { while (*p == SPACECHAR || *p == TABCHAR || *p == RETURNCHAR || *p == LINEFEEDCHAR) ++p; }

      if (*p == NULCHAR) break; /* end of args */

      /* scan an argument */
      if (argv)
         *argv++ = args;     /* store ptr to arg */
      ++*numargs;

      /* loop through scanning one argument */
      for (;;) 
      {
         copychar = 1;
         /* Rules: 2N backslashes + " ==> N backslashes and begin/end quote
         2N+1 backslashes + " ==> N backslashes + literal "
         N backslashes ==> N backslashes */
         numslash = 0;
         while (*p == SLASHCHAR) 
         {
            /* count number of backslashes for use below */
            ++p;
            ++numslash;
         }
         if (*p == DQUOTECHAR) 
         {
            /* if 2N backslashes before, start/end quote, otherwise copy literally */
            if (numslash % 2 == 0) {
               if (inquote) {
                  if (p[1] == DQUOTECHAR)
                     p++;    /* Double quote inside quoted string */
                  else        /* skip first quote char and copy second */
                     copychar = 0;
               } else
                  copychar = 0;       /* don't copy quote */

               inquote = !inquote;
            }
            numslash /= 2;          /* divide numslash by two */
         }

         /* copy slashes */
         while (numslash--) {
            if (args)
               *args++ = SLASHCHAR;
            ++*numchars;
         }

         /* if at end of arg, break loop */
         if (*p == NULCHAR || (!inquote && (*p == SPACECHAR || *p == TABCHAR || *p == RETURNCHAR || *p == LINEFEEDCHAR)))
            break;

         /* copy character into argument */
         if (copychar) 
         {
            if (args)
               *args++ = *p;
            ++*numchars;
         }
         ++p;
      }
      /* null-terminate the argument */
      if (args)
         *args++ = NULCHAR;          /* terminate string */
      ++*numchars;
   }
   /* We put one last argument in -- a null ptr */
   if (argv)
      *argv++ = NULL;
   ++*numargs;
}

bool hkxcmd::ParseArgs(LPCTSTR line)
{
   int nargs = 0, nchars = 0;
   ParseLine(line, NULL, NULL, &nargs, &nchars);
   char **largv = (char **)malloc(nargs * sizeof(char *) + nchars * sizeof(char));
   ParseLine(line, largv, ((char*)largv) + nargs * sizeof(char*), &nargs, &nchars);
   bool rv = ParseArgs(nargs, largv);
   free(largv);
   return rv;
}

void hkxcmd::PrintHelp()
{
   char fullName[MAX_PATH], exeName[MAX_PATH];
   GetModuleFileName(NULL, fullName, MAX_PATH);
   _splitpath(fullName, NULL, NULL, exeName, NULL);

   string version = GetFileVersion(fullName);


   Log::Info("%s - Version %s", exeName, version.c_str());
   Log::Info("Usage: %s <command> [-opts[modifiers]]", exeName);
   Log::Info("");
   Log::Info("<Commands>");

   for (hkxcmdListType::iterator itr = hkxcmdList.begin(), end = hkxcmdList.end(); itr != end; ++itr){
      hkxcmd* p = (*itr);
	  fputs("  ", stderr);
      p->HelpCmd(htShort);
   }
   Log::Info("");
   Log::Info("<Global Switches>");
   Log::Info("  %-13s %s", "help", "List of additional help options");
   Log::Info("");
}

hkxcmd* hkxcmd::GetCommand(std::string name)
{
   for (hkxcmdListType::iterator itr = hkxcmdList.begin(), end = hkxcmdList.end(); itr != end; ++itr){
      hkxcmd* p = (*itr);
      if (0 == _tcsicmp(p->Name.c_str(), name.c_str())){
         return p;
      }
   }
   return NULL;
}
list<hkxcmd*> hkxcmd::GetCommand()
{
   list<hkxcmd*> list;
   for (hkxcmdListType::iterator itr = hkxcmdList.begin(), end = hkxcmdList.end(); itr != end; ++itr){
      hkxcmd* p = (*itr);
      list.push_back(p);
   }
   return list;
}

bool hkxcmd::ParseArgs(int argc, char **argv)
{
   bool rv = false;
   try
   {
      if (argc == 0)
      {
         PrintHelp();
         return false;
      }
      else if (argv[0] && ( 0 == _tcsicmp(argv[0], "help")) )
      {
         if (argc > 1 && argv[1] && argv[1][0])
         {
            if (hkxcmd* p = GetCommand(argv[1])) {
               p->HelpCmd(htLong);
               return false;
            }
         }
         PrintHelp();
      }
      else
      {
         if (hkxcmd* p = GetCommand(argv[0])) {
            hkxcmdLine cmdLine(argc-1, &argv[1]);
            rv |= p->ExecuteCmd(cmdLine);
         }
         else
         {
            Log::Error("Unknown command '%s'", argv[0]);
            PrintHelp();
         }
      }
   }
   catch (exception* e)
   {
      Log::Error("Exception occurred:");
      Log::Error("  %s", e->what());
   }
   catch (exception& e)
   {
	   Log::Error("Exception occurred:");
	   Log::Error("  %s", e.what());
   }
   catch (...)
   {
	   Log::Error("Unknown exception occurred");
   }
   return rv;
}

hkxcmdLine::hkxcmdLine(int argc, char **argv)
{
   this->argc = argc;
   this->argv = argv;
}


int _tmain(int argc, _TCHAR* argv[])
{
	ConsoleLogger console;
	Log::AddListener( &console );
	Log::SetLogLevel(LOG_INFO);

    bool ok = hkxcmd::ParseArgs(argc-1, &argv[1]);
	Log::ClearListeners();
    return ok ? 0 : 1;
}

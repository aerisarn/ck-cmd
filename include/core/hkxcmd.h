//#ifndef __ADDIN_H
//#define __ADDIN_H
//
//#include <string>
//#include <functional>
//#include <set>
// 
//#include <loki/Functor.h>
//using namespace std;
//
//class hkxcmdLine
//{
//public:
//   hkxcmdLine(int argc, char **argv);
//
//   int argc;
//   char **argv;
//};
//
//class hkxcmd
//{
//   struct Less
//   {
//      bool operator()(const hkxcmd& lhs, const hkxcmd& rhs) const{
//         return (stricmp(lhs.Name.c_str(), rhs.Name.c_str()) < 0);
//      }
//      bool operator()(const hkxcmd* lhs, const hkxcmd* rhs) const{
//         return (stricmp(lhs->Name.c_str(), rhs->Name.c_str()) < 0);
//      }
//   };
//   typedef std::set<hkxcmd*, hkxcmd::Less> hkxcmdListType;
//   static hkxcmdListType hkxcmdList;
//public:
//
//   enum HelpType {
//      htShort,
//      htLong
//   };
//
//   hkxcmd(std::string name
//      , Loki::Functor<void, LOKI_TYPELIST_1(HelpType)> helpCmd
//      , Loki::Functor<bool, LOKI_TYPELIST_1(hkxcmdLine&)> executeCmd)
//      : Name(name)
//      , HelpCmd(helpCmd)
//      , ExecuteCmd(executeCmd)
//   {
//      hkxcmdList.insert(this);
//   }
//
//   std::string Name;
//   Loki::Functor<void, LOKI_TYPELIST_1(HelpType)> HelpCmd;
//   Loki::Functor<bool, LOKI_TYPELIST_1(hkxcmdLine&)> ExecuteCmd;
//
//   static hkxcmd* GetCommand(std::string name);
//   static list<hkxcmd*> GetCommand();
//   static string HelpString();
//   static bool ParseArgs(LPCTSTR line);
//   static bool ParseArgs(int argc, char **argv);
//   static void ParseLine(const char *start,char **argv,char *args,int *numargs,int *numchars);
//
//};


//#endif //__ADDIN_H
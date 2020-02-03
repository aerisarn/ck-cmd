//#include "stdafx.h"
//
//// Command Base
//#include <commands/CommandBase.h>
//
//class Project : public CommandBase<Project>
//{
//	COMMAND_PARAMETERS_LIST
//	{
//		//COMMAND_PARAMETER(bool, a);
//	};
//
//    REGISTER_COMMAND_HEADER(Project)
//
//private:
//    Project();
//    virtual ~Project();
//
//public:
//	static string GetName();
//	static string GetHelp();
//	static string GetHelpShort();
//
//protected:
//    virtual bool InternalRunCommand(const CommandSettings& settings);
//};
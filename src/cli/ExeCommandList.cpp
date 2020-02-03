#include <commands/CommandBase.h>

Cobalt::detail::Command::PointerType ExeCommandList::CommandList()
{
	static auto root = Cobalt::CreateCommand();
	root->Use = GetExeName();
	root->Short = GetExeName();
	root->Long = GetExeName();
	return root;
}

void ExeCommandList::AddCommand(Cobalt::detail::Command::PointerType cmd)
{

	CommandList()->AddCommand(cmd);
}

string ExeCommandList::GetExeName()
{
	return "ck-cmd";
}

string ExeCommandList::GetExeHelp()
{
	string usage;
	string cmdList;

	// Base exe usage format string
	// Command-specific usage must be included separately in the command's class
	usage = "Usage: " + GetExeName() + " <command> [<args> ...]\r\n";

	// Lists all available commands
	for( auto cmd : CommandList()->Commands)
	{
		cmdList += cmd->Use;
	}

	// Usage: EXE_NAME <command> [<args> ...]
	//
	// Commands:
	// COMMAND_NAME_1: COMMAND_HELP_SHORT_1
	// COMMAND_NAME_2: COMMAND_HELP_SHORT_2
	// [...]
	//
	// See "EXE_NAME help <command>" for more information on a specific command.
	return usage + "\r\nCommands: \r\n" + cmdList + "\r\nSee \"" + GetExeName() + " help <command>\" for more information on a specific command.";
}

string ExeCommandList::GetExeVersion()
{
	return "PLACEHOLDER VERSION";
}
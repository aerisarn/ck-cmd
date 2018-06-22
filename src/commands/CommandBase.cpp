#include <commands\CommandBase.h>

#include <core/log.h>



CommandBase::CommandBase()
{
    ExeCommandList::AddCommand(this);
}

CommandBase::~CommandBase()
{
}

bool CommandBase::RunCommand(int argc, char** argv)
{
    map<string, docopt::value> parsedArgs =
        docopt::docopt(GetHelp(), { argv, argv + argc }, true, ExeCommandList::GetExeVersion());

    return InternalRunCommand(parsedArgs);
}


void ExeCommandList::AddCommand(CommandBase* cmd)
{
    CommandList().emplace_back(cmd);
}

CommandBase* ExeCommandList::GetCommandByIndex(int index)
{
    return CommandList()[index];
}

CommandBase* ExeCommandList::GetCommandByName(string name)
{
    transform(name.begin(), name.end(), name.begin(), ::tolower);

    for each (CommandBase* cmd in CommandList())
    {
        string cmdName = cmd->GetName();
        transform(cmdName.begin(), cmdName.end(), cmdName.begin(), ::tolower);

        if (name.compare(cmdName) == 0)
            return cmd;
    }

    throw runtime_error("Invalid command. See \"ck-cmd help\" for a list of available commands.");
}

const vector<CommandBase*>& ExeCommandList::GetCommandList()
{
    return CommandList();
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
    for each (CommandBase* cmd in CommandList())
    {
        cmdList += (cmd->GetName() + ": " + cmd->GetHelpShort() + "\r\n");
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
#pragma once
#include "stdafx.h"

#ifdef USE_DOCOPT_CMD
#include <docopt.h>
#else
#undef min
#undef max
#include <cobalt.hpp>
#include <memory>
#endif

#include <core/log.h>


/* 
    CHECKLIST FOR IMPLEMENTING NEW COMMANDS USING COMMANDBASE
    For documentation on docopt see: https://github.com/docopt/docopt.cpp
    For examples see: https://github.com/docopt/docopt/tree/master/examples/git

    1.  Create the new command class and inherit CommandBase
    2.  Include the REGISTER_COMMAND_HEADER and REGISTER_COMMAND_CPP macros in the header and cpp respectively, or else your command won't be in the global command list
        Note: REGISTER_COMMAND_HEADER must be within the class definition
    3.  Make the default constructor and destructor private. This is not *absolutely* needed but the command classes use the singleton pattern, so it's just proper convention
    4.  Implement GetName(), GetHelp() and GetHelpShort() in your class
        GetName: The name of the command. Note this is the same string that will be checked for when parsing the arguments (case insensitive)
        GetHelp: The full help string for the command, according to docopt convention. This string will be displayed when "help <command>" is entered, and is also used to parse the arguments
        GetHelpShort: A single-line description for the command. This string will be displayed when just "help" is entered, listed alongside the other commands
    5.  Implement InternalRunCommand() in your class
        This function receives the docopt-parsed arguments that were entered, and is the starting point for the command
        For more information on how to use the parsed argument map, see the section below
    6.  Implement the command. If you've followed this checklist it should automatically be enabled for the command line. If you wish to disable a command just remove REGISTER_COMMAND_CPP from its header
*/

/*
    HELP MESSAGE REFERENCE
    TODO: I've cooked up a simple standard for our usage strings for the sake of implementing them, but feel free to analyze it and discuss alternatives if you feel so - gdn
    
    - Begin your usage string with "Usage: %exe_name% %command_name%" followed by the arguments. Example: "Usage: ck-cmd convert <path>"
    - List any *required* arguments first, surrounding each with <>. Example: "<inpath> <outpath>"
    - List any *optional* arguments after. Begin each optional argument with a prefix, followed by the argument itself. Wrap everything with []. Example: "[-o <path>] [-d <level>]"
        - Do not use the same prefix for different arguments. Bad example: "[-d <flags>] [-d <level>]"
        - However, multiple prefixes can share the same argument name. Example: "[-f <flags>] [-v <flags>]"
        - Prefixes can be either short or long. Examples: short: "[-d <level>]" long: "[--debug <level>]" However in an attempt to keep a simple standard try to avoid long ones.
        - To allow multiple arguments for the same prefix, add ellipsis after the argument. Example: "[-v <flags> ...]"
        - Finally, you can forego the argument completely to make it a simple (true|false) argument. Example: "[-d]"
    
    - In the next line, include a quick description of what the command does. This can be the same string from GetHelpShort() or a slightly more verbose explanation if needed.

    - After a blank line, include the required argument descriptions (if the command has any required arguments)
        - Begin with "Arguments:"
        - Proceed to list every required argument and a quick description for each. Example: "<inpath>  The path of the file to be converted"
        - *Note*: This section is not required by docopt, however it is being included here as a possible standard for this application

    - After another blank line, include the optional argument descriptions (if the command has any optional arguments)
        - Begin with "Options:"
        - Proceed to list every optional argument. 
            - Begin each entry with at least one whitespace. Recommended standard is 4 spaces.
            - Next include the prefix, including the hyphen(s)
            - Then include the argument, separated from the prefix by a single space. If the option has no argument, do not include anything here.
            - Finally, include the description, separated from the last previous word by at least two spaces.
            - The description can span multiple lines, as long as none of them begin with a whitespace followed by a hyphen (that's how docopt parses each optional argument)
            - In the description, you can add an automatic default option via the following format: "[default: default_string]". By standard include this as the last line of the description if needed.
            - Full example:
                -o <path>  Optional output file path
                           [default: out.txt]

    - Finally, include additional sections to describe flags, enumerations and anything else particular to the specific command.

    - For more examples, check the help strings of existing commands
*/

/*
    USING THE DOCOPT ARGUMENT MAP
    
    1.  Docopt returns a std::map with the option/argument strings as keys and a docopt::value as value
    2.  To view the contents of a required argument, access it by using the argument itself as the key. Example: "parsedArgs["<path>"]"
    3.  For optional arguments, use the prefix instead. Example: "parsedArgs["-d"]"
    3.  The resulting docopt::value contains three main methods to access its data:
        asBool(): wether the argument/option was included in the command line. Arguments that are required or have a default value will always return true
        asString(): the string argument (use this in most cases)
        asStringList():a list of multiple string arguments (use this if you allowed multiple arguments to a single prefix)
*/

/*
    COMMAND LINE NOTES
    TODO: Perhaps a more concise form of this should also be provided as a readme or in the base help string - gdn

    - If "-h" or "--help" is included anywhere in the arguments, docopt will ignore all other arguments and print the help string
        - If a command is passed before (-h|--help), the help for that command will be printed instead
        - This can be disabled if desired, for now it's on by default
    - Required arguments are passed directly as strings in the command line, and will be parsed in the order they are presented in the help string
    - Optional arguments are passed by including the prefix followed by the string value. This allows them to be passed in any order, and even be mixed with required arguments
        - If an option allows multiple entries, the same prefix can be repeated in the command line to add more entries

    - Examples of valid command lines for: example <req1> <req2> [-a <opt>] [-b <opt> ...] [-c]
    example "required1" "required2"
    example "required1" "required2" -a "optional1" -b "optional2" -c
    example "required1" "required2" -c -b "optional2" -a "optional1"
    example "required1" "required2" -b "optional2A" -b "optional2B"
    example -a "optional1" "required1" -b "optional2A" "required2" -b "optional2B"
*/

using namespace std;

#ifdef USE_DOCOPT_CMD
typedef map<string, docopt::value> CommandSettings;
#else
template<typename T>
struct CommandDefinition {
	T value;
	const char* name;
	const char* help;
	const char* parameter;
	bool optional;
};
//typedef Cobalt::Arguments CommandSettings;
#endif

//static classname instance;
//classname classname::instance{};
#define COMMAND_PARAMETERS_LIST public: struct CommandSettings
#define REGISTER_COMMAND_HEADER(classname) 	template<typename T> friend class CommandBase; static const classname& getInstance() { return instance; }
#define COMMAND_PARAMETER(type, name) CommandDefinition<type> name
#define FORMAL_COMMAND_PARAMETER(value, name, help) {value, name, help}

class ExeCommandList;

#include <magic_get.hpp>

// Base class for commands
template<typename T>
class CommandBase : public Cobalt::Command<T>
{
public:
	struct Flags {};

protected:



	template<typename V>
	static void convert_arg(const std::string& in, V& out) {
		std::stringstream convert(in);
		convert >> out;
	}

	template<>
	static void convert_arg(const std::string& in, bool& out) {
		std::stringstream convert(in);
		convert >> std::boolalpha >> out;
	}

	template <int First, int Last, typename P>
	struct static_for
	{
		void operator()(P& parsed_args, const Cobalt::Arguments& args) const
		{
			if (First < Last)
			{
				if (First < args.size())
					convert_arg(args.at(First), flat_get<First>(parsed_args));
				static_for<First + 1, Last, P>()(parsed_args, args);
			}
		}
	};

	template <int N, typename P>
	struct static_for<N, N, P>
	{
		void operator()(P& parsed_args, const Cobalt::Arguments& args) const
		{}
	};

	template <typename P>
	struct static_for<0, 0, P>
	{
		void operator()(P& parsed_args, const Cobalt::Arguments& args) const
		{}
	};


	void registerFlags()
	{
		//static_for<0, flat_tuple_size<T::CommandSettings>::value>()();
		//ForEach<flat_tuple_size<T::Parameters>::value>::item<0>();
	}

	CommandBase() {
		ExeCommandList::AddCommand(
			Cobalt::detail::Convert<T>()((T*)this)
		);
	}
	virtual ~CommandBase() {}

	T* command() { return  dynamic_cast<T*>(this); }

public:

	static T instance;

	std::string Use() {
		return T::GetName();
	}

	std::string Short() {
		return T::GetHelpShort();
	}

	std::string Long() {
		return T::GetHelp();
	}

	int Run(const Cobalt::Arguments& args) {
		T::CommandSettings parsed_args = {};
		static_for<0, flat_tuple_size<T::CommandSettings>::value, T::CommandSettings>()(parsed_args, args);
		return command()->InternalRunCommand(parsed_args);
	}
};

template<typename T>
T CommandBase<T>::instance = {};

class ExeCommandList
{
private:
	static Cobalt::detail::Command::PointerType CommandList();

public:
	static void AddCommand(Cobalt::detail::Command::PointerType cmd);

	static int Run(int argc, char** argv) {
		return CommandList()->Execute(argc, argv);
	}

	static string GetExeName();
	static string GetExeHelp();
	static string GetExeVersion();
};


static void HK_CALL ErrorReport(const char* msg, void* userContext)
{
    Log::Debug("%s", msg);
}


/*
EXAMPLE:

	COMMAND_PARAMETERS_LIST
	{
		COMMAND_PARAMETER(bool, a);
	};

	...

FORMAL_PARAMETERS_LIST(About)
{
	FORMAL_COMMAND_PARAMETER(true, "a", "a bool"),
};

*/
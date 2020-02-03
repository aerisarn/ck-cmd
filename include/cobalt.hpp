/**
   Copyright 2017 The Cobalt Authors.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.)
 */

#ifndef COBALT_HPP_INCLUDED
#define COBALT_HPP_INCLUDED

#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include <utility>
#include <functional>
#include <tuple>
#include <memory>
#include <algorithm>
#include <numeric>
#include <cctype>
#include <stdexcept>
#include <typeinfo>
#include <typeindex>

namespace Cobalt {

/**
    Typedefs for used types
 */
typedef std::vector<std::string> Arguments;

/**
    Macro to easily generate an exception type that can be thrown by Cobra
 */
#define COBALT_ERROR(name, message) \
    class name##Exception : std::runtime_error { \
    public: \
        name##Exception() : std::runtime_error(message) { } \
        name##Exception(const std::string& msg) : std::runtime_error(msg) { } \
    };

/**
    Utilities
 */

struct Utilities {

	static bool iequals(const std::string& a, const std::string& b)
	{
		return std::equal(a.begin(), a.end(),
			b.begin(), b.end(),
			[](char a, char b) {
				return tolower(a) == tolower(b);
			});
	}

	// Add padding to a string
	static std::string Rpad(std::string string, int size = 20) {
		while (string.size() < size) string += " ";
		return string;
	}

	static std::string Ltrim(std::string string) {
		while (string.size() > 0 && (string[0] == ' ' || string[0] == '\n' || string[0] == '\r' || string[0] == '\t')) string = string.substr(1);
		return string;
	}

	static std::string Rtrim(std::string string) {
		while (string.size() > 0 && (string[string.size() - 1] == ' ' || string[string.size() - 1] == '\n' || string[string.size() - 1] == '\r' || string[string.size() - 1] == '\t')) string = string.substr(0, string.size() - 1);
		return string;
	}

	static std::string Trim(std::string string) {
		return Rtrim(Ltrim(string));
	}

	static std::string ToLowerCase(const std::string& s) {
		std::string result = s;
		std::transform(result.begin(), result.end(), result.begin(), (int(*)(int)) std::tolower);
		return result;
	}

	// Convert standard C style args to a vector of strings.
	static Arguments ConvertArguments(int argc, char** argv) {
		Arguments args;
		for (int i = 1; i < argc; ++i) {
			args.push_back(argv[i]);
		}
		return args;
	}

	static void PrintArguments(Arguments args) {
		for (auto& arg : args) {
			std::cout << arg << std::endl;
		}
	}

	static Arguments StripFlags(Arguments args, std::map<std::string, std::string>* flags = nullptr) {
		std::pair<std::string, std::string> newFlag;

		for (int i = 0; i < args.size(); ++i) {
			auto arg = Trim(args[i]);

			if (arg.find("--") != std::string::npos || arg.find("-") != std::string::npos) {
				newFlag.first = arg;

				// Check for =
				int pos = arg.find("=");
				if (pos != std::string::npos) {
					newFlag.first = arg.substr(0, pos);
					newFlag.second = arg.substr(pos + 1);
				}
				else {
					newFlag.second = "true";
				}

				// If in the short form AND the param is a switch, split
				if (Trim(newFlag.first)[0] == '-' && newFlag.second == "true") {
					for (int i = 1; i < newFlag.first.length(); ++i) {
						if (flags) {
							flags->insert({ "-" + std::string(1, newFlag.first[i]), "true" });
						}
					}
				}
				else {
					if (flags) flags->insert(newFlag);
				}

				// Remove the argument from the list
				args.erase(args.begin() + i);
				--i;
			}
		}

		return args;
	}

	/**
		Calculate the Levenshtein distance between two strings.

		See https://wikipedia.org/Levenshtein_Distance
	 */
	static int LevenshteinDistance(std::string s, std::string t, bool ignoreCase) {
		if (ignoreCase) {
			s = ToLowerCase(s);
			t = ToLowerCase(t);
		}

		int s1len = s.size();
		int s2len = t.size();

		auto column_start = (decltype(s1len))1;

		auto column = new decltype(s1len)[s1len + 1];
		std::iota(column + column_start, column + s1len + 1, column_start);

		for (auto x = column_start; x <= s2len; x++) {
			column[0] = x;
			auto last_diagonal = x - column_start;
			for (auto y = column_start; y <= s1len; y++) {
				auto old_diagonal = column[y];
				auto possibilities = {
					column[y] + 1,
					column[y - 1] + 1,
					last_diagonal + (s[y - 1] == t[x - 1] ? 0 : 1)
				};
				column[y] = std::min(possibilities);
				last_diagonal = old_diagonal;
			}
		}
		auto result = column[s1len];
		delete[] column;
		return result;
	}
};

// Error messages
COBALT_ERROR(UnknownType, "The data type is not known");
COBALT_ERROR(UnknownFlag, "Unknown flag");
COBALT_ERROR(WrongType, "Cannot convert flag to this type");
COBALT_ERROR(NotRunnable, "The command is not runnable");
COBALT_ERROR(TalkbackNotFound, "The talkback pointer was not set");

/**
    The internal part to get commands going
 */
namespace detail {

/**
    Flags can have the following types.

    With the help of template classes we convert the type into an enum value
    and provide a method to parse the string into the desired type
 */
enum class Types {
    BOOL,
    INT,
    FLOAT,
    CHAR,
    STRING,

    UNDETERMINED
};

template<typename T>
struct TypeToEnum;

template<>
struct TypeToEnum<bool> {
    static Types Value() { return Types::BOOL; }
    static void Convert(bool& r, std::string value) { r = (value == "true") ? true : false;  }
    static std::string From(bool v) { return (v) ? "true" : "false";  }
};

template<>
struct TypeToEnum<char> {
    static Types Value() { return Types::CHAR; }
    static void Convert(char& r, std::string value) { r = (value.size() > 0) ? value[0] : static_cast<char>(0); }
    static std::string From(char c) { return std::string(1, c); }
};

template<>
struct TypeToEnum<int> {
    static Types Value() { return Types::INT; }
    static void Convert(int& r, std::string value) { r = std::stoi(value); }
    static std::string From(int i) {return std::to_string(i); }
};

template<>
struct TypeToEnum<short> {
    static Types Value() { return Types::INT; }
    static void Convert(short& r, std::string value) { r = std::stoi(value); }
    static std::string From(short i) {return std::to_string(i); }
};

template<>
struct TypeToEnum<long> {
    static Types Value() { return Types::INT; }
    static void Convert(long& r, std::string value) { r = std::stoi(value); }
    static std::string From(long i) {return std::to_string(i); }
};

template<>
struct TypeToEnum<long long> {
    static Types Value() { return Types::INT; }
    static void Convert(long long& r, std::string value) { r = std::stoi(value); }
    static std::string From(long long i) {return std::to_string(i); }
};

template<>
struct TypeToEnum<float> {
    static Types Value() { return Types::FLOAT; }
    static void Convert(float& r, std::string value) { r = std::stof(value); }
    static std::string From(float i) {return std::to_string(i); }
};

template<>
struct TypeToEnum<double> {
    static Types Value() { return Types::FLOAT; }
    static void Convert(double& r, std::string value) { r = std::stof(value); }
    static std::string From(double i) {return std::to_string(i); }
};

template<>
struct TypeToEnum<std::string> {
    static Types Value() { return Types::STRING; }
    static void Convert(std::string& r, std::string value) { r = value; }
    static std::string From(std::string s) { return s; }
};

/**
    Flag is the internal class to describe a CLI flag.
 */
struct Flag {
    // Type of the flag.
    Types Type;
    // Short name of the flag.
    std::string Short;
    // Long name of the flag.
    std::string Long;
    // Description of the flag that will be printed in the usage and help messages.
    std::string Description;
    // Method to set the value
    std::function<void(std::string)> Setter;
    // The bare value
    std::string BareValue;

    std::string Usage() const {
        std::string result;

        result += "--" + Long;

        if (!Short.empty()) {
            result += ", -" + Short;
        }

        result = Utilities::Rpad(result, 20);

        result += Description;

        return result;
    }
};


class Flags {
public:
    void Add(Types type, std::string Long, std::string Short, std::string Description, std::function<void(std::string)> Setter) {
        std::shared_ptr<Flag> flag = std::make_shared<Flag>();

        flag->Type = type;

        flag->Short         = Short;
        flag->Long          = Long;
        flag->Description   = Description;

        flag->Setter        = Setter;

        flags.push_back(std::move(flag));
    }

    template<typename T>
    void Add(std::string Long, std::string Short, T Default, std::string Description, std::function<void(std::string)> Setter) {
        Add(TypeToEnum<T>::Value(), Long, Short, Description, Setter);

        // Set the bare value to the default
        Lookup(Long)->BareValue = Default;

        // Set to the default value
        Setter(TypeToEnum<T>::From(Default));
    }

    /**
        Method to set the provided reference to the parsed value.
     */
    template<typename T>
    void Add(T& Reference, std::string Long, std::string Short, std::string Description) {
        Add(TypeToEnum<T>::Value(), Long, Short, Description, std::bind(TypeToEnum<T>::Convert, std::ref(Reference), std::placeholders::_1));
    }

    template<typename T>
    void Add(T& Reference, std::string Long, std::string Short, T Default, std::string Description) {
        // Add the flag to the list
        Add(Reference, Long, Short, Description);

        // Initialize the default value
        Reference = Default;
    }

    void Add(std::shared_ptr<Flag> flag) {
        flags.push_back(std::move(flag));
    }

    std::shared_ptr<Flag> Lookup(std::string name) {
        auto it = std::find_if(flags.begin(), flags.end(), [&] (std::shared_ptr<Flag> f) {
            return f->Long == name;
        });

        return (it != flags.end()) ? *it : nullptr;
    }

    std::shared_ptr<const Flag> Lookup(std::string name) const {
        auto it = std::find_if(flags.begin(), flags.end(), [&] (std::shared_ptr<Flag> f) {
            return f->Long == name;
        });

        return (it != flags.end()) ? *it : nullptr;
    }

    size_t Size() const { return flags.size(); }

    typedef std::vector<std::shared_ptr<Flag>>::iterator   iterator_t;
    typedef std::vector<std::shared_ptr<Flag>>::const_iterator const_iterator_t;

    iterator_t begin() { return flags.begin(); }
    iterator_t end() { return flags.end(); }

    const_iterator_t begin() const { return flags.begin(); }
    const_iterator_t end() const { return flags.end(); }
public:
    void Parse(std::map<std::string, std::string> args) {
        // Iterate over all flags
        for (auto& pair : args) {
            std::shared_ptr<Flag> flag;

            std::string name = (pair.first[1] == '-') ? pair.first.substr(2) : pair.first.substr(1);

            if (pair.first[1] == '-') {
                auto it = std::find_if(flags.begin(), flags.end(), [&](std::shared_ptr<Flag> flag) {
                    return flag->Long == name;
                });

                flag = (it != flags.end()) ? *it : nullptr;
            } else {
                auto it = std::find_if(flags.begin(), flags.end(), [&](std::shared_ptr<Flag> flag) {
                    return flag->Short == name;
                });

                flag = (it != flags.end()) ? *it : nullptr;
            }

            if (!flag) {
                throw UnknownFlagException("Unknown flag: " + name);
            }

            // Call the setter with the argument
            flag->BareValue = pair.second;
            flag->Setter(pair.second);
        }
    }
private:
    std::vector<std::shared_ptr<Flag>> flags;
public:
    bool ContinueOnError;
};

/**
    Internal class for a command. This is were the real magic happens.
 */
class Command : public std::enable_shared_from_this<Command> {
public:
    typedef std::function<int(const Arguments&)>    HookType;
    typedef std::function<void(const Arguments&)>   PrePostHookType;
    typedef std::shared_ptr<Command>                PointerType;
public:
    // The one-line usage message.
    std::string Use;
    // An array of aliases that can be used instead of the first word in use.
    std::vector<std::string> Aliases;
    // The short description shown in the 'help' output.
    std::string Short;
    // The long message shown in the 'help <this-command>' output.
    std::string Long;
    // Examples of how to use the command.
    std::string Example;
    // Is this command deprecated and should print this string when used?
    std::string Deprecated;
    // Is this command hidden and should NOT show up in the list of available commands?
    bool Hidden=false;
    // Annotations are key/value pairs that can be used by applications to identify or
    // group command
    std::map<std::string, std::string> Annotations;

    // Set of flags children of this command will inherit
    Flags PersistentFlags;
    // Flags that are declared specifically by this command (not inherited)
    Flags LocalFlags;

    // SilenceErrors is an option to quiet errors down stream.
    bool SilenceErrors;
    // SilenceUsage is an option to silence usage when an error occurs.
    bool SilenceUsage;

    // PersistentPreRun: children of this command will inherit and execute.
    PrePostHookType PersistentPreRun;
    // PreRun: children of this command will not inherit.
    PrePostHookType PreRun;
    // Run: Typically the actual work. Most command will implement this.
    HookType Run;
    // PostRun: run after the Run command.
    PrePostHookType PostRun;
    // PersistentPostRun: children of this command will inherit and execute after PostRun.
    PrePostHookType PersistentPostRun;

    // The parent command
    PointerType Parent;
    // The subcommands
    std::vector<PointerType> Commands;

    // The output stream
    std::ostream* Output;

    // We can furthermore attach arbitrary data to the command
    void* Data;

    // The subcommands are already sorted alphabetically.
    bool IsSorted;
public:
    Command() {
        Output = &std::cout;
    }
protected:
    PointerType GetThisPointer() { return shared_from_this(); }
    std::shared_ptr<const Command> GetThisPointer() const { return shared_from_this(); }
public:
    /**
        Extract the name of a command from the usage line.
        It is either the first string or the whole.
     */
    std::string Name() const {
        auto pos = Use.find(" ");
        if (pos == std::string::npos) return Use;
        return Use.substr(0,pos);
    }

    /**
        Usage puts out the usage for the command.
        Used when a user provides invalid input.
     */
    void Usage() {
        std::stringstream ss;

        ss << "Usage:" << std::endl;

        // Print the main usage
        if (IsRunnable()) {
            if (HasAvailableFlags()) {
                ss << "  " << UseLine() << " [flags]" << std::endl;
            } if (HasAvailableSubCommands()) {
                ss << "  " << CommandPath() << " [command]" << std::endl;
            } else {
                ss << "  " << UseLine() << std::endl;
            }
        } else {
            if (HasAvailableSubCommands()) {
                ss << "  " << CommandPath() << " [command]" << std::endl;
            }
        }

        // Aliases
        if (Aliases.size() > 0) {
            ss << std::endl << "Aliases:" << std::endl;
            ss << "  " << Name() << std::endl;
            for (auto& alias : Aliases) {
                ss << "  " << alias << std::endl;
            }
        }

        // Example
        if (HasExample()) {
            ss << std::endl << "Example:" << std::endl;
            ss << "  " << Example << std::endl;
        }

        // Available commands
        if (HasAvailableSubCommands()) {
            ss << std::endl << "Available commands:" << std::endl;
            for (auto& cmd : Commands) {
                if (cmd->IsAvailableCommand() && cmd->Name() != "help") {
                    ss << "  " << Utilities::Rpad(cmd->Name(), 20) << cmd->Short << std::endl;
                }
            }
        }

        // Flags
        if (LocalFlags.Size() > 0) {
            ss << std::endl << "Flags:" << std::endl;
            for (auto& flag : LocalFlags) {
                ss << "  " << flag->Usage() << std::endl;
            }
        }

        // Global Flags
        MergePersistentFlags();
        if (PersistentFlags.Size() > 0) {
            ss << std::endl << "Global Flags:" << std::endl;
            for (auto& flag : PersistentFlags) {
                ss << "  " << flag->Usage() << std::endl;
            }
        }

        // Additional help topics
        if (HasHelpSubCommands()) {
            ss << std::endl << "Additional help topics:" << std::endl;
            for (auto& cmd : Commands) {
                if (cmd->IsAdditionalHelpTopicCommand()) {
                    ss << "  " << Utilities::Rpad(cmd->CommandPath(), 20) << cmd->Short << std::endl;
                }
            }
        }

        // Help notice
        if (HasAvailableSubCommands()) {
            ss << std::endl << "Use \"" << CommandPath() << " [command] --help\" for more information about a command." << std::endl;
        }

        // Output the result
        (*Output) << ss.str();
    }

    /**
        Help puts out the help for the command.
        Used when a user calls help [command].
     */
    void Help() {
        std::stringstream ss;

        if (Short.size() > 0) {
            ss << Short << std::endl << std::endl;
        }

        if (Long.size() > 0) {
            ss << Long << std::endl << std::endl;
        }

        if (IsRunnable() || HasSubCommands()) {
            ss << UsageString();
        }

        (*Output) << ss.str();
    }

    /**
        Return all the flags that were inherited from parents
     */
    Flags InheritedFlags() {
        Flags result;

        std::function<void(PointerType)> rmerge = [&](PointerType cmd) {
            if (cmd->PersistentFlags.Size() > 0) {
                for (auto& flag : cmd->PersistentFlags) {
                    if (!result.Lookup(flag->Long)) {
                        result.Add(flag);
                    }
                }

                if (cmd->Parent) rmerge(cmd->Parent);
            }
        };

        if (Parent) rmerge(Parent);

        return result;
    }

    Flags FullFlags() {
        MergePersistentFlags();

        Flags result = LocalFlags;

        for (auto& flag : PersistentFlags) {
            if (!result.Lookup(flag->Long)) {
                result.Add(flag);
            }
        }

        return result;
    }

    bool HasAvailableFlags() {
        return FullFlags().Size() > 0;
    }

    void MergePersistentFlags() {
        std::function<void(PointerType)> rmerge = [&](PointerType cmd) {
            if (cmd->PersistentFlags.Size() > 0) {
                for (auto& flag : cmd->PersistentFlags) {
                    if (!PersistentFlags.Lookup(flag->Long)) {
                        PersistentFlags.Add(flag);
                    }
                }
            }

            if (cmd->Parent) {
                rmerge(cmd->Parent);
            }
        };

        rmerge(GetThisPointer());
    }

    void SortSubCommands() {
        // If the commands are sorted, return
        if (IsSorted) return;

        std::sort(Commands.begin(), Commands.end(), [&](const PointerType& a, const PointerType& b) {
            std::string aName = a->Name();
            std::string bName = b->Name();
            return std::lexicographical_compare(aName.begin(), aName.end(), bName.begin(), bName.end());
        });

        IsSorted = true;
    }

    std::string UsageString() {
        // Cache the old output
        std::ostream* tmpOutput = Output;

        // Set the output to a stringstream
        std::stringstream ss;
        Output = &ss;

        // Print usage message
        Usage();

        // Reset the output
        Output = tmpOutput;

        return ss.str();
    }

    /**
        CommandPath returns the full path to this command.
     */
    std::string CommandPath() const {
        std::string path = Name();
        auto x = GetThisPointer();
        while (x->Parent) {
            path = x->Parent->Name() + " " + path;
            x = x->Parent;
        }
        return path;
    }

    /**
        UseLine puts out the full usage for a given command (including parents).
     */
    std::string UseLine() const {
        std::string line = "";
        if (Parent) {
            line = Parent->CommandPath() + " ";
        }
        return line + Use;
    }

    bool HasAlias(std::string name) const {
        for (auto& alias : Aliases) {
            if (Utilities::iequals(alias,name)) return true;
        }
        return false;
    }

    bool HasExample() const {
        return Example.size() > 0;
    }

    bool IsRunnable() const {
        return (Run != nullptr);
    }

    bool IsAvailableCommand() const {
        if (Deprecated.size() > 0 || Hidden) return false;

        //if (Name() == "help") return false;

        if (IsRunnable() || HasAvailableSubCommands()) return true;

        return false;
    }

    bool HasSubCommands() const {
        return Commands.size() > 0;
    }

    bool HasAvailableSubCommands() const {
        for (auto& cmd : Commands) {
            if (cmd->IsAvailableCommand()) return true;
        }
        return false;
    }

    bool IsAdditionalHelpTopicCommand() const {
        if (IsRunnable() || Deprecated.size() > 0 || Hidden) return false;

        // if any non-help sub command is found, the command is not a 'help' command
        for (auto& cmd : Commands) {
            if (!cmd->IsAdditionalHelpTopicCommand()) return false;
        }

        return true;
    }

    bool HasHelpSubCommands() const {
        for (auto& cmd : Commands) {
            if (cmd->IsAdditionalHelpTopicCommand()) return true;
        }

        return false;
    }

    std::vector<std::string> SuggestionsFor(std::string name) const {
        std::vector<std::string> suggestions;

        for (auto& cmd : Commands) {
            if (!cmd->IsAvailableCommand()) continue;

            int distance = Utilities::LevenshteinDistance(name, cmd->Name(), true);
            bool suggestByLevenshtein = distance <= 2;
            bool suggestByPrefix = Utilities::ToLowerCase(cmd->Name()).substr(0,name.size()) == Utilities::ToLowerCase(name);
            if (suggestByLevenshtein || suggestByPrefix) {
                suggestions.push_back(cmd->Name());
            }
        }

        return suggestions;
    }
public:
    void AddCommand(PointerType cmd) {
        IsSorted = false;
        cmd->Parent = GetThisPointer();
        Commands.push_back(cmd);

        SortSubCommands();
    }

    void InjectHelpCommand() {
        auto root = Root();

        auto cmdHelp = std::make_shared<Command>();
        cmdHelp->Use = "help [command]";
        cmdHelp->Short = "Help about any command";
        cmdHelp->Long = "Help provides help for any command in the application.\nSimply type " + Name() + " help [path to command] for details.";
        cmdHelp->Run = std::bind([&](std::shared_ptr<Command> root, const Cobalt::Arguments& args) {
            Cobalt::Arguments a = args;
            auto tmp = root->Find(a);

            // Print the help file
            tmp->Help();
            return 0;
        }, root, std::placeholders::_1);

        root->AddCommand(std::move(cmdHelp));
    }

    std::shared_ptr<Command> Find(Arguments& args) {
        PointerType tmp = GetThisPointer();

        while (true) {
            if (args.size() == 0) break;

            // If no subcommands available, break
            if (!HasAvailableSubCommands()) break;

            // Get the first argument
            auto arg = args[0];

            bool cmdFound = false;

            // Iterate over all SubCommands
            for (auto& cmd : tmp->Commands) {
                if (!cmd->IsAvailableCommand()) continue;

                if (Utilities::iequals(cmd->Name(),arg) || cmd->HasAlias(arg)) {
                    args.erase(args.begin());
                    tmp = cmd;
                    cmdFound = true;
                    break;
                }
            }

            if (!cmdFound) break;
        }

        return tmp;
    }

    void ExecutePersistentPreHooks(Arguments args) const {
        std::vector<PrePostHookType> hooks;

        // Collect all persistent pre hooks
        auto tmp = GetThisPointer();

        while (tmp) {
            if (tmp->PersistentPreRun) hooks.insert(hooks.begin(), tmp->PersistentPreRun);
            tmp = tmp->Parent;
        }

        // Execute all
        for (auto& hook : hooks) {
            hook(args);
        }
    }

    void ExecutePersistentPostHooks(Arguments args) const {
        // Collect all persistent pre hooks
        auto tmp = GetThisPointer();

        while (tmp) {
            if (tmp->PersistentPostRun) tmp->PersistentPostRun(args);
            tmp = tmp->Parent;
        }
    }

    int Execute(Arguments args) {
        // Inject the help command into the root command
        InjectHelpCommand();

        // No matter which command was used for parsing, start at root
        if (Parent) {
            return Root()->Execute(args);
        }

        // Strip all flags from the arguments
        std::map<std::string, std::string> flags;
        args = Utilities::StripFlags(args, &flags);

        // Has help flag?
        if (flags.find("--help") != flags.end() || flags.find("-h") != flags.end()) {
            args.insert(args.begin(), "help");
            return Execute(args);
        }

        // Climb through the graph until the final subcommand is found
        PointerType tmp = Find(args);

        if (tmp == Root() && tmp->HasAvailableSubCommands() && args.size() > 0) {
            // The command was not found, print suggestions
            std::string suggestionsString = "";

            auto suggestions = tmp->SuggestionsFor(args[0]);
            if (suggestions.size() > 0) {
                suggestionsString += "\n\nDid you mean this?\n";
                for (auto& s : suggestions) {
                    suggestionsString += "   " + s + "\n";
                }
            }

            std::cerr << "Unknown command " << args[0] << " for " << tmp->CommandPath() << suggestionsString;
            return -1;
        }

        if (tmp->IsRunnable()) {
            // Parse the flags and set all the parameters
            try {
                tmp->FullFlags().Parse(flags);
            } catch (...) {
                if (!tmp->FullFlags().ContinueOnError) {
                    tmp->Usage();
                    return -1;
                }
            }

            // Run the command
            try {
                // Execute all persistent pre run hooks
                ExecutePersistentPreHooks(args);

                // Execute the pre run hook, if set
                if (PreRun) PreRun(args);

                // Execute the actual run code
                int code = tmp->Run(args);

                // Execute the local post hook
                if (PostRun) PostRun(args);

                // Execute the persistent post hooks
                ExecutePersistentPostHooks(args);

                // Return the result code
                return code;
            } catch (NotRunnableException& e) {
                tmp->Usage();
                return 0;
            }
        } else {
            tmp->Usage();
            return 0;
        }
    }

    int Execute(int argc, char** argv) {
        // Convert the arguments
        Arguments args = Utilities::ConvertArguments(argc, argv);

        return Execute(args);
    }
public:
    PointerType Root() {
        if (!Parent) return GetThisPointer();
        return Parent->Root();
    }

    std::shared_ptr<const Command> Root() const {
        // No parent? Then this is the root
        if (!Parent) return GetThisPointer();
        return Parent->Root();
    }
};


/**
    This meta-programming class is used to convert a Command
    implemented in an extra class to better structure code
    into the internal Command representation implemented above.
 */
template<class... Commands>
struct Convert;

template<class... Commands>
struct Join;

template<class T>
struct Convert<T> {
    typedef typename Command::PointerType CommandType;

    CommandType operator()() {
        CommandType cmd = std::make_shared<Command>();

        // Allocate the data
        T* data = new T();
        cmd->Data = (void*) (data);

        // Establish the talkback for the DSL command class
        data->data = cmd;

        cmd->Use            = data->Use();
        cmd->Long           = data->Long();
        cmd->Short          = data->Short();
        cmd->Example        = data->Example();
        cmd->Deprecated     = data->Deprecated();
        cmd->Hidden         = data->Hidden();
        cmd->SilenceErrors  = data->SilenceErrors();
        cmd->SilenceUsage   = data->SilenceUsage();
        cmd->Annotations    = data->Annotations();
        cmd->Aliases        = data->Aliases();

        cmd->PersistentPreRun     = std::bind([](CommandType cmd, const Arguments& args) -> void {
            static_cast<T*>(cmd->Data)->PersistentPreRun(args);
        }, cmd, std::placeholders::_1);

        cmd->PreRun     = std::bind([](CommandType cmd, const Arguments& args) -> void {
            static_cast<T*>(cmd->Data)->PreRun(args);
        }, cmd, std::placeholders::_1);

        cmd->Run     = std::bind([](CommandType cmd, const Arguments& args) -> int {
            return static_cast<T*>(cmd->Data)->Run(args);
        }, cmd, std::placeholders::_1);

        cmd->PostRun     = std::bind([](CommandType cmd, const Arguments& args) -> void {
            static_cast<T*>(cmd->Data)->PostRun(args);
        }, cmd, std::placeholders::_1);

        cmd->PersistentPostRun     = std::bind([](CommandType cmd, const Arguments& args) -> void {
            static_cast<T*>(cmd->Data)->PersistentPostRun(args);
        }, cmd, std::placeholders::_1);

        /** Register flags **/
        data->RegisterFlags();

        /** Automatically create children for the command **/
        cmd = T::CreateChildren(cmd);

        return std::move(cmd);
    }

	CommandType operator()(T* data) {
		CommandType cmd = std::make_shared<Command>();

		cmd->Data = (void*)(data);

		// Establish the talkback for the DSL command class
		data->data = cmd;

		cmd->Use = data->Use();
		cmd->Long = data->Long();
		cmd->Short = data->Short();
		cmd->Example = data->Example();
		cmd->Deprecated = data->Deprecated();
		cmd->Hidden = data->Hidden();
		cmd->SilenceErrors = data->SilenceErrors();
		cmd->SilenceUsage = data->SilenceUsage();
		cmd->Annotations = data->Annotations();
		cmd->Aliases = data->Aliases();

		cmd->PersistentPreRun = std::bind([](CommandType cmd, const Arguments& args) -> void {
			static_cast<T*>(cmd->Data)->PersistentPreRun(args);
			}, cmd, std::placeholders::_1);

		cmd->PreRun = std::bind([](CommandType cmd, const Arguments& args) -> void {
			static_cast<T*>(cmd->Data)->PreRun(args);
			}, cmd, std::placeholders::_1);

		cmd->Run = std::bind([](CommandType cmd, const Arguments& args) -> int {
			return static_cast<T*>(cmd->Data)->Run(args);
			}, cmd, std::placeholders::_1);

		cmd->PostRun = std::bind([](CommandType cmd, const Arguments& args) -> void {
			static_cast<T*>(cmd->Data)->PostRun(args);
			}, cmd, std::placeholders::_1);

		cmd->PersistentPostRun = std::bind([](CommandType cmd, const Arguments& args) -> void {
			static_cast<T*>(cmd->Data)->PersistentPostRun(args);
			}, cmd, std::placeholders::_1);

		/** Register flags **/
		data->RegisterFlags();

		/** Automatically create children for the command **/
		cmd = T::CreateChildren(cmd);

		return std::move(cmd);
	}
};

template<class Child, class... Children>
struct Join<Child, Children...> {
    typedef typename Command::PointerType CommandType;

    inline CommandType operator()(CommandType cmd) {
        return std::move(Join<Children...>()(std::move(Join<Child>()(cmd))));
    }
};

template<class Child>
struct Join<Child> {
    typedef typename Command::PointerType CommandType;

    inline CommandType operator()(CommandType cmd) {
        auto subcmd = Convert<Child>()();
        cmd->AddCommand(std::move(subcmd));
        return std::move(cmd);
    }
};

template<>
struct Join<> {
    typedef typename Command::PointerType CommandType;

    inline CommandType operator()(CommandType cmd) {
        return std::move(cmd);
    }
};

template<class Parent, class... Children>
struct Convert<Parent, Children...> {
    typedef typename Command::PointerType CommandType;

    inline CommandType operator()() {
        return std::move(Join<Children...>(std::move(Convert<Parent>()())));
    }
};

} /* namespace detail */

/**
    \class Command

    A class that can be used to structure the Cobalt code in
    a better fashion. For large projects the way to go is to
    save each command in another header and then register them
    in the main file.

    Example:

        // cmd/serve.hpp

        class PrintCommand : public Cobalt::Command<PrintCommand> {
        public:
            static std::string Use() {
                return "print [text to print]";
            }

            static std::string Short() {
                return "Print the given text to screen";
            }

            static std::string Long() {
                return "Print the given text to screen.";
            }

            int Run(const Cobalt::Arguments& args) {
                for (auto& arg : args) {
                    std::cout << arg << " ";
                }
                std::cout << std::endl;
            }
        };

        // cmd/root.hpp
        class RootCommand : public Cobalt::Command<RootCommand, PrintCommand> {
        public:
            static std::string Use() {
                return "echo";
            }
        };

        // main.cpp

        ...

        int main(int argc, char** argv) {
            return Cobalt::Parse<RootCommand>(argc, argv)();
        }
 */
template<class This, class... Children>
class Command {
public:
    static std::string Use() {
        return "";
    }

    static std::string Short() {
        return "";
    }

    static std::string Long() {
        return "";
    }

    static std::string Example() {
        return "";
    }

    static std::string Deprecated() {
        return "";
    }

    static std::vector<std::string> Aliases() {
        return {};
    }

    static std::map<std::string, std::string> Annotations() {
        return {};
    }

    static bool Hidden() {
        return false;
    }

    static bool SilenceErrors() {
        return false;
    }

    static bool SilenceUsage() {
        return false;
    }

    void PersistentPreRun(const Arguments& args) {
        // do nothing
    }

    void PreRun(const Arguments& args) {
        // do nothing
    }

    int Run(const Arguments& args) {
        throw NotRunnableException();
        return -1;
    }

    void PostRun(const Arguments& args) {
        // do nothing
    }

    void PersistentPostRun(const Arguments& args) {
        // do nothing
    }
public:
    void RegisterFlags() {
        // do nothing
    }

    template<typename T>
    void AddPersistentFlag(T& Ref, std::string Long, std::string Short, T Default, std::string Description) {
        data->PersistentFlags.Add<T>(Ref, Long, Short, Default, Description);
    }

    void AddPersistentFlag(detail::Types Type, std::string Long, std::string Short, std::string Description) {
        data->PersistentFlags.Add(Type, Long, Short, Description);
    }

    template<typename T>
    void AddLocalFlag(T& Ref, std::string Long, std::string Short, T Default, std::string Description) {
        data->LocalFlags.Add<T>(Ref, Long, Short, Default, Description);
    }

    template<typename T>
    void AddLocalFlag(T& Ref, std::string Long, std::string Short, std::string Description) {
        data->LocalFlags.Add<T>(Ref, Long, Short, Description);
    }

    template<typename T>
    T Lookup(std::string name) {
        if (!data) throw TalkbackNotFoundException();

        // Lookup the names in all the flags, local and persistent
        auto flag = data->FullFlags().Lookup(name);

        // If the flag was not found, throw an exception
        if (!flag) throw UnknownFlagException();

        T result;

        // Convert to the result type
        detail::TypeToEnum<T>::Convert(result, flag->BareValue);

        return result;
    }
public:
    static std::shared_ptr<detail::Command> CreateChildren(std::shared_ptr<detail::Command> cmd) {
        return detail::Join<Children...>()(cmd);
    }
private:
    friend class detail::Convert<This>;
    std::shared_ptr<detail::Command> data;
};

template<class T>
using CreateCommandFromClass = detail::Convert<T>;

inline typename detail::Command::PointerType CreateCommand() {
    detail::Command::PointerType cmd = std::make_shared<detail::Command>();
    return std::move(cmd);
}

template<class Root, class... Children>
struct Execute {
    int argc;
    char** argv;

    Execute(int argc, char** argv) : argc(argc), argv(argv) {  }

    // Convert the command and execute it
    inline int operator()() {
        auto cmd = detail::Convert<Root, Children...>()();
        return cmd->Execute(argc, argv);
    }

    // Employ the fact that the result of a main method has to be an integer
    inline operator int() {
        return (*this)();
    }
};

// Undefine the COBALT_ERROR macro
#undef COBALT_ERROR

} /* namespace Cobalt */
#endif /* COBALT_HPP_INCLUDED */

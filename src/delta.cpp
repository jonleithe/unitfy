/*
 * filename:    delta.cpp
 * created:     26 April 2026
 * author:      jole
 * description: Core application logic: REPL and CLI mode, input parsing,
 *              and dispatching to quantity conversion routines.
 */

#include <cctype>
#include <cstdio>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <iostream>

#if defined(HAVE_READLINE)
    #include <readline/history.h>
    #include <readline/readline.h>
#endif

#include "delta.h"



namespace unitfy {

#if defined(HAVE_READLINE)
namespace {

// Single-token aliases are used for REPL tab-completion to avoid partial
// replacements when the user is mid-way through multi-word units.
static constexpr const char* kReplCommands[] = {
    "help",
    "exit",
    "quit",
};

static constexpr const char* kUnitAliases[] = {
    "c", "celsius", "f", "fahrenheit", "k", "kelvin", "r", "rankine",
    "mm", "millimeter", "millimeters", "cm", "centimeter", "centimeters",
    "m", "meter", "meters", "km", "kilometer", "kilometers", "in", "inch", "inches",
    "ft", "foot", "feet", "usft", "surveyfoot", "surveyfeet", "ukft", "imperialfoot", "imperialfeet",
    "mi", "mile", "miles",
    "ml", "milliliter", "milliliters", "l", "liter", "liters", "gal", "gallon", "gallons",
    "floz", "fluidounce", "fluidounces", "m3", "cubicmeter", "cubicmeters",
    "mm3", "cubicmm", "cubicmillimeter", "cm3", "cubiccm", "cubiccentimeter",
    "pa", "pascal", "pascals", "kpa", "kilopascal", "kilopascals", "bar",
    "atm", "atmosphere", "atmospheres", "psi", "torr", "mmhg",
};

static std::vector<std::string> g_completion_words;



static std::string to_lower_copy_ascii(const std::string& value)
{
    std::string lower = value;
    for (char& c : lower) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return lower;
}



static int token_index_at_cursor(const char* line, int cursor_pos)
{
    if (line == nullptr || cursor_pos <= 0) {
        return 0;
    }

    int token_index = 0;
    bool in_token = false;
    for (int i = 0; i < cursor_pos && line[i] != '\0'; ++i) {
        if (std::isspace(static_cast<unsigned char>(line[i])) != 0) {
            if (in_token) {
                ++token_index;
                in_token = false;
            }
        } else {
            in_token = true;
        }
    }

    return token_index;
}



static void build_completion_words(int start)
{
    g_completion_words.clear();

    const int token_index = token_index_at_cursor(rl_line_buffer, start);
    if (token_index == 0) {
        for (const char* command : kReplCommands) {
            g_completion_words.emplace_back(command);
        }
        return;
    }

    for (const char* alias : kUnitAliases) {
        g_completion_words.emplace_back(alias);
    }
}



static char* repl_completion_generator(const char* text, int state)
{
    static std::size_t completion_index = 0;
    static std::string prefix;

    if (state == 0) {
        completion_index = 0;
        prefix = to_lower_copy_ascii((text != nullptr) ? std::string(text) : std::string());
    }

    while (completion_index < g_completion_words.size()) {
        const std::string& candidate = g_completion_words[completion_index++];
        if (candidate.compare(0, prefix.size(), prefix) == 0) {
            char* result = static_cast<char*>(std::malloc(candidate.size() + 1));
            if (result == nullptr) {
                return nullptr;
            }
            std::memcpy(result, candidate.c_str(), candidate.size() + 1);
            return result;
        }
    }

    return nullptr;
}



static char** repl_attempted_completion(const char* text, int start, int end)
{
    (void)end;

    build_completion_words(start);
    rl_attempted_completion_over = 1;
    return rl_completion_matches(text, repl_completion_generator);
}

} // namespace
#endif

static std::string history_file_path(){
    const char* home = std::getenv("HOME");
    if (home == nullptr || home[0] == '\0') {
        return "";
    }

    return std::string(home) + "/.unitfy_history";
} // ———  END OF function history_file_path()———————————————————————————————————



static std::string program_name_from_argv0(const char* argv0){
    if (argv0 == nullptr || argv0[0] == '\0') {
        return "unitfy";
    }

    std::string argv0_str(argv0);
    const std::size_t last_sep = argv0_str.find_last_of("/\\");
    if (last_sep == std::string::npos) {
        return argv0_str;
    }

    const std::string name = argv0_str.substr(last_sep + 1);
    return name.empty() ? std::string("unitfy") : name;
} // ———  END OF function program_name_from_argv0()—————————————————————————————



MDelta::MDelta()
{

} // ———  END OF function MDelta::MDelta()——————————————————————————————————————



MDelta::~MDelta()
{

} // ———  END OF function MDelta::~MDelta()—————————————————————————————————————



/// Process one REPL or one-shot conversion input string.
/// Handles help/exit commands, parses value/source/target unit tokens, and
/// dispatches to the matching quantity conversion routine.
CommandResult MDelta::process_input(const std::string& input)
{

    // Handle empty input and "help" command by displaying usage information.
    if (input.empty() || input == "help") {
        printf(cli_messages::kHelpTitleFmt, program_name_.c_str());
        printf("\n");
        printf("%s\n", cli_messages::kHelpUsage);
        printf("%s\n", cli_messages::kHelpExamples);
        printf("%s\n", cli_messages::kHelpTemperatureUnits);
        printf("%s\n", cli_messages::kHelpLengthUnits);
        printf("%s\n", cli_messages::kHelpVolumeUnits);
        printf("%s\n", cli_messages::kHelpPressureUnits);
        printf("%s\n", cli_messages::kHelpExitHint);

        return CommandResult::Continue;
    }

    // Handle exit commands to leave the REPL.
    if (input == "exit" || input == "quit"){
        printf(cli_messages::kGoodbyeFmt, program_name_.c_str());
        printf("\n");

        return CommandResult::Exit;
    }

    // The user is expected to input at least a value and a unit, like "100 C".
    // Optionally, they can also provide a target unit for direct
    // conversion, like "100 C F".
    //
    // The parsing logic below first tries to read a value and a unit. If that
    // fails, it reports an invalid input format.
    // Then it checks if there's an additional token. If there is exactly one
    // more token, it treats that as the target unit for conversion. If there
    // are multiple remaining tokens, it assumes the first one is part of the
    // unit (to support multi-word units like "cubic meter") and combines them
    // into the from_unit_str.
    //
    // Should the input unit be two worded, like "fl oz", the user can input
    // "2 fl oz" for all conversions. For targeted conversion, use a
    // single-token alias such as "2 floz mL".
    
    // Present the input as a stream to parse value and unit(s), and add
    // variables to hold the parsed components.
    std::istringstream iss(input);
    double value;
    std::string from_unit_str;
    std::string to_unit_str;
    
    // First; try to parse a value. If that fails, report invalid input format.
    if(!(iss >> value)){
        printf("%s\n", cli_messages::kInvalidInputFormat);
        
        return CommandResult::Continue;
    }

    // Next, try to parse the from unit. If that fails, report invalid input
    // format.
    if(!(iss >> from_unit_str)){
        printf("%s\n", cli_messages::kInvalidInputFormat);
        
        return CommandResult::Continue;
    }

    // Finally, check if there's an additional token. If there is, read the rest
    // of the line as a single string. If that string contains exactly one
    // token, treat it as the target unit for conversion. If there are multiple
    // remaining tokens, assume the first one is part of the unit (to support
    // multi-word units like "cubic meter") and combine them into the
    // from_unit_str.
    {
        std::string remainder;
        std::getline(iss >> std::ws, remainder);
        if(!remainder.empty()){
            if(remainder.find(' ') == std::string::npos){
                
                // Exactly one more token: targeted conversion mode
                to_unit_str = remainder;
            }
            else{
                
                // Multiple remaining tokens: treat all as part of a multi-word from_unit
                from_unit_str += " ";
                from_unit_str += remainder;
            }
        }
    }

    try{
        // If to_unit_str is empty, we're in all-conversion mode and we try to
        // match the from_unit_str against all known units. If to_unit_str is
        // not empty, we try to match the from_unit_str against the target unit.
        if(to_unit_str.empty()){
            
            // All-conversion mode (existing behaviour)
            if (try_convert_temperature(value, from_unit_str)) return CommandResult::Continue;
            if (try_convert_length(value, from_unit_str))      return CommandResult::Continue;
            if (try_convert_volume(value, from_unit_str))      return CommandResult::Continue;
            if (try_convert_pressure(value, from_unit_str))    return CommandResult::Continue;
            
            printf("Unknown unit: %s\n", from_unit_str.c_str());
        }

        // If to_unit_str is not empty, we're in targeted conversion mode. We
        // first try to match the from_unit_str and to_unit_str against known
        // units. If that fails, we fall back to treating "from to" as a
        // multi-word from_unit (all-conversion).
        else{
            // Targeted mode: try single-word from + single-word to
            const bool matched =
                try_convert_temperature(value, from_unit_str, to_unit_str) ||
                try_convert_length(value, from_unit_str, to_unit_str)      ||
                try_convert_volume(value, from_unit_str, to_unit_str)      ||
                try_convert_pressure(value, from_unit_str, to_unit_str);

            if (!matched) {
                // Fallback: treat "from to" as a multi-word from_unit (all-conversion)
                const std::string combined = from_unit_str + " " + to_unit_str;
                if (!try_convert_temperature(value, combined) &&
                    !try_convert_length(value, combined)      &&
                    !try_convert_volume(value, combined)      &&
                    !try_convert_pressure(value, combined)) {
                    
                        printf("Unknown unit: %s\n", combined.c_str());
                }
            }
        }
    } // ———  END OF try_convert_* checks———————————————————————————————————————

    // Catch and report any QuantityError exceptions thrown by the conversion
    // routines.
    catch (const QuantityError& e){
        printf("Error: %s\n", e.what());
    } // ———  END OF catch(QuantityError)———————————————————————————————————————

    // If all is good here, we return Continue to keep the REPL running. The
    // conversion functions will have already printed the results to the user.
    return CommandResult::Continue;
} // ———  END OF function MDelta::process_input()——————————————————————————————



/*
 * MDelta::repl() implements the Read-Eval-Print Loop (REPL) for interactive use:
 * - Displays a banner with the program name and version
 * - Provides usage hints
 * - Supports command history if readline is available
 * - Processes user input in a loop until exit commands are received
 */
void MDelta::repl(void)
{
    printf(cli_messages::kReplBannerFmt,
           program_name_.c_str(),
           UNITFY_VERSION);
           
    printf("\n");
    printf("%s\n", cli_messages::kReplHint);
        
    // The code below will be dimmed out if HAVE_READLINE is not defined.
    // That's fine since it will be evaluated at build time and won't cause
    // issues on platforms without readline.
    // Readline provides a better user experience with input editing and
    // history, but we want to support basic REPL functionality even without it.
    #if defined(HAVE_READLINE)
        rl_attempted_completion_function = repl_attempted_completion;

        const std::string history_file = history_file_path();
        if (!history_file.empty()) {
            (void)read_history(history_file.c_str());
        }

        // REPL loop with readline support
        while(true){
            char* raw_line = readline("> ");
            if (raw_line == nullptr) {
                printf("\n");
                break;
            }

            std::string line(raw_line);
            std::free(raw_line);

            if (!line.empty()) {
                add_history(line.c_str());
            }

            if (process_input(line) == CommandResult::Exit) {
                break;
            }
        } // ———  END OF REPL loop with readline————————————————————————————————

        if (!history_file.empty()) {
            (void)write_history(history_file.c_str());
        } // ———  END OF REPL loop with readline————————————————————————————————

    #else
        std::string line;
        // REPL loop without readline support
        while(true){
            printf("> ");
            if (!std::getline(std::cin, line)){
                break;
            }
            if (process_input(line) == CommandResult::Exit) {
                break;
            }
        } // ———  END OF REPL loop without readline—————————————————————————————
    #endif // ———  END OF conditional compilation for readline——————————————————
} // ———  END OF function MDelta::repl()————————————————————————————————————————



/*
 * MDelta::run() implements the main application logic:
 * - Parses command-line arguments to determine mode (REPL vs single input)
 * - In REPL mode, starts an interactive loop with readline support if available
 * - In single input mode, processes the input string directly
 * - Handles --version flag and provides usage hints for invalid input
 * - Catches and reports QuantityError exceptions from conversion routines
 * - Catches and reports any unexpected exceptions to prevent crashes  
*/
int MDelta::run(int argc, char* argv[])
{
    try{
        program_name_ = program_name_from_argv0((argc > 0) ? argv[0] : nullptr);

        // If no arguments, start REPL.        
        if (argc == 1){
            repl();
        }

        // If one argument, treat it as a single input string.
        else if (argc == 2){
            const std::string arg = argv[1];
            if (arg == "--version") {
                printf(cli_messages::kVersionFmt,
                       program_name_.c_str(),
                       UNITFY_VERSION);
                printf("\n");
                return 0;
            }

            std::string combined = argv[1];
            const std::size_t space_pos = combined.find(' ');

            if (space_pos == std::string::npos){
                printf(cli_messages::kCliUsageFmt, program_name_.c_str());
                printf("\n");
                printf(cli_messages::kCliUsageReplFmt, program_name_.c_str());
                printf("\n");
                printf(cli_messages::kCliUsageVersionFmt, program_name_.c_str());
                printf("\n");
                
                return 1;
            }

            process_input(combined);
        }

        // If multiple arguments, combine them into a single input string.
        else{
            std::string input;
            for (int i = 1; i < argc; ++i) {
                if (i > 1) {
                    input += " ";
                }
                input += argv[i];
            }
            process_input(input);
        }

        return 0;

    } // ———  END OF try block for run()————————————————————————————————————————

    catch (const QuantityError& e){
        fprintf(stderr, "Error: %s\n", e.what());

        return 1;
    } // ———  END OF catch(QuantityError) for run()—————————————————————————————

    catch (const std::exception& e){
        fprintf(stderr, "Unexpected error: %s\n", e.what());
        
        return 2;
    } // ———  END OF catch(std::exception) for run()————————————————————————————

} // ———  END OF function MDelta::run()—————————————————————————————————————————

}  // namespace unitfy

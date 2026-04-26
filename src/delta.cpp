#include <cstdio>
#include <string>
#include <cstdlib>
#include <sstream>
#include <iostream>

#if defined(HAVE_READLINE)
    #include <readline/history.h>
    #include <readline/readline.h>
#endif

#include "delta.h"



namespace delta {

static std::string history_file_path(){
    const char* home = std::getenv("HOME");
    if (home == nullptr || home[0] == '\0') {
        return "";
    }

    return std::string(home) + "/.delta_history";
} // ———  END OF function history_file_path()———————————————————————————————————



static std::string program_name_from_argv0(const char* argv0){
    if (argv0 == nullptr || argv0[0] == '\0') {
        return "delta";
    }

    std::string argv0_str(argv0);
    const std::size_t last_sep = argv0_str.find_last_of("/\\");
    if (last_sep == std::string::npos) {
        return argv0_str;
    }

    const std::string name = argv0_str.substr(last_sep + 1);
    return name.empty() ? std::string("delta") : name;
} // ———  END OF function program_name_from_argv0()—————————————————————————————



MDelta::MDelta()
{

} // ———  END OF function MDelta::MDelta()——————————————————————————————————————



MDelta::~MDelta()
{

} // ———  END OF function MDelta::~MDelta()—————————————————————————————————————



CommandResult MDelta::process_input(const std::string& input){
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

    if (input == "exit" || input == "quit"){
        printf(cli_messages::kGoodbyeFmt, program_name_.c_str());
        printf("\n");

        return CommandResult::Exit;
    }

    std::istringstream iss(input);
    double value;
    std::string unit_str;

    if (!(iss >> value)){
        printf("%s\n", cli_messages::kInvalidInputFormat);
        return CommandResult::Continue;
    }

    std::getline(iss >> std::ws, unit_str);
    if (unit_str.empty()){
        printf("%s\n", cli_messages::kInvalidInputFormat);
        return CommandResult::Continue;
    }

    try{
        if (try_convert_temperature(value, unit_str)){
            return CommandResult::Continue;
        }

        if (try_convert_length(value, unit_str)){
            return CommandResult::Continue;
        }

        if (try_convert_volume(value, unit_str)){
            return CommandResult::Continue;
        }
        
        if (try_convert_pressure(value, unit_str)){
            return CommandResult::Continue;
        }

        printf("Unknown unit: %s\n", unit_str.c_str());
    } // ———  END OF try_convert_* checks———————————————————————————————————————

    catch (const QuantityError& e){
        printf("Error: %s\n", e.what());
    } // ———  END OF catch(QuantityError)———————————————————————————————————————

    return CommandResult::Continue;
} // ———  END OF function MDelta::process_input()——————————————————————————————



void MDelta::repl(void)
{
    printf(cli_messages::kReplBannerFmt,
           program_name_.c_str(),
           DELTA_VERSION);
    printf("\n");
    printf("%s\n", cli_messages::kReplHint);
        
    // The code below will be dimmed out if HAVE_READLINE is not defined.
    // That's fine since it will be evaluated at build time and won't cause
    // issues on platforms without readline.
    #if defined(HAVE_READLINE)
        const std::string history_file = history_file_path();
        if (!history_file.empty()) {
            (void)read_history(history_file.c_str());
        }

        while (true){
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
        }

        if (!history_file.empty()) {
            (void)write_history(history_file.c_str());
        } // ———  END OF REPL loop with readline————————————————————————————————

    #else
        std::string line;
        while (true){
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
                       DELTA_VERSION);
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

}  // namespace delta

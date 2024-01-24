

#include <iostream>
#include <filesystem>
#include <string>

#include <fstream>
#include <chrono>
#include <math.h>

#include "../include/termcolor.h"
#include "../include/termcolor-helper.hpp"
#include "../include/argv.hpp"
#include "../include/process.hpp"

#include "../include/iniparser.hpp"

using namespace std;
using namespace std::literals::string_literals;

namespace tc = termcolor;

enum /*ReturnCodes*/ {
    RET_OK = 0, RET_NO_CONF = 100, RET_NO_BUILD_TASK = 101, RET_FAILED = 113,
};

/** startup ! */
int main(int argc, char** argv) {

    //////////////////////
    // here come tests
    //////////////////////

    // cout << uniqueName("./cpp/wow.cpp", ".o") << std::endl;
    // return RET_OK;

    // return 0;
    // std::u8string us { u8"UTF-8 кодированный текст. 今日はご機嫌いかがですか？" };
    // std::cout << reinterpret_cast<const char*>(us.c_str()) << std::endl;
 
    //////////////////////
    // program itself
    /////////////////////

    constexpr const char* default_conf_file { "./build.conf" };
    Argv args { argc, argv };

    const string conf_file { args.getValue("--build-conf").value_or(default_conf_file) };

    // check conf file existance
    if(not std::filesystem::exists(conf_file))  {
        cerr << "cannot find [" << conf_file << "] with build options; stopping.";
        return RET_NO_CONF;
    }

    // parse build.conf
    auto ini = parse_ini(conf_file, ':');

        
    // collect info needed for build process
    if(not ini.has_value()) {
        cerr << "cannot retrieve build options; stopping.";
        return RET_NO_BUILD_TASK;
    }

    vector<string> exts {};
    string source_path {};
    string compiler_command {}, compiler_opts {};
    string linker_command {}, linker_opts {};
    string object_path {}, output {};

    for(auto &options: ini.value()) {   // walk thru given build rules

        string snd_val {};
        bool has_val { false };

        if(options.second.has_value())  {
            has_val = true;
            snd_val = trim(options.second.value());
        }

        if(options.first == "source-ext") {
            if(has_val) exts.push_back(snd_val);  // add exetension to list
        } else 
        if(options.first == "source-path") {
            if(has_val) source_path = snd_val;  // add exetension to list
        } else
        if(options.first == "object-path") {
            if(has_val) object_path = snd_val;  // add exetension to list
        } else
        if(options.first == "compiler") {
            if(has_val) compiler_command = snd_val;
        } else 
        if(options.first == "linker") {
            if(has_val) linker_command = snd_val;
        } else
        if(options.first == "linker-options") {
            if(has_val) linker_opts = snd_val;
        } else
        if(options.first == "output") {
            if(has_val) output = snd_val;
        } else 
        if(options.first == "compiler-options") {
            if(has_val) compiler_opts = snd_val;
        } else {    // unknown option
            cerr << "unrecognized option [" << options.first << "]; ignoring." << std::endl;
        }
    }

    // AND NOW: GO-GO !
    if(process(BuildTask { compiler: compiler_command, compiler_options: compiler_opts, 
                        linker: linker_command, linker_options: linker_opts,
                        sources: source_path, extensions: exts, object_path: object_path, 
                        output: output, verbose: args.isPresent("--verbose"), 
                        stop_on_error: args.isPresent("--stop-on-error"),
                        test_mode: args.isPresent("--test-mode") }))   {

        cout << "build succeded." << std::endl;
        return RET_OK;
    } else {
        cerr << "build failed." << std::endl;
        return RET_FAILED;
    }

    // done.
    return RET_OK;

}   // main()

// eof

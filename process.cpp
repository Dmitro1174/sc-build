
#include <filesystem>
#include <fstream>
#include "../include/termcolor.h"
#include "../include/process.hpp"

const string object_file_extension { ".o" };
const string list_file_name { "listfile" };

/** does main build tasks */
bool process(const BuildTask& task)   {

    bool res = true;   // allover processing result

    // first test general options
    if(task.compiler.length() == 0)  {
        cerr << "no compiler specified; stopping." << std::endl;
        return false;
    }

    if(task.output.length() == 0)   {
        cerr << "no output target specified; stopping." << std::endl;
        return false;
    }

    // test output path and create if needed
    if(std::filesystem::exists(task.object_path)) {
        if(task.verbose) cout << "ok, object path exists, going further" << std::endl;
    } else {
        if(not task.test_mode) { // change FS only if test-mode is OFF
            if(not std::filesystem::create_directory(task.object_path)) {
                cerr << "object path [" << task.object_path << "] do not exist and cannot be created; stopping." << std::endl;
                return false;
            } else {
                if(task.verbose) cout << "object path [" << task.object_path << "] created." << std::endl;
            }
        }
    }

    cout << task;

    //
    // now collect files to compile
    vector<string> all_sources { enumFiles(task.sources, task.extensions) };
    
    // list of all file to compile created; then make (source, object) pairs
    vector<pair<string, string>> file_pairs {};
    std::for_each(begin(all_sources), end(all_sources), [&file_pairs, &task] (const string& item) mutable {

        filesystem::path obj_name { changeParentDir(item, task.sources, task.object_path) };

        file_pairs.push_back(make_pair(item, obj_name.replace_extension(object_file_extension).string()));
    });

    // show result if needed
    if(task.verbose) {

        cout << "files to process: " << std::endl << std::endl;

        cout << colorizePairs<string, string>(file_pairs) << std::endl;
        
    }   

    // test: whether is need to wok at all
    if(all_sources.size() == 0) {
        cerr << "no files to process; stopping." << std::endl;
        return true;
    }

    // start compilation process itself !!!
    bool needToLink = false;
    

    for(const pair<string, string>& current_build_item: file_pairs) {

        // #1 - yield source file name (from full path)
        string file_name { std::filesystem::path(current_build_item.first).filename() };

        // #2 - create directory if needed
        // first try to create subdir
        string object_file { current_build_item.second };
        string dir = (filesystem::path { object_file }).remove_filename();

        if(not filesystem::is_directory(dir))   {   // not a directory, create ?
            if(system((string("mkdir ") + dir).c_str()) != 0) {
                cerr << "cannot create output directory: " << dir << "; stopping." << std::endl;
                return false;
            } else
                if(task.verbose) cout << "object directory " << dir << " created." << std::endl;
        }

        // here we must check whether source file is newer than object file
        

        if(not needToCompile(current_build_item.first, current_build_item.second))  {

            if(task.verbose) cout << current_build_item.first << ": object newer than source; passing." << std::endl;
            

        } else {

            needToLink = true;  // if at least one file needs to be compiled we must link them all

            // construct and run compiler command
            stringstream os_command {};
            
            os_command << task.compiler;
            os_command << " " << task.compiler_options;
            os_command << " " << current_build_item.first;
            os_command << " -o " << object_file;

            if(task.verbose) cout << "compiling: " << file_name << " ";

            if(not task.test_mode) { // run commands only if -test-mode is OFF
                if(0 != system(os_command.str().c_str()))   {   // compiler error !

                    cerr << termcolor::bright_red << "error." << termcolor::reset << std::endl;
                    if(task.stop_on_error) break;

                } else {

                    // object_files.push_back(object_file);

                    if(task.verbose) cout << termcolor::bright_green << "done." << termcolor::reset << std::endl;
                }
            } else if(task.verbose) cout << termcolor::bright_magenta << " fake operation done." << termcolor::reset << std::endl;

        }   // else needToComile()...
    }   // for(...)

    // check whether build target exists
    if(not filesystem::exists(task.output)) needToLink = true;  // if there is no target file - link !

    // now do linking
    if(needToLink) {
        // create list file & subdirs if needed
        if(not task.test_mode) {  // run commands only if -test-mode is IFF
            
            // create object file list
            vector<string> object_files { enumFiles(task.object_path, { object_file_extension }) }; // .o files' list for linking

            // try to write listfile
            if(not vectorToFile<string>(object_files, list_file_name)) {
                cerr << "cannot create listfile; stopping." << std::endl;
                return false;
            }
        }

        if(task.verbose) cout << "list file created." << std::endl;

        // LINK !!!
        if(not task.test_mode) {  // run commands only if -test-mode is IFF
            stringstream link_command {};

            link_command << task.linker;
            link_command << " @listfile -o " << task.output; 

            if(0 != system(link_command.str().c_str())) {
                cerr << "link failed." << std::endl;
                if(task.stop_on_error)
                    return false;
                else 
                    res = false;
            }
        }
    }   // if(needToLink)...

    // success !!!
    return res;

}   // process()

/* outputs human readable contents of `BuildTask` */
ostream& operator<<(ostream& os, const BuildTask& task) {

    // #1 - general options
    if(task.verbose) os << "compiler: [" << task.compiler << "]" << std::endl;
    if(task.verbose) os << "compiler flags: [" << task.compiler_options << "]" << std::endl;
    if(task.verbose) os << "output: [" << task.compiler << "]" << std::endl;

    // #2 - additional options
    size_t n = 1;

    if(task.verbose) {
        os << "extensions: { ";
        std::for_each(begin(task.extensions), end(task.extensions), [&n] (const string& ss) { cout << "#" << n++ << ": [" <<  ss << "]; "; });
        os << "}" << std::endl;
    }

    if(task.verbose) {
        os << "source path: " << task.sources << std::endl;
    }

    // done.
    return os;
}

// e-o-f

////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Dimitry Ishenko
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com
//
// Distributed under the GNU GPL license. See the LICENSE.md file for details.

////////////////////////////////////////////////////////////////////////////////
#include "gui/window.hpp"
#include "pgm/args.hpp"
#include "src/util.hpp"

#include <QApplication>

#include <exception>
#include <fstream>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

#if !defined(VERSION)
#  define VERSION "0"
#endif

////////////////////////////////////////////////////////////////////////////////
std::string version(const std::string& name)
{
}

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    int exit_code = 0;
    try
    {
        auto name{ fs::path(argv[0]).filename() };
        pgm::args args
        {
            { "-h", "--help",    "Print this help screen and exit." },
            { "-v", "--version", "Show version number and exit."    },
            { "actions?",        "Path to alternate actions file."  },
        };
        args.parse(argc, argv);

        if(args["--help"])
        {
            std::cout << args.usage(name) << std::endl;
        }
        else if(args["--version"])
        {
            std::cout << name.string() << " version " << VERSION << std::endl;
        }
        else
        {
            fs::path path{ args["actions"].value_or("") };
            if(path.empty())
            {
                path = src::data_path() / name / "actions.conf";

                fs::create_directory(path.parent_path());
                if(!fs::exists(path)) std::fstream{ path, std::ios::out };
            }

            QApplication app{ argc, argv };

            gui::window win;
            win.show();

            src::on_interrupt([&](int signal)
            {
                std::cout << "Received signal " << signal << " - exiting." << std::endl;
                app.exit(1);
            });

            std::cout << "Starting event loop." << std::endl;
            exit_code = app.exec();
        }
    }
    catch(std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        exit_code = 1;
    }
    catch(...)
    {
        std::cerr << "???" << std::endl;
        exit_code = 1;
    }
    return exit_code;
}

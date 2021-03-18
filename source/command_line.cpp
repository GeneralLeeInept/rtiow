#include "command_line.h"
#include "cxxopts.hpp"

std::string print(uint32_t i)
{
    int len = std::snprintf(nullptr, 0, "%u", i) + 1;
    std::string s;
    s.resize(len);
    std::snprintf(&s[0], len, "%u", i);
    return s;
}

bool parseCommandLine(int argc, char** argv, CommandLineArguments& arguments)
{
    try
    {
        cxxopts::Options options("rtiow", "General Lee Inept's Raytracer");
        options.add_options()
            ("w,width", "Image width", cxxopts::value<uint32_t>()->default_value(print(arguments.imageWidth).c_str()))
            ("h,height", "Image height", cxxopts::value<uint32_t>()->default_value(print(arguments.imageHeight).c_str()))
            ("s,samples", "Samples per pixel", cxxopts::value<uint32_t>()->default_value(print(arguments.samplesPerPixel).c_str()))
            ("d,maxdepth", "Maximum ray bounces", cxxopts::value<uint32_t>()->default_value(print(arguments.maxDepth).c_str()))
            ("j,numjobs", "Number of parallel jobs", cxxopts::value<uint32_t>()->default_value(print(arguments.numJobs).c_str()))
            ("o,output", "Output filename (without extension)", cxxopts::value<std::string>()->default_value(arguments.outputName))
            ("help", "Print usage")
        ;

        auto commandLine = options.parse(argc, argv);

        if (commandLine.count("help"))
        {
            std::cout << options.help() << "\n";
            exit(EXIT_SUCCESS);
        }

        arguments.imageWidth = commandLine["width"].as<uint32_t>();
        arguments.imageHeight = commandLine["height"].as<uint32_t>();
        arguments.samplesPerPixel = commandLine["samples"].as<uint32_t>();
        arguments.maxDepth = commandLine["maxdepth"].as<uint32_t>();
        arguments.numJobs = commandLine["numjobs"].as<uint32_t>();
        arguments.outputName = commandLine["output"].as<std::string>();

        return true;
    }
    catch (cxxopts::OptionException& e)
    {
        std::cerr << e.what() << "\n";
        return false;
    }
}

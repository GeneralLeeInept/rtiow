#pragma once

#include <cstdint>
#include <string>

struct CommandLineArguments
{
    uint32_t imageWidth;
    uint32_t imageHeight;
    uint32_t samplesPerPixel;
    uint32_t maxDepth;
    uint32_t numJobs;
    std::string outputName;
    std::string hdriSkyPath;
};

bool parseCommandLine(int argc, char** argv, CommandLineArguments& arguments);

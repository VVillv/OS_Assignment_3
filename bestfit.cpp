#include "memory_manager.h"
#include <iostream>
#include <sstream>
#include <cstdio> // For popen() and pclose()
#include <memory> // For std::unique_ptr

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " path_to_a3_gen_script" << std::endl;
        return 1;
    }

    // Construct the command to execute the a3_gen.sh script
    std::string command = "./" + std::string(argv[1]);

    // Use popen to execute the command and create a pipe to read its output
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
    if (!pipe)
    {
        std::cerr << "popen() failed!" << std::endl;
        return 1;
    }

    // Read the script's output and process the commands
    MemoryManager manager;
    manager.set_strategy(true); // Use best-fit strategy

    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe.get()) != nullptr)
    {
        std::string line(buffer);
        std::istringstream iss(line);
        std::string action;
        iss >> action;

        if (action == "alloc")
        {
            std::size_t size;
            iss >> size;
            manager.alloc(size); // Perform allocation
        }
        else if (action == "dealloc")
        {
            manager.dealloc(nullptr); // Perform deallocation (you need to determine what to deallocate)
        }
    }

    manager.print_statistics(); // Print the memory usage statistics

    return 0;
}

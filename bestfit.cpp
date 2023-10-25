#include "memory_manager.h"
#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " datafile" << std::endl;
        return 1;
    }

    MemoryManager manager(MemoryManager::Strategy::BestFit); // Initialize with Best Fit strategy

    std::string filename = argv[1];
    if (!manager.read_commands(filename)) {
        std::cerr << "Failed to process file: " << filename << std::endl;
        return 1;
    }

    manager.print_memory_state(); // Print the final state of the memory

    return 0;
}
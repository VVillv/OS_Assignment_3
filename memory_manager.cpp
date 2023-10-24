#include "memory_manager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>  // for sbrk()
#include <algorithm> // for std::max

MemoryManager::MemoryManager() : use_best_fit(false)
{
    // Initialization if needed
}

MemoryManager::~MemoryManager()
{
    // Cleanup if needed
}

void *MemoryManager::alloc(std::size_t size)
{
    Allocation *allocation = find_fit(size);

    if (!allocation)
    {
        void *space = sbrk(size); // Allocate new memory space
        if (space == (void *)-1)
        {
            return nullptr; // sbrk failed to allocate memory
        }

        Allocation newAllocation;
        newAllocation.size = size;
        newAllocation.space = space;
        newAllocation.is_free = false;

        memory.push_back(newAllocation);
        return newAllocation.space;
    }
    else
    {
        allocation->is_free = false;
        return allocation->space;
    }
}

void MemoryManager::dealloc(void *space)
{
    auto it = std::find_if(memory.begin(), memory.end(), [space](const Allocation &allocation)
                           { return allocation.space == space; });

    if (it != memory.end())
    {
        it->is_free = true;
    }
}

void MemoryManager::read_commands(const char *filepath)
{
    std::ifstream file(filepath);
    std::string line;
    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string command;
        ss >> command;
        if (command == "alloc")
        {
            std::size_t size;
            ss >> size;
            void *space = alloc(size);
            std::cout << "Allocated " << size << " bytes at " << space << std::endl;
        }
        else if (command == "dealloc")
        {
            void *space;
            ss >> space;
            dealloc(space);
            std::cout << "Deallocated space at " << space << std::endl;
        }
    }
}

void MemoryManager::print_statistics()
{
    std::size_t total_size = 0;
    std::size_t free_chunks_bytes = 0;
    std::size_t occupied_chunks_bytes = 0;
    std::size_t total_allocated_bytes = 0;

    for (const auto &alloc : memory)
    {
        total_size += alloc.size;
        if (alloc.is_free)
        {
            free_chunks_bytes += alloc.size;
        }
        else
        {
            occupied_chunks_bytes += alloc.size;
            total_allocated_bytes += alloc.size; // This assumes no internal fragmentation
        }
    }

    std::size_t occupied_unused_bytes = occupied_chunks_bytes - total_allocated_bytes;
    std::size_t total_unused_bytes = free_chunks_bytes + occupied_unused_bytes;

    std::cout << "Memory Usage Statistics:" << std::endl;
    std::cout << "Total size: " << total_size << " bytes" << std::endl;
    std::cout << "Free chunks: " << free_chunks_bytes << " bytes" << std::endl;
    std::cout << "Occupied chunks: " << occupied_chunks_bytes << " bytes" << std::endl;
    std::cout << "Total allocated: " << total_allocated_bytes << " bytes" << std::endl;
    std::cout << "Occupied but unused: " << occupied_unused_bytes << " bytes" << std::endl;
    std::cout << "Total unused: " << total_unused_bytes << " bytes" << std::endl;
}

Allocation *MemoryManager::find_fit(std::size_t size)
{
    // For simplicity, this example uses a first-fit strategy regardless of the use_best_fit flag.
    for (auto &alloc : memory)
    {
        if (alloc.is_free && alloc.size >= size)
        {
            return &alloc;
        }
    }
    return nullptr;
}

void MemoryManager::merge_free_chunks()
{
    // This function would go through your memory list and merge adjacent free chunks.
    // This is a simplification. In a real scenario, you would need to ensure the chunks are actually adjacent in memory, not just in your list.
}

void MemoryManager::set_strategy(bool use_best_fit)
{
    this->use_best_fit = use_best_fit;
}

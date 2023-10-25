#include "memory_manager.h"
#include <iostream>
#include <unistd.h>  // For the 'sbrk' function
#include <algorithm> // For std::find_if
#include <cassert>   // For assert
#include <fstream>
#include <sstream>

MemoryManager::MemoryManager(Strategy strategy) : selectedStrategy(strategy)
{
    // Initialize the free list with a single chunk of memory
    Allocation initial_allocation;
    initial_allocation.size = 0;
    initial_allocation.space = sbrk(0);
    free_list.push_back(initial_allocation);
}

MemoryManager::~MemoryManager()
{
    // Free all allocated memory chunks
    for (auto &allocation : allocated_list)
    {
        sbrk(-allocation.size);
    }
}

void *MemoryManager::alloc(std::size_t chunk_size)
{
    // Ensure the requested size is within the allowed bounds.
    assert(chunk_size > 0 && "Size must be positive");
    assert(chunk_size <= 512 && "Maximum allocation size exceeded"); // assuming 512 is the max size

    // Adjust the chunk size to match one of the fixed partition sizes.
    if (chunk_size <= 32)
        chunk_size = 32;
    else if (chunk_size <= 64)
        chunk_size = 64;
    else if (chunk_size <= 128)
        chunk_size = 128;
    else if (chunk_size <= 256)
        chunk_size = 256;
    else
        chunk_size = 512; // Adjust based on your fixed sizes

    // The search function will depend on the current strategy.
    auto search_func = [this, chunk_size](const Allocation &chunk)
    {
        if (best_fit_strategy)
        {
            // For best-fit, we're looking for the smallest chunk that is big enough.
            return chunk.size >= chunk_size; // You might want to implement a more sophisticated search
        }
        else
        {
            // For first-fit, we're looking for the first chunk that is big enough.
            return chunk.size >= chunk_size;
        }
    };

    // Search the free list for a suitable chunk.
    auto it = std::find_if(free_list.begin(), free_list.end(), search_func);

    if (it != free_list.end())
    {
        // Suitable chunk found in the free list. Remove it from the free list.
        Allocation allocation = *it;
        free_list.erase(it);

        // If the chunk is larger than needed, you might want to split it here and
        // put the unused portion back into the free list (depending on assignment requirements).

        // Add the allocation to the allocated list.
        allocated_list.push_back(allocation);

        // Return the allocated space.
        return allocation.space;
    }
    else
    {
        // No suitable chunk was found. Request more memory from the system.
        void *new_space = sbrk(chunk_size); // Request memory from the OS.
        if (new_space == (void *)-1)
        {
            // Handle memory allocation failures.
            return nullptr; // sbrk failed to allocate memory.
        }

        // Create a new allocation.
        Allocation new_allocation;
        new_allocation.size = chunk_size;
        new_allocation.space = new_space;

        // Add the new allocation to the allocated list.
        allocated_list.push_back(new_allocation);

        // Return the newly allocated space.
        return new_allocation.space;
    }
}

void MemoryManager::dealloc(void *chunk)
{
    // Validate the input pointer.
    if (chunk == nullptr)
    {
        std::cerr << "Attempt to deallocate null pointer" << std::endl;
        return; // or handle error as appropriate
    }

    // Find the allocation associated with the chunk.
    auto it = std::find_if(allocated_list.begin(), allocated_list.end(), [chunk](const Allocation &allocation)
                           { return allocation.space == chunk; });

    if (it != allocated_list.end())
    {
        // The chunk was found in the allocated list.

        // Create a copy of the allocation record (if needed for later).
        Allocation allocation = *it;

        // Remove the chunk from the allocated list.
        allocated_list.erase(it);

        // Here, you could merge adjacent free chunks to avoid fragmentation.
        // This step is optional and depends on your assignment's requirements.

        // Add the freed chunk to the free list.
        free_list.push_back(allocation);

        // If your system requires, you might need to sort the free list or merge adjacent chunks.
    }
    else
    {
        // The chunk was not found. This is an error.
        std::cerr << "Attempt to deallocate memory that was not allocated" << std::endl;
        // Handle the error as appropriate for your system.
    }
}

void MemoryManager::set_strategy(bool use_best_fit)
{
    best_fit_strategy = use_best_fit;
}

void MemoryManager::print_memory_state()
{
    std::cout << "Memory State" << std::endl;

    // Print the allocated list
    std::cout << "\nAllocated List:" << std::endl;
    if (allocated_list.empty())
    {
        std::cout << "Empty" << std::endl;
    }
    else
    {
        for (const auto &allocation : allocated_list)
        {
            std::cout << "Address: " << allocation.space
                      << ", Size: " << allocation.size
                      << ", Used: " << allocation.used_size << std::endl; // If you track 'used_size'
        }
    }

    // Print the free list
    std::cout << "\nFree List:" << std::endl;
    if (free_list.empty())
    {
        std::cout << "Empty" << std::endl;
    }
    else
    {
        for (const auto &free_chunk : free_list)
        {
            std::cout << "Address: " << free_chunk.space
                      << ", Size: " << free_chunk.size << std::endl;
        }
    }
}

// Method definition for reading commands
bool MemoryManager::read_commands(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return false; // Return false if the file couldn't be opened
    }

    std::string line;
    std::vector<void *> allocated_chunks; // To keep track of allocated memory chunks

    while (std::getline(file, line)) // Read the file line by line
    {
        std::stringstream ss(line); // Use a stringstream to process the line
        std::string command;
        ss >> command; // Extract the command from the line

        if (command == "alloc:") // If the command is an 'alloc' command
        {
            int size;
            ss >> size; // Read the size from the command

            // Now, you would call the function that handles memory allocation
            void *allocated_memory = this->alloc(size); // This is hypothetical; your actual call might be different

            if (allocated_memory != nullptr)
            {                                                 // Check if allocation was successful
                allocated_chunks.push_back(allocated_memory); // Store the pointer for later use
            }
            else
            {
                std::cerr << "Allocation failed for size: " << size << std::endl;
            }
        }
        else if (command == "dealloc") // If the command is a 'dealloc' command
        {
            // Here, you would call the function that handles memory deallocation.
            // You need to determine which memory chunk to deallocate.
            // This requires you to have kept track of your allocations.
            if (!allocated_chunks.empty())
            {
                void *chunk_to_free = allocated_chunks.back(); // Get the last allocated chunk
                allocated_chunks.pop_back();                   // Remove the last element from the tracking vector

                this->dealloc(chunk_to_free); // Deallocate the chunk. This is hypothetical; your actual call might be different and might require parameters
            }
            else
            {
                std::cerr << "No memory chunk available for deallocation." << std::endl;
            }
        }
        else
        {
            std::cerr << "Unknown command in file: " << command << std::endl;
        }
    }

    file.close(); // Don't forget to close the file
    return true;  // Return true if everything was successful
}
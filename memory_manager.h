#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <cstddef>
#include <list>
#include <string>

struct Allocation
{
    std::size_t size;
    void *space;
    std::size_t used_size;
};

class MemoryManager
{
public:
    enum class Strategy
    {
        FirstFit,
        BestFit
    };

    MemoryManager(Strategy strategy);
    ~MemoryManager();

    void *alloc(std::size_t chunk_size);
    void dealloc(void *chunk);
    void set_strategy(bool use_best_fit); // true for best-fit, false for first-fit
    void print_memory_state();

    bool read_commands(const std::string &filename);

private:
    std::list<Allocation> free_list;      // List of free memory chunks.
    std::list<Allocation> allocated_list; // List of allocated memory chunks.
    bool best_fit_strategy;               // true for best-fit, false for first-fit
    Strategy selectedStrategy;
};

#endif // MEMORY_MANAGER_H

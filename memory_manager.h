#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <cstddef>
#include <list>

struct Allocation
{
    std::size_t size;
    void *space;
    bool is_free;
};

class MemoryManager
{
public:
    MemoryManager();
    ~MemoryManager();
    void *alloc(std::size_t size);
    void dealloc(void *space);
    void read_commands(const char *filepath);
    void print_statistics();
    void set_strategy(bool use_best_fit); // true for best-fit, false for first-fit

private:
    std::list<Allocation> memory;
    bool use_best_fit;

    Allocation *find_fit(std::size_t size);
    void merge_free_chunks();
};

#endif // MEMORY_MANAGER_H

#ifndef ALLOCATION_H
#define ALLOCATION_H

struct allocation
{
    std::size_t size;
    void *space;
};

#endif // ALLOCATION_H
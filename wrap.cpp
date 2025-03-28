#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <mutex>

extern "C" {
void __nnDetailNintendoSdkRuntimeObjectFileRefer()
{
    // balls
}
void __nnDetailNintendoSdkRuntimeObjectFile()
{
    // balls
}

void* glslc_Alloc(size_t size)
{
    return malloc(size);
}
void glslc_Free(void* ptr)
{
    free(ptr);
}
void* glslc_Realloc(void* ptr, size_t size)
{
    return realloc(ptr, size);
}

void __nnmusl_init_dso()
{
    // ehhh
}

void __nnmusl_fini_dso()
{
    // ehhh
}

#define STUB_WARN(SYM)                     \
    void SYM()                             \
    {                                      \
        fprintf(stderr, #SYM " called\n"); \
        abort();                           \
    }

STUB_WARN(NvOsFclose);
STUB_WARN(NvOsFwrite);
STUB_WARN(NvOsFopen);
STUB_WARN(NvOsFread);

void NvOsMutexCreate(std::mutex** shit)
{
    *shit = new std::mutex;
}

void NvOsMutexLock(std::mutex** shit)
{
    (*shit)->lock();
}

void NvOsMutexUnlock(std::mutex** shit)
{
    (*shit)->unlock();
}

void NvOsMutexDestroy(std::mutex** shit)
{
    delete *shit;
}

struct NvAllocator {
    void* alloc;
    void* allocAlign;
    void* realloc;
    void* free;
};

NvAllocator sAllocator;

NvAllocator* glslc_GetAllocator()
{
    return &sAllocator;
}
}
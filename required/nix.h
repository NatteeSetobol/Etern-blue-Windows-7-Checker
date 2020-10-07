#ifndef __NIX_H__
#define __NIX_H__

#include <string.h>
#include <stdlib.h>

void* PlatformAllocate(int size);
void PlatformFree(void* chunk);
void ClearMemory(void* ptr, int size);
#endif

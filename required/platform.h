#ifndef __PLATFORM_H__
#define __PLATFORM_H__ 

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))
void* PlatformAllocate(int size);

void ClearMemory(void* ptr, int size);
#endif

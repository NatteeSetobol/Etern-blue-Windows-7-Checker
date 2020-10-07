#include "nix.h"
void* PlatformAllocate(int size)
{
	void* result = malloc(size);

	return result;
}

void PlatformFree(void* chunk)
{
	free(chunk);
	chunk = NULL;
}

void ClearMemory(void* ptr, int size)
{
	memset(ptr, 0, size);
}
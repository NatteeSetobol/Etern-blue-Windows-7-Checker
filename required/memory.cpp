#include "memory.h"
extern struct memory_struct* memoryHead = NULL;


//struct memory_info* memoryInfoHead = NULL;

/*
void* MAlloc(size_t size, int line, const char* file)
{
	struct memory_info* memoryInfo = (struct memory_info*)PlatformAllocate(size + sizeof(*memoryInfo));
	memoryInfo->file = file;
	memoryInfo->line = line;
	memoryInfo->size = size;
	memoryInfo->next = memoryInfoHead;
	if (memoryInfoHead)
		memoryInfo->next->prev = memoryInfo;
	memoryInfo->prev = NULL;
	memoryInfoHead = memoryInfo;

	return memoryInfo + 1;
}

void MFree(void* ptr)
{
	if (ptr)
	{
		struct memory_info* memoryInfo = (struct memory_info*) ptr - 1;
		memoryInfo->size = ~memoryInfo->size;

		if (memoryInfo->prev == NULL)
		{
			memoryInfoHead = memoryInfo->next;
		}
		else {
			memoryInfo->prev->next = memoryInfo->next;

			if (memoryInfo->next)
				memoryInfo->next->prev = memoryInfo->prev;
		}
	}
}
*/

void* MemoryRaw_(int size, int line, const char* filename)
{
#ifdef MEM_DEBUG
	int newSize = (size)+sizeof(struct memory_struct);
	void* rawMemory = PlatformAllocate(newSize);
	struct memory_struct* result = (struct memory_struct*) rawMemory;

	ClearMemory(rawMemory, newSize);

	result->line = line;
	result->size = size;
	result->name = filename;
	//strcpy(result->name,filename);

	if (memoryHead == NULL)
	{
		memoryHead = result;
		memoryHead->next = NULL;
		memoryHead->prev = NULL;
	}
	else {
		memoryHead->prev = result;
		result->next = memoryHead;
		memoryHead = result;
	}

	return  (void*)((i8*)rawMemory + sizeof(struct memory_struct));
#else

	void* rawMemory = PlatformAllocate(size);
	ClearMemory(rawMemory, size);
	return rawMemory;
#endif
}

void* Memory_(int size, int line, const char* filename)
{
#ifdef MEM_DEBUG
	int newSize = size + sizeof(struct memory_struct);
	void* rawMemory = PlatformAllocate(newSize);
	struct memory_struct* result = (struct memory_struct*) rawMemory;

	ClearMemory(rawMemory, newSize);

	result->line = line;
	result->size = size;
	result->name = filename;
	//strcpy(result->name,filename);

	if (memoryHead == NULL)
	{
		memoryHead = result;
		memoryHead->next = NULL;
		memoryHead->prev = NULL;
	}
	else {
		memoryHead->prev = result;
		result->next = memoryHead;
		memoryHead = result;
	}

	return (((i8*)rawMemory) + sizeof(struct memory_struct));
#else
	void* rawMemory = PlatformAllocate(size + 50);

	ClearMemory(rawMemory, size+50);
	return rawMemory;

#endif
}

void FreeMemory(i8* chunk)
{

#ifdef MEM_DEBUG
	if (chunk)
	{
		struct memory_struct* mi = (struct memory_struct*) ((chunk)-sizeof(struct memory_struct));

		if (mi)
		{
			if (mi->prev == NULL)
			{
				memoryHead = mi->next;
			}
			else
				mi->prev->next = mi->next;
			if (mi->next)
				mi->next->prev = mi->prev;
			if (mi == memoryHead)
			{
				memoryHead = memoryHead->next;
				if (memoryHead)
				{
					memoryHead->prev = NULL;
				}
				else {
					memoryHead = NULL;
				}
			}
			else {
				if (mi->prev)
				{
					mi->prev->next = mi->next;
					if (mi->next)
					{
						if (mi->next->prev)
						{
							mi->next->prev = mi->prev;
						}
					}
					else {

					}
				}
				else {

				}
			}

		}
		PlatformFree(mi);
	}

#else
	PlatformFree(chunk);
#endif

}

void MemoryResults()
{
	int leakCount = 0;
	struct memory_struct* m = NULL;
	m = memoryHead;

	while (m)
	{
		struct memory_struct* chunk = m;
		if (m)
		{
			printf("leaked in %s, line %i\n", m->name, m->line);
			leakCount++;
			m = m->next;
		}
		else {
		}
		if (chunk)
		{
			PlatformFree(chunk);
			chunk = NULL;
		}
	}

	printf("total leaked: %i\n", leakCount);
}

void GetMemoryResult(struct marray *result)
{
	int leakCount = 0;
	struct memory_struct* m = NULL;

	m = memoryHead;

	while (m)
	{
		struct memory_struct* chunk = m;

		if (m)
		{
			struct marray* info = (struct marray*) MemoryRaw(sizeof(struct marray));
			memset(info, 0, sizeof(struct marray));

			if (m->name == NULL) return ;
			

			char* name = (char*)MemoryRaw(strlen(m->name)+1);
			strcpy(name, m->name);
			AddToMArray(info, name);

			//NOTES(): This is a bad way of doing this!!!
			char* string = (char*)MemoryRaw(255);
			sprintf(string, "Line: %i  ------- Size: %i byte(s)", m->line,m->size);

			AddToMArray(info, string);
			
			AddToMArray(result, info);

			leakCount++;
			m = m->next;
		}


	}
}

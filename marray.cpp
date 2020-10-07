#include "marray.h"

struct marray_list marrayFreeList = {};


void AddToMFreeList(int index)
{
	if (marrayFreeList.head == NULL)
	{
		marrayFreeList.head = Memory(struct marray_link);
		marrayFreeList.head->id = index;
		marrayFreeList.head->next = NULL;
		marrayFreeList.tail = marrayFreeList.head;
	}
	else {
		marrayFreeList.tail->next = Memory(struct marray_link);
		marrayFreeList.tail->next->id = index;
		marrayFreeList.tail->next->next = NULL;
		marrayFreeList.tail = marrayFreeList.tail->next;
	}
}

void CreateMArray_(struct marray* mArray, char* filename, int line)
{

	mArray->realCount = 0;
	mArray->count = 0;
	mArray->size = 2;
	mArray->ptr = MemoryAL(i32*, mArray->size, line, filename);
}

void AddToMArray_(struct marray* mArray, i32* addr, char* filename, int line)
{
	if (mArray->ptr == NULL)
	{
		CreateMArray(mArray);
	}
	else {
		if (marrayFreeList.head)
		{

			int newIndex = marrayFreeList.head->id;
			struct marray_link* temp = marrayFreeList.head;
			marrayFreeList.head = marrayFreeList.head->next;

			if (temp)
			{
				FreeMemory((i8*)temp);
				temp = NULL;
			}

			mArray->ptr[newIndex] = (i32*)addr;
			return;
		}
	}

	if (mArray->realCount < mArray->size)
	{
		mArray->ptr[mArray->realCount] = (i32*)addr;
		//newArray= addr;
		mArray->count++;
		mArray->realCount++;
	}
	else {
		i32 oldSize = 0;
		i32** tempChunk = mArray->ptr;
		oldSize = mArray->size;
		mArray->size *= 2;
		mArray->ptr = MemoryA(i32*, mArray->size);

		for (int i = 0; i < oldSize; i++)
		{
			mArray->ptr[i] = tempChunk[i];
		}

		mArray->ptr[mArray->realCount] = addr;

		mArray->count++;
		mArray->realCount++;
		if (tempChunk)
		{
			FreeMemory((i8*)tempChunk);
			tempChunk = NULL;
		}
	}
}

i32* GetFromMArray(struct marray* Array, i32 index)
{
	return (i32*)Array->ptr[index];
}

void RemoveFromArray(struct marray* Array, i32 index)
{
	void* ptr = Array->ptr[index];
	if (ptr)
	{
		FreeMemory((i8*)ptr);
		ptr = NULL;
	}
	//	AddToMFreeList(index);
	Array->count--;
}

void FreeMArray(struct marray* Array)
{
	struct marray_link* arrayListHead = NULL;

	if (Array->realCount == 0) return;

	for (int i = 0; i < Array->realCount; i++)
	{
		RemoveFromArray(Array, i);
	}

	if (Array->ptr)
	{
		FreeMemory((i8*)Array->ptr);
		Array->ptr = NULL;
	}


	arrayListHead = marrayFreeList.head;
	while (arrayListHead)
	{
		struct marray_link* temp = arrayListHead;
		arrayListHead = arrayListHead->next;

		if (temp)
		{
			FreeMemory((i8*)temp);
			temp = NULL;
		}
	}
}
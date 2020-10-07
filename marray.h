#ifndef __MARRAY_H__
#define __MARRAY_H__


#include "./required/memory.h"
#include "./required/intrinsic.h"

struct marray
{
	i32 realCount;
	i32 count;
	i32 size;
	i32** ptr;
};

struct marray_link
{
	i32 id;
	struct marray_link *next;
};

struct marray_list
{
	struct marray_link *head;
	struct marray_link *tail;
};



void AddToMArray_(struct marray* mArray, i32* addr, char* filename, int line);
i32* GetFromMArray(struct marray* Array, i32 index);
void RemoveFromArray(struct marray* Array, i32 index);
void FreeMArray(struct marray* Array);
void CreateMArray_(struct marray* mArray, char* filename, int line);
void AddToMFreeList(int index);

#define AddToMArray(ArrayStruct, Data) AddToMArray_(ArrayStruct, (i32*) Data,(char*) __FILE__, __LINE__)

#define for_marray(MArrayStruct,var) struct marray *MArrayStruct##ary =  MArrayStruct; for(int var=0;var <MArrayStruct##ary->count;var++)
#define CreateMArray(array) CreateMArray_(array, (char*) __FILE__,__LINE__)

#endif

#ifndef __MEMORY_H__
#define __MEMORY_H__

#define _CRT_SECURE_NO_WARNINGS 1

#include <stdio.h>
#include "intrinsic.h";
#include "./nix.h";
#include "platform.h";
#include "../marray.h";

struct memory_struct
{
	int size;
	int line;
	const char *name;
	struct memory_struct *prev;
	struct memory_struct *next;
};

struct memory_info
{
	const char* file;
	int line;
	size_t size;
	struct memory_info *next;
	struct memory_info *prev;
};


//void* MAlloc(size_t size, int line, const char* file);
//void MFree(void* ptr);
void* MemoryRaw_(int size, int line, const char* filename);
void* Memory_(int size, int line, const char* filename);
void FreeMemory(i8* chunk);
void MemoryResults();
void GetMemoryResult(struct marray* result);

#define Memory(type) (type*) Memory_(sizeof(type), __LINE__,__FILE__)
#define MemoryA(type,size) (type*) Memory_( (size * sizeof(type)) ,__LINE__,__FILE__)
#define MemoryAL(type,size,line,file) (type*) Memory_( (size * sizeof(type)) ,line,file)
#define MemoryRaw(size) MemoryRaw_(size,__LINE__,__FILE__);
#define Free(chunk) FreeMemory((i8*) chunk)




#endif

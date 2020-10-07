#ifndef __nix_threaded_queue_h
#define __nix_threaded_queue_h
#include "required/intrinsic.h"
#include "required/nix.h"
#include "required/memory.h"
#include "required/platform.h"
#include "required/assert.h"
#include <semaphore.h>
#include <pthread.h> // threading

struct work_queue;
#define WORK_QUEUE_CALLBACK(name) void name(work_queue *queue, void *data)
typedef WORK_QUEUE_CALLBACK(work_queue_callback);

struct work_queue_entry
{
	work_queue_callback *callback;
	void *data;
};


struct work_queue
{
	ui32 volatile completionGoal;
	ui32 volatile completionCount;

	ui32 volatile nextEntryToWrite;
	ui32 volatile nextEntryToRead;
	sem_t semaphoreHandle;

	work_queue_entry entries[1028];
};

struct linux_thread_startup
{
	work_queue *queue;
};

struct thread
{
	pthread_t id;
};

#endif

#include "nix_threading.h"

void AddEntry(work_queue *queue, work_queue_callback *callback, void *data)
{
	ui32 newNextEntryToWrite = (queue->nextEntryToWrite + 1) % ArrayCount(queue->entries);
	work_queue_entry *entry = queue->entries + queue->nextEntryToWrite;
	entry->callback = callback;
	entry->data = data;
	++queue->completionGoal;

	asm volatile("" ::: "memory");

	queue->nextEntryToWrite = newNextEntryToWrite;
	sem_post(&queue->semaphoreHandle);
}

b32 DoNextWorkQueueEntry(work_queue *queue)
{
	bool32 weShouldSleep = false;

	ui32 originalNextEntryToRead = queue->nextEntryToRead;
	ui32 newNextEntryToRead = (originalNextEntryToRead + 1) % ArrayCount(queue->entries);

	if (originalNextEntryToRead != queue->nextEntryToWrite)
	{
		ui32 index = __sync_val_compare_and_swap(&queue->nextEntryToRead,originalNextEntryToRead,newNextEntryToRead);

		if (index == originalNextEntryToRead)
		{
			work_queue_entry entry = queue->entries[index];
			entry.callback(queue,entry.data);
			__sync_fetch_and_add(&queue->completionCount,1);
		}
	} else {
		weShouldSleep = true;
	}

	return weShouldSleep;
}

void CompleteAllWork(work_queue *queue)
{
	while(queue->completionGoal != queue->completionCount)
	{
		DoNextWorkQueueEntry(queue);
	}

	queue->completionGoal = 0;
	queue->completionCount = 0;
}

void *ThreadProc(void *parameter)
{
	linux_thread_startup *thread = (linux_thread_startup *) parameter;
	work_queue *queue = thread->queue;

	for (;;)
	{
		if (DoNextWorkQueueEntry(queue))
		{
			sem_wait(&queue->semaphoreHandle);
		}
	}
}

void MakeQueueLinux(work_queue *queue, ui32 threadCount, linux_thread_startup *startups)
{
	queue->completionGoal = 0;
	queue->completionCount = 0;
	
	queue->nextEntryToWrite = 0;
	queue->nextEntryToRead = 0;
	
	ui32 initialCount = 0;
	
	sem_init(&queue->semaphoreHandle,0,initialCount);

	for (ui32 threadIndex = 0; threadIndex < threadCount;++threadIndex)
	{
		linux_thread_startup *startup = startups + threadIndex;
		startup->queue = queue;

		pthread_attr_t attr;
		pthread_t threadID;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		int result = pthread_create(&threadID, &attr, ThreadProc, startup);
		pthread_attr_destroy(&attr);
	}
}


void MakeQueue(work_queue *queue, ui32 threadCount)
{
	linux_thread_startup *HighPriStartups = (linux_thread_startup *)MemoryRaw(threadCount * sizeof(linux_thread_startup));

	MakeQueueLinux(queue, threadCount, HighPriStartups);
}



void CreateThread(struct thread *thrd,void *func(void *route), void *arg)
{
		pthread_attr_t attr;
		pthread_t threadID;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		int result = pthread_create(&threadID, &attr, func, arg);
		pthread_attr_destroy(&attr);

		thrd->id = threadID;
}


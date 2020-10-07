#ifndef __THREADS_H__
#define __THREADS_H__

//#define COMPILER_MSVC 1

#ifdef COMPILER_MSVC
//Windows Thread` header
#include "win_threaded_queue.h"
#else
#include "nix_threading.h"
#endif


void AddEntry(work_queue *queue, work_queue_callback *callback, void *data);
b32 DoNextWorkQueueEntry(work_queue *queue);
void CompleteAllWork(work_queue *queue);
void *ThreadProc(void *parameter);
void MakeQueue(work_queue *queue, ui32 threadCount);
void CreateThread(struct thread *thrd,void *func(void *route), void *arg);
#endif

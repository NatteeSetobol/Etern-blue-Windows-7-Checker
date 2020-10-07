#include "queue.h"


void QueuePush(Queue *queue, i32 *item)
{
	if (queue->head == NULL)
	{
		queue->head = (struct queue_info*) Memory(struct queue_info);
		queue->head->id = queue->count;
		queue->head->address = item;
		queue->head->next = NULL;
		queue->head->prev = NULL;
		queue->current = queue->head;
	}  else {
		struct queue_info *newHead = (struct queue_info*) Memory(struct queue_info);
		newHead->id = queue->count;
		newHead->address = item;
		newHead->next = queue->head;
		queue->head->prev = newHead;
		newHead->prev = NULL;
		queue->head = newHead;
	}
	queue->count++;
}

struct queue_info *QueuePop(Queue *queue)
{

	struct queue_info *tempHead = queue->head;
	if (queue->head)
	{
		if (queue->head->next)
		{
			queue->head = queue->head->next;
		} else {
			queue->head = NULL;
		}
	}
	/*
	if (tempHead)
	{
		FreeMemory((i8*)tempHead);
		tempHead=NULL;
	}
	*/
	return tempHead;
}

void QueueAddToBottom(Queue *queue, i32 *item)
{
	if (queue->head == NULL)
	{
		queue->head = (struct queue_info*) Memory(struct queue_info);
		queue->head->id = queue->count;
		queue->head->address = item;
		queue->head->next = NULL;
		queue->head->prev = NULL;
		queue->current = queue->head;
	} else {
		queue->current->next = (struct queue_info*) Memory(struct queue_info);
		queue->current->next->id = queue->count;
		queue->current->next->address = item;
		queue->current->next->next = NULL;
		queue->current->next->prev = queue->current;
		queue->current = queue->current->next;
	}
	queue->count++;
}


void QueueRemoveById(Queue *queue, i32 index)
{
	struct queue_info *q = queue->head;

	while (q != NULL)
	{
		if (q->id == index)
		{
			if (q == queue->head)
			{
				if (q->next)
				{
					if (q->next == queue->current)
					{
						queue->current->prev = NULL;
					}
					queue->head = q->next;
				} else {
					queue->head = NULL;
					queue->current = NULL;
				}
			} else 
			if (q == queue->current)
			{
				if (q->prev)
				{
					queue->current = q->prev;
					queue->current->next = NULL;
				} else {
					queue->current = NULL;
				}
			} else {
				if (q->prev)
				{
					q->prev->next = q->next;
				}
			}
			/*
			FreeMemory((i8*)q);
			q=NULL;
			*/

			break;
		}
		
		q = q->next;
	}
}

void QueueRemoveAll(Queue *queue)
{
	struct queue_info *headTemp = queue->head;

	while(queue->head)
	{
		struct queue_info *toBeDel = queue->head;

		if (queue->head->next)
		{
			queue->head = queue->head->next;
			queue->head->prev = NULL;
		} else {
			queue->head = NULL;
		}

		if (toBeDel)
		{
            
			if (toBeDel->address)
			{
				FreeMemory((i8*) toBeDel->address);
				toBeDel->address=NULL;
			}
             
			FreeMemory((i8*) toBeDel);
			toBeDel=NULL;
		}

		
	}

    queue->count = 0;
	queue->current=NULL;
	queue->head = NULL;

	#if 0
	queue_info *endTemp = queue->current;

	while(queue->current)
	{
		queue_info *temp = queue->current;

		if (queue->current->prev)
		{
			queue->current = queue->current->prev;
			
			if (queue->current)
			{
				queue->current->next = NULL;
			}
			
		} else {
			queue->current = NULL;
			queue->head = NULL;
		}

		if (temp)
		{
			if (temp->address)
			{
				FreeMemory((i8*) temp->address);
				temp->address=NULL;
			}
			FreeMemory((i8*) temp);
			temp=NULL;
		}


	}
    queue->count = 0;
	queue->current=NULL;
	queue->head = NULL;
	#endif
}

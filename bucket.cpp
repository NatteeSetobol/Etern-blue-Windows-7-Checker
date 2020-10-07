#include "bucket.h"

i32 DefaultHashFunction(s32* key, i8* value)
{
	i32 keyLen = Strlen(key);
	i32 index = -1;

	for (int i = 0; i < keyLen; i++)
	{
		index += key[i];
	}

	return index;
}

void InitBucket(struct bucket_list* buckets, int numberOfBuckets, HASHFUNCTION hashFunction)
{
	//Assert(numberOfBuckets > 5);
	buckets->total = numberOfBuckets;
	buckets->count = 0;
	buckets->buckets = NULL;
	buckets->hashFunction = hashFunction;
	//buckets->keys = CreateArray(int);
}



void AddToBucket(struct bucket_list* buckets, s32* key, i8* value)
{
	i32 index = 0;

	//Assert(buckets->total > 5);

	if (buckets->hashFunction == NULL)
	{
		buckets->hashFunction = DefaultHashFunction;
	}

	index = (buckets->hashFunction)(key, NULL);

	index = index / buckets->total;

	while (index >= buckets->total)
	{
		index = index / buckets->total;
	}

	//buckets->keys[buckets->count] = index;
	buckets->keys[buckets->count] = S32(key);
	//AddToArray(  buckets->keys,  index);

	if (buckets->buckets == NULL)
	{
		buckets->buckets = MemoryA(struct bucket, buckets->total);

		for (int i=0; i < buckets->total; i++)
		{
			
			struct bucket* currentBucket = buckets->buckets + i;
			currentBucket->filled = false;
		}
	}

	struct bucket* newBucket = buckets->buckets + index;

	if (newBucket->filled == false)
	{
		newBucket->id = buckets->count;
		newBucket->deleted = false;
		newBucket->filled = true;
		newBucket->key = key;
		newBucket->value = (i8*)value;
		newBucket->next = NULL;
	}
	else {

		struct bucket* nextNewBucket = Memory(struct bucket);
		nextNewBucket->id = buckets->count;
		nextNewBucket->filled = true;
		nextNewBucket->key = key;
		nextNewBucket->value = (i8*)value;
		//nextNewBucket->next = NULL;

		if (newBucket->next)
		{
			nextNewBucket->next = newBucket->next;
			newBucket->next = nextNewBucket;
			/*
			newBucket->next->next =  Memory(bucket);
			newBucket->next->next->id = buckets->count;
			newBucket->next->next->filled = true;
			newBucket->next->next->key = S32(key);
			newBucket->next->next->value= (i8*)value;
			newBucket->next->next->deleted =  false;
			newBucket->next->next->next = NULL;

			newBucket->next= newBucket->next->next;
			*/
		}
		else {
			newBucket->next = nextNewBucket;
			/*
			newBucket->next=  Memory(bucket);
			newBucket->next->id = buckets->count;
			newBucket->next->filled = true;
			newBucket->next->key = S32(key);
			newBucket->next->deleted = false;
			newBucket->next->value= (i8*)value;
			newBucket->next->next = NULL;
		//	newBucket = nextNewBucket;
		*/
		}
	}
	buckets->count++;
}


struct bucket* GetBucketFromKey(struct bucket_list* buckets, char* key)
{
	struct bucket* result = NULL;
	i32 index = 0;

	if (key == NULL)
	{
		return result;
	}

	if (buckets->hashFunction == NULL)
	{
		buckets->hashFunction = DefaultHashFunction;
	}

	index = (buckets->hashFunction)(key, NULL);

	index = index / buckets->total;
	while (index > buckets->total)
	{
		index = index / buckets->total;
	}

	//Assert(index > -1);


	struct bucket* tempBucket = (struct bucket*) (buckets->buckets + index);

	if (tempBucket)
	{
		if (tempBucket->deleted == false)
		{
			if (tempBucket->key)
			{
				if (StrCmp(key, tempBucket->key))
				{
					result = tempBucket;
				}
			}
			else {
				struct bucket* currentBucket = tempBucket->next;

				while (currentBucket != NULL)
				{
					if (currentBucket->key)
					{
						if (StrCmp(key, currentBucket->key))
						{
							result = currentBucket;
							break;
						}
					}

					currentBucket = currentBucket->next;
				}
			}
		}
	}


	return result;
}




i8* GetFromBucket(struct bucket_list* buckets, char* key)
{
	i8* result = NULL;
	i32 index = 0;

	if (key == NULL)
	{
		return result;
	}

	if (buckets->hashFunction == NULL)
	{
		buckets->hashFunction = DefaultHashFunction;
	}
	index = (buckets->hashFunction)(key, NULL);

	index = index / buckets->total;
	while (index > buckets->total)
	{
		index = index / buckets->total;
	}

	//Assert(index > -1);
	struct bucket* tempBucket = (struct bucket*) (buckets->buckets + index);

	if (tempBucket)
	{
		if (buckets->count > 0)
		{
			if (buckets->deleted == false)
			{
				if (StrCmp(key, tempBucket->key))
				{
					result = tempBucket->value;
				}
				else {
					struct bucket* currentBucket = tempBucket->next;

					while (currentBucket != NULL)
					{
						if (StrCmp(key, currentBucket->key))
						{
							result = currentBucket->value;
							break;
						}

						currentBucket = currentBucket->next;
					}
				}
			}
		}
	}


	return result;
}

void BucketPrintAllKeys(i8* chunk)
{
#if 0
	struct array_info* chunkInfo = (struct array_info*) chunk;
	int* allKeys = (int*)(chunk + sizeof(struct array_info));

	for (int i = 0; i < chunkInfo->count; i++)
	{
		printf("%i\n", allKeys[i]);
	}
#endif
}




void FreeAllBuckets(struct bucket_list* buckets)
{
	int i = 0;
	for (i = 0; i < buckets->count; i++)
	{
		char* index = buckets->keys[i];
		struct bucket* buck = GetBucketFromKey(buckets, index);

		if (buck)
		{
			if (buck->deleted == false)
			{
				if (buck->key)
				{
					Free(buck->key);
					buck->key = NULL;
				}

				if (buck->value)
				{
					Free(buck->value);
					buck->value = NULL;
				}

				if (buck->next)
				{
					struct bucket* nextBucketBlock = buck->next;

					while (nextBucketBlock)
					{
						if (nextBucketBlock)
						{
							struct bucket* tempNextBucketBlock = nextBucketBlock;
							nextBucketBlock = nextBucketBlock->next;

							if (tempNextBucketBlock->key)
							{
								FreeMemory((i8*)tempNextBucketBlock->key);
								tempNextBucketBlock->key = NULL;
							}

							if (tempNextBucketBlock->value)
							{
								FreeMemory((i8*)tempNextBucketBlock->value);
								tempNextBucketBlock->value = NULL;
							}

							if (tempNextBucketBlock)
							{
								FreeMemory((i8*)tempNextBucketBlock);
								tempNextBucketBlock = NULL;
							}
						}
					}
				} else {

				}
				buck->deleted = true;
			} else {
				/*
				if (buck->key)
				{
					Free(buck->key);
					buck->key = NULL;
				}
				*/
			}
		}

		if (index)
		{
			Free(index);
			index = NULL;
		}
	}

	if (buckets->buckets)
	{
		FreeMemory((i8*)buckets->buckets);
		buckets->buckets = NULL;
	}
	buckets->count = 0;
	/*
	if (buckets->keys)
	{
		FreeMemory(buckets->keys);
		buckets->keys=NULL;
	}
	"*/
}

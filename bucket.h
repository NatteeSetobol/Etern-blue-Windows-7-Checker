#ifndef __BUCKET_H__
#define __BUCKET_H__

#include "required/intrinsic.h"
#include "required/nix.h"
#include "required/memory.h"
#include "required/platform.h"
#include "./stringz.h"


//#include <typeinfo>

/*NOTE(): Bucket Example:

	bucket_list buckets = {};
	InitBucket(&buckets);

	s32 *one = S32("one\n");
	s32 *two = S32("two\n");
	s32 *three = S32("three\n");
	s32 *four = S32("three3\n");
	s32 *five= S32("three4\n");
	s32 *six= S32("three5\n");

	AddToBucket(&buckets,"one", (i8*)  one);
	AddToBucket(&buckets,"two", (i8*) two); 
	AddToBucket(&buckets,"three", (i8*) three); 
	AddToBucket(&buckets,"four", (i8*) four); 
	AddToBucket(&buckets,"five", (i8*) five); 
	AddToBucket(&buckets,"six", (i8*) six); 
	FreeAllBuckets(&buckets);
*/


typedef i32 (*HASHFUNCTION)(char* key, i8* ptrToValue);
#define AddS32ToBucket(chunk,key,value) do {  char *tempKey = S32(key);char *tempChar = S32((char*)value); AddToBucket(chunk,tempKey, (i8*) tempChar);   }while(0)

struct bucket
{
	i32 id;
	bool32 deleted;
	s32 *key;
	bool filled;
	i8 *value;
	struct bucket *next;
};


struct bucket_list
{
	/*NOTE: leave this for now until the array is fixed*/
	//int keys[256];
	char* keys[256];
	i32 total;
	i32 count;
	struct bucket *buckets;
	HASHFUNCTION hashFunction;
	bool32 deleted;
};

i32 DefaultHashFunction(s32* key, i8* value);
void InitBucket(struct bucket_list* buckets, int numberOfBuckets, HASHFUNCTION hashFunction);
void AddToBucket(struct bucket_list* buckets, s32* key, i8* value);
struct bucket* GetBucketFromKey(struct bucket_list* buckets, char* key);
i8* GetFromBucket(struct bucket_list* buckets, char* key);
void BucketPrintAllKeys(i8* chunk);
void FreeAllBuckets(struct bucket_list* buckets);


#endif

#ifndef __INTRINSIC__
#define __INTRINSIC__

#ifdef COMPILER_MSVC
#include <Windows.h>
#endif
#include <stdint.h>
#include <stddef.h>
#include <limits.h>
#include <float.h>
#include "math.h"

#define i8 int8_t
#define i16 int16_t
#define i32 int32_t
#define i64 int64_t

#define ui8 uint8_t
#define ui16 uint16_t
#define ui32 uint32_t
#define ui64 uint64_t

#define int8 int8_t
#define int16 int16_t
#define int32 int32_t
#define int64 int64_t

#define uint8 uint8_t
#define uint16 uint16_t
#define uint32 uint32_t
#define uint64 uint64_t

#define bool32 ui32
#define b32 ui32

#define uchar unsigned char

typedef float real32;
typedef double real64;

#define Real32Maximum FLT_MAX

#if !defined(internal)
#define internal static
#endif
#define local_persist static
#define global_variable static

#define Pi32 3.14159265359f
#define Tau32 6.28318530717958647692f

#define Minimum(A, B) ((A < B) ? (A) : (B))
#define Maximum(A, B) ((A > B) ? (A) : (B))

#define MIN(A,B) Minimum(A,B)
#define MAX(A,B) Maximum(A,B)

#define Kilobytes(value) ((value)*1024L)
#define Megabytes(value) (Kilobytes(value)*1024L)
#define Gigabytes(value) (Megabytes(value)*1024L)
#define Terabytes(value) (Gigabytes(value)*1024L)

#define AlignPow2(Value, Alignment) ((Value + ((Alignment) - 1)) & ~((Alignment) - 1))
#define Align4(Value) ((Value + 3) & ~3)
#define Align8(Value) ((Value + 7) & ~7)
#define Align16(Value) ((Value + 15) & ~15)
 

#ifdef COMPILER_MSVC
#define CompletePreviousWritesBeforeFuturesRead _ReadBarrier()
#define CompletePreviousWritesBeforeFuturesWrites _WriteBarrier()
inline uint32 AtomicCompareExchangeUIint32(uint32 volatile *value, uint32 New, uint32 expected)
{
	ui32 result = _InterlockedCompareExchange((long *)value,New,expected);

	return result;
}
#elif COMPILER_LLVM
#define CompletePreviousWritesBeforeFuturesRead asm volatile(""::: "memory");
#define CompletePreviousWritesBeforeFuturesWrites asm volatile(""::: "memory")

inline ui32 AtomicCompareExchangeUIint32(uint32 volatile *value, ui32 New, ui32 expected)
{
	ui32 result = __sync_val_compare_and_swap(value,New,expected);

	return result;
}
#else
	//TODO: Other compilers/platforms
#endif

inline uint32 SafeTruncateUInt64(uint64 value)
{
	//Assert(value <= 0xFFFFFFFF);
	uint32 result = (uint32) value;

	return result;
}

inline int32 SignOf(int32 value)
{
	int32 result = (value >=0) ? 1 : -1;
	return result;
}

inline real32 SignOf(real32 value)
{
	real32 result = (value >=0) ? 1.0f : -1.0f;

	return result;
}

inline real32 SquareRoot(real32 Real32)
{
	real32 result = sqrtf(Real32);

	return result;
}

inline real32 AbsoluteValue(real32 Real32)
{
	real32 result = fabs(Real32);

	return result;
}

inline uint32 RotateLeft(uint32 value, int32 amount)
{
	#ifdef COMPILER_MSVC
	uint32 result = _rotl(value,amount);
	#else
		amount &= 31;
		uint32 result = ((value << amount) | (value >> (32 - amount)));
	#endif

	return result;
}

inline uint32 RotateRight(uint32 value, int32 amount)
{
	#ifdef COMPILER_MSVC
		uint32 result = _rotr(value,amount);
	#else
		amount &= 31;
		uint32 result = ((value >> amount) | (value << (32 - amount)));
	#endif

	return result;
}

inline int32 RoundReal32ToInt32(real32 Real32)
{
	int32 result = (int32)roundf(Real32);

	return result;
}

inline uint32 RoundReal32ToUInt32(real32 Real32)
{
	uint32 result = (uint32)roundf(Real32);
	return result;
}

inline int32 FloorReal32ToInt32(real32 Real32)
{
	int32 result = (int32)floorf(Real32);
	return result;
}

inline int32 CeilReal32ToInt32(real32 Real32)
{
	int32 result = (int32)ceilf(Real32);
	return result;
}

inline int32 TruncateReal32ToInt32(real32 Real32)
{
	int32 result = (int32) Real32;
	return result;
}

inline real32 Sin(real32 angle)
{
	real32 result = sinf(angle);
	return result;
}

inline real32 Cos(real32 angle)
{
	real32 result = cosf(angle);
	return result;
}

inline real32 ATan2(real32 y, real32 x)
{
	real32 result = atan2f(y,x);
	return result;
}

struct bit_scan_result
{
	bool32 found;
	uint32 index;
};

inline bit_scan_result FindLeastSignificantSetBit(uint32 value)
{
	bit_scan_result result ={};

	#ifdef COMPILER_MSVC
		result.found = _BitScanForward((unsigned long *)&result.index,value);
	#else
		for (uint32 test = 0;
				test < 32;
				++test)
		{
			if (value & (1 << test))
			{
				result.index = test;
				result.found = true;
			}
		}
	#endif

	return result;
}


#endif

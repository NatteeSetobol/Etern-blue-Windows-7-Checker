#ifndef _MATH_H_
#define _MATH_H_
#include <math.h>


//NOTES(): This was based on HandmadeHero's handmade_math.h. For more infomation
//go to the site http://handmadehero.org

union v2
{
	struct
	{
		real32 x,y;
	};

	struct
	{
		real32 u,v;
	};

	struct
	{
		real32 width, height;
	};

	real32 E[2];
};

union v3
{
	struct
	{
		real32 x,y,z;
	};
	struct
	{
		real32 u,v,w;
	};

	struct
	{
		real32 r,g,b;
	};

	struct
	{
		v2 xy;
		real32 ignored0_;
	};

	struct 
	{
		real32 ignored1_;
		v2 yz;
	};

	struct 
	{
		v2 uv;
		real32 ignored2_;
	};

	struct
	{
		real32 ignored3_;
		v2 vw;
	};

	real32 E[3];

};

union v4
{
	struct
	{
		union
		{
			v3 xyz;
			struct
			{
				real32 x,y,z;
			};
		};

		real32 w;
	};

	struct
	{
		union
		{
			v3 rgb;
			struct
			{
				real32 r,g,b;
			};
		};

		real32 a;
	};


	struct
	{
		v2 xy;
		real32 ignored0_;
		real32 ignored1_;
	};

	struct
	{
		real32 ignored2_;
		v2 yz;
		real32 ignored3_;
	};

	struct
	{
		real32 ignored4_;
		real32 ignored5_;
		v2 zw;
	};

	real32 E[4];
};

struct rectangle2
{
	v2 Min;
	v2 Max;
};

struct rectangle3
{
	v3 Min;
	v3 Max;
};

//////NOTES: V2 Functions
inline v2 V2i(int32 x, int32 y)
{
	v2 result = {(real32)x, (real32)y};

	return result;
}

inline v2 V2i(uint32 x, uint32 y)
{
	v2 result = {(real32)x, (real32)y};

	return result;
}

inline v2 V2(real32 x, real32 y)
{
	v2 result;

	result.x = x;
	result.y = y;

	return result;
}
/////////////////////////////////////


//////NOTES: V3 Functions

inline v3 V3(real32 x, real32 y, real32 z)
{
	v3 result;

	result.x = x;
	result.y = y;
	result.z = z;

	return result;
}

inline v3 V3(v2 xy, real32 z)
{
	v3 result;

	result.x = xy.x;
	result.y = xy.y;
	result.z = z;

	return result;
}

/////////////////////////////////////


//////NOTES: V4 Functions

inline v4 V4(real32 x, real32 y, real32 z, real32 w)
{
	v4 result;

	result.x = x;
	result.y = y;
	result.z = z;
	result.w = w;

	return result;
}

inline v4 V4(v3 xyz, real32 w)
{
	v4 result;

	result.xyz = xyz;
	result.w = w;

	return result;
}

/////////////////////////////////////

//NOTES(): Scalar operations


//NOTES(): this is suppose to be intrinsics.h
#if 0
inline real32 SquareRoot(real32 Real32)
{
	real32 result = sqrtf(Real32);

	return result;
}
#endif
inline real32 Square(real32 a)
{
	real32 result = a*a;

	return result;
}

inline real32 Lerp(real32 a, real32 t, real32 b)
{
	real32 result = (1.0f - t) * a + t * b;

	return result;
}

inline real32 Clamp(real32 Min, real32 value, real32 Max)
{
	real32 result = value;

	if (result < Min)
	{
		result = Min;
	} else if (result > Max)
	{
		result = Max;
	}

	return result;
}

inline real32 Clamp01(real32 value)
{
	real32 result = Clamp(0.0f, value, 1.0f);

	return result;
}

inline real32 Clamp01MapToRange(real32 Min, real32 t, real32 Max)
{
	real32 result = 0.0f;

	real32 range = Max - Min;

	if (range != 0.0f)
	{
		result = Clamp01((t - Min) / range);
	}

	return result;
}


inline real32 SafeRatioN(real32 numerator, real32 divisor, real32 n)
{
	real32 result = n;

	if (divisor != 0.0f)
	{
		result = numerator / divisor;
	}

	return result;
}

inline real32 SafeRatio0(real32 numerator, real32 divisor)
{
	real32 result = SafeRatioN(numerator, divisor,0.0f);

	return result;
}

inline real32 SafeRatio1(real32 numerator, real32 divisor)
{
	real32 result = SafeRatioN(numerator,divisor,1.0f);

	return result;
}

/////////////////////////////////////

/////// NOTE(): v2 Operations //////
inline v2 Perp(v2 a)
{
	v2 result = {-a.y,a.x};
	return result;
}

inline v2 operator*(real32 a, v2 b)
{
	v2 result;

	result.x = a*b.x;
	result.y = a*b.y;

	return result;
}

inline v2 operator*(v2 b, real32 a)
{
	v2 result = a*b;

	return result;
}

inline v2 & operator*=(v2 &b, real32 a)
{
	b = a * b;

	return b;
}

inline v2 operator-(v2 a)
{
	v2 result;

	result.x = -a.x;
	result.y = -a.y;

	return result;
}

inline v2 operator+(v2 a, v2 b)
{
	v2 result;

	result.x = a.x + b.x;
	result.y = a.y + b.y;

	return result;
}

inline v2 & operator+=(v2 &a, v2 b)
{
	a = a + b;

	return a;
}

inline v2 operator-(v2 a, v2 b)
{
	v2 result;

	result.x = a.x - b.x;
	result.y = a.y - b.y;

	return result;
}

inline v2 Hadamard(v2 a, v2 b)
{
	v2 result = {a.x*b.x,a.y*b.y};

	return result;
}

inline real32 Inner(v2 a, v2 b)
{
	real32 result = a.x*b.x + a.y*b.y;

	return result;
}


inline real32 LengthSq(v2 a)
{
	real32 result = Inner(a,a);

	return result;
}

inline real32 Length(v2 a)
{
	real32 result = SquareRoot(LengthSq(a));
	return result;
}

inline v2 Clamp01(v2 value)
{
	v2 result;

	result.x = Clamp01(value.x);
	result.y = Clamp01(value.y);

	return result;
}

/////////// v3 operations ///////

inline v3 operator*(real32 a, v3 b)
{
	v3 result;

	result.x = a*b.x;
	result.y = a*b.y;
	result.z = a*b.z;

	return result;
}

inline v3 operator*(v3 b, real32 a)
{
	v3 result = a*b;

	return result;
}

inline v3 & operator*=(v3 &b, real32 a)
{
	b = a*b;

	return b;
}

inline v3 operator-(v3 a)
{
	v3 result;

	result.x = -a.x;
	result.y = -a.y;
	result.z = -a.z;

	return result;
}

inline v3 operator+(v3 a, v3 b)
{
	v3 result;

	result.x = a.x + b.x;
	result.y = a.y + b.y;
	result.z = a.z + b.z;

	return result;
}

inline v3 & operator+=(v3 &a,v3 b)
{
	a = a + b;
	return a;
}

inline v3 operator-(v3 a, v3 b)
{
	v3 result;

	 result.x = a.x - b.x;
	 result.y = a.y - b.y;
	 result.z = a.z - b.z;

	 return result;
}

inline v3 Hadamard(v3 a, v3 b)
{
	v3 result = {a.x*b.x,a.y*b.y,a.z*b.z};

	return result;
}

inline real32 Inner(v3 a, v3 b)
{
	real32 result = a.x * b.x + a.y*b.y + a.z*b.z;

	return result;
}

inline real32 LengthSq(v3 a)
{
	real32 result = Inner(a,a);

	return result;
}

inline real32 Length(v3 a)
{
	real32 result = SquareRoot(LengthSq(a));
	return result;
}

inline v3 Normalize(v3 a)
{
	v3 result = a * (1.0f /Length(a));

	return result;
}

inline v3 Clamp01(v3 value)
{
	v3 result;

	result.x = Clamp01(value.x);
	result.y = Clamp01(value.y);
	result.z = Clamp01(value.z);

	return result;
}

inline v3 Lerp(v3 a, real32 t, v3 b)
{
	v3 result = (1.0f - t)*a + t*b;

	return result;
}
/////////////////////////////////////


/////////// v4 operations ///////

inline v4 operator*(real32 a, v4 b)
{
	v4 result;

	result.x = a*b.x;
	result.y = a*b.y;
	result.z = a*b.z;
	result.w = a*b.w;

	return result;
}

inline v4 operator*(v4 b, real32 a)
{
	v4 result = a*b;

	return result;
}

inline v4 & operator*=(v4 &b, real32 a)
{
	b = a * b;

	return b;
}

inline v4 operator-(v4 a)
{
	v4 result;

	result.x = -a.x;
	result.y = -a.y;
	result.z = -a.z;
	result.w = -a.w;

	return result;
}

inline v4 operator+(v4 a, v4 b)
{
	v4 result;

	result.x = a.x + b.x;
	result.y = a.y + b.y;
	result.z = a.z + b.z;
	result.w = a.w + b.w;

	return result;
}

inline v4 & operator +=(v4 &a, v4 b)
{
	a = a + b;

	return a;
}

inline v4 operator-(v4 a, v4 b)
{
	v4 result;

	result.x = a.x - b.x;
	result.y = a.y - b.y;
	result.z = a.z - b.z;
	result.w = a.w - b.w;

	return result;
}

inline v4 Hadamard(v4 a, v4 b)
{
	v4 result = {a.x*b.x, a.y*b.y,a.z*b.z,a.w*b.w};

	return result;
}


inline real32 Inner(v4 a, v4 b)
{
	real32 result = a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;

	return result;
}

inline real32 LengthSq(v4 a)
{
	real32 result = Inner(a,a);

	return result;
}

inline real32 Length(v4 a)
{
	real32 result = SquareRoot(LengthSq(a));

	return result;
}

inline v4 Clamp01(v4 value)
{
	v4 result;

	result.x = Clamp01(value.x);
	result.y = Clamp01(value.y);
	result.z = Clamp01(value.z);
	result.w = Clamp01(value.w);

	return result;
}

inline v4 Lerp(v4 a, real32 t, v4 b)
{
	v4 result = (1.0f - t) * a + t*b;

	return result;
}

/////////////////////////////////////


//////// NOTE(): Rectangle 2

inline v2 GetMinCorner(rectangle2 rect)
{
	v2 result = rect.Min;
	return result;
}

inline v2 GetMaxCorner(rectangle2 rect)
{
	v2 result = rect.Max;

	return result;
}

inline v2 GetDim(rectangle2 rect)
{
	v2 result = rect.Max = rect.Min;
	return result;
}

inline v2 GetCenter(rectangle2 rect)
{
	v2 result = 0.5f*(rect.Min + rect.Max);
	return result;
}

inline rectangle2 RectMinMax(v2 Min, v2 Max)
{
	rectangle2 result;

	result.Min = Min;
	result.Max = Max;

	return result;

}

inline rectangle2 RectMinDim(v2 Min, v2 dim)
{
	rectangle2 result;

	result.Min = Min;

	return result;
}

inline rectangle2 RectCenterHalfDim(v2 center, v2 halfDim)
{
	rectangle2 result;

	result.Min = center - halfDim;
	result.Max = center + halfDim;

	return result;
}

inline rectangle2 AddRadiusTo(rectangle2 a, v2 radius)
{
	rectangle2 result;
	result.Min = a.Min - radius;
	result.Max = a.Max + radius;

	return result;
}

inline rectangle2 RectCenterDim(v2 center, v2 dim)
{
	rectangle2 result = RectCenterHalfDim(center,0.5f*dim);

	return result;
}

inline bool32 IsInRectangle(rectangle2 rectangle, v2 test)
{
	bool32 result = ((test.x >= rectangle.Min.x) &&
					 (test.y >= rectangle.Min.y) &&
					 (test.x < rectangle.Max.x) &&
					 (test.y < rectangle.Max.y));

	return result;
}

inline v2 GetBarycentric(rectangle2 a, v2 p)
{
	v2 result;

	result.x = SafeRatio0(p.x - a.Min.x, a.Max.x - a.Min.x);
	result.y = SafeRatio0(p.y - a.Min.y, a.Max.y - a.Min.y);

	return result;
}

/////////////////////////////////////

//////// NOTE(): Rectangle 3

inline v3 GetMinCorner(rectangle3 rect)
{
	v3 result = rect.Min;
	return result;
}

inline v3 GetMaxCorner(rectangle3 rect)
{
	v3 result = rect.Max;

	return result;
}

inline v3 GetDim(rectangle3 rect)
{
	v3 result = rect.Max - rect.Min;

	return result;
}

inline v3 GetCenter(rectangle3 rect)
{
	v3 result = 0.5f * (rect.Min + rect.Max);

	return result;
}


inline rectangle3 RectMinMax(v3 Min, v3 Max)
{
	rectangle3 result;

	result.Min = Min;
	result.Max = Max;

	return result;
}

inline rectangle3 RectMinDim(v3 Min, v3 dim)
{
	rectangle3 result;

	result.Min = Min;
	result.Max = Min + dim;

	return result;
}

inline rectangle3 RectCenterHalfDim(v3 center, v3 halfDim)
{
	rectangle3 result;

	result.Min = center - halfDim;
	result.Max = center + halfDim;

	return result;
}

inline rectangle3 AddRadiusTo(rectangle3 a, v3 radius)
{
	rectangle3 result;

	result.Min = a.Min - radius;
	result.Max = a.Max + radius;

	return result;
}

inline rectangle3 Offset(rectangle3 a, v3 offset)
{
	rectangle3 result;

	result.Min = a.Min + offset;
	result.Max = a.Max + offset;

	return result;
}

inline rectangle3 RectCenterDim(v3 center, v3 dim)
{
	rectangle3 result = RectCenterHalfDim(center, 0.5f*dim);

	return result;
}

inline bool32 IsInRectangle(rectangle3 rectangle, v3 test)
{
	bool32 result = ((test.x >= rectangle.Min.x) &&
					 (test.y >= rectangle.Min.y) &&
					 (test.z >= rectangle.Min.z) &&
					 (test.x < rectangle.Max.x) &&
					 (test.y < rectangle.Max.y) &&
					 (test.z < rectangle.Max.z));

	return result;
}

inline bool32 RectanglesIntersect(rectangle3 a, rectangle3 b)
{
	bool32 result = !((b.Max.x <= a.Min.x) ||
					  (b.Min.x >= a.Max.x) ||
					  (b.Max.y <= a.Min.y) ||
					  (b.Min.y >= a.Max.y) ||
					  (b.Max.z <= a.Min.z) ||
					  (b.Min.z >= a.Max.z));

	return result;
}
	
inline v3 GetBarycentric(rectangle3 a, v3 p)
{
	v3 result;

	result.x = SafeRatio0(p.x - a.Min.x, a.Max.x - a.Min.x);
	result.y = SafeRatio0(p.y - a.Min.y, a.Max.y - a.Min.y);
	result.z = SafeRatio0(p.z - a.Min.z, a.Max.z - a.Min.z);

	return result;
}

inline rectangle2 ToRectangleXY(rectangle3 a)
{
	rectangle2 result;

	result.Min = a.Min.xy;
	result.Max = a.Max.xy;

	return result;
}

//////////////////////////////////////////////

struct rectangle2i
{
	int32 MinX, MinY;
	int32 MaxX, MaxY;
};

inline rectangle2i Intersect(rectangle2i a, rectangle2i b)
{
	rectangle2i result;
	result.MinX = (a.MinX < b.MinX) ? b.MinX : a.MinX;
	result.MinY = (a.MinY < b.MinY) ? b.MinY : a.MinY;
	result.MaxX = (a.MaxX > b.MaxX) ? b.MaxX : a.MaxX;
	result.MaxY = (a.MaxY > b.MaxY) ? b.MaxY : a.MaxY;

	return result;
}

inline rectangle2i Union(rectangle2i a, rectangle2i b)
{
	rectangle2i result;

	result.MinX = (a.MinX < b.MinX) ? a.MinX : b.MinX;
	result.MinY = (a.MinY < b.MinY) ? a.MinY : b.MinY;
	result.MaxX = (a.MaxX > b.MaxX) ? a.MaxX : b.MaxX;
	result.MaxY = (a.MaxY > b.MaxY) ? a.MaxY : b.MaxY;

	return result;
}

inline int32 GetClampedRectArea(rectangle2i a)
{
	int32 width = (a.MaxX - a.MinX);
	int32 height = (a.MaxY - a.MinY);
	int32 result = 0;

	if ((width > 0) && (height > 0))
	{
		result = width*height;
	}
	
	return result;
}

inline bool32 HasArea(rectangle2i a)
{
	bool32 result = ((a.MinX < a.MaxX) && (a.MinY < a.MaxY));

	return result;
}

inline rectangle2i InvertedInfinityRectangle()
{
	rectangle2i result;

	result.MinX = result.MinY = INT_MAX;
	result.MaxX = result.MaxY = -INT_MAX;

	return result;
}

inline v4 SRGB255ToLinear(v4 c)
{
	v4 result;

	real32 inv255 = 1.0f/ 255.0f;

	result.r = Square(inv255*c.r);
	result.g = Square(inv255*c.g);
	result.b = Square(inv255*c.b);
	result.a = inv255*c.a;

	return result;
}

inline v4 Linear1ToSRGB255(v4 c)
{
	v4 result;

	real32 one255 = 255.0f;

	result.r = one255*SquareRoot(c.r);
	result.g = one255*SquareRoot(c.g);
	result.b = one255*SquareRoot(c.b);
	result.a = one255*c.a;

	return result;

}

////////////////////////////////
#endif

#ifndef __ASSERT_H__
#define __ASSERT_H__
#include <stdio.h>
#define Assert(x) if(!(x)){printf("assertion in %s on line %i\n",__FILE__, __LINE__); MemoryResults();  exit(0);}
#define assert(x) if(!(x)){printf("assertion in %s on line %i\n",__FILE__, __LINE__);  MemoryResults(); exit(1);}
#endif



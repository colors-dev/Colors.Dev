// color_support.c
#include "color_support.h"
#include <malloc.h>            // For free

COLORS_DEV_API void FreeAllocPtr(void* p) 
{ 
	if(p)
		free(p); 
}
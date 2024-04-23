#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <cstdio>


// TODO: This is an example of a library function
inline void fnDependencyProj(float test)
{
	printf("This is a test %f \n", test);
}

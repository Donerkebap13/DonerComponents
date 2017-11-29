#pragma once

// Check windows
#if _WIN32 || _WIN64
	#if _WIN64
		#define ENV64BIT
	#else
		#define ENV32BIT
	#endif
#endif

// Check GCC
#if __GNUC__
	#if __x86_64__ || __ppc64__
		#define ENV64BIT
	#else
		#define ENV32BIT
	#endif
#endif


#if defined(ENV64BIT)
static_assert(sizeof(void*) == 8, "ENV64BIT: Error: pointer should be 8 bytes.");
#elif defined (ENV32BIT)
static_assert(sizeof(void*) == 4, "ENV32BIT: Error: pointer should be 4 bytes.");
#else
#error "Must define either ENV32BIT or ENV64BIT".
#endif

//
//	IMPORTANT -- This file is here just to get around what
//		is probably simply a local implementation bug
//		you should remove this file and only use
//		if compiling produces strange link errors
//


#ifdef assert
#undef assert
#endif
#ifdef _assert
#undef _assert
#endif

#define assert(e) ((void)0)


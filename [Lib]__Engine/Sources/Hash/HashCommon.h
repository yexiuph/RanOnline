#ifndef _HASH_COMMON_H_
#define _HASH_COMMON_H_

/**
 * \ingroup EngineLib
 *
 * \date 2006-10-25
 *
 * \author Jgkim
 *
 * \par license
 *
 * http://www.codeproject.com/cpp/chash.asp 의 코드를 약간 수정했다. 
 *
 */
#ifndef _QWORD_DEFINED
#define _QWORD_DEFINED
#ifdef QWORD
#undef QWORD
typedef __int64 QWORD, *LPQWORD;
#endif
#endif

#define MAKEQWORD(a, b)	\
	((QWORD)( ((QWORD) ((DWORD) (a))) << 32 | ((DWORD) (b))))

#define LODWORD(l) \
	((DWORD)(l))

#ifdef HIDWORD
#undef HIDWORD
#endif
#define HIDWORD(l) \
	((DWORD)(((QWORD)(l) >> 32) & 0xFFFFFFFF))

// Read 4K of data at a time (used in the C++ streams, Win32 I/O, and assembly functions)
#define MAX_BUFFER_SIZE	4096

// Map a "view" size of 10MB (used in the filemap function)
#define MAX_VIEW_SIZE	10485760

#endif // _HASH_COMMON_H_

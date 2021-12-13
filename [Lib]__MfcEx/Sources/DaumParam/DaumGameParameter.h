#pragma once

#pragma warning( disable: 4702 )  /* more than 64k source lines */
#include <list>

#ifndef MAX_DGIP_LENGTH
	#define MAX_DGIP_LENGTH	500
#endif

#ifndef MAX_DGPORT_LENGTH
	#define MAX_DGPORT_LENGTH 500
#endif

#ifndef MAX_DGPARAMETER_LENGTH
	#define MAX_DGPARAMETER_LENGTH 500
#endif


// _CDaumGameMemoryBlock is needed to CDaumGameParameter
class _CDaumGameMemoryBlock;

class CDaumGameParameter
{
public:
	CDaumGameParameter( void );
	~CDaumGameParameter( void );

	BOOL	Init( void );
	BOOL	Flush( void );
	void	Empty( void );
	void	Destroy( void );

	BOOL	Set( LPSTR lpBuffer );
	BOOL	Set( LPCSTR lpszKeyName, LPSTR lpBuffer );
	BOOL	Get( LPCSTR lpszKeyName, LPSTR lpBuffer, DWORD nSize );

protected:
	UINT	m_uDGPFormat;
	BOOL	m_bModified;
	_CDaumGameMemoryBlock *m_pMemoryBlock;
};

class _CDaumGameMemoryBlock
{
public:
	_CDaumGameMemoryBlock( void );
	~_CDaumGameMemoryBlock( void );

	BOOL Assign( HANDLE hBlock );
	HANDLE CreateHandle( void );

	LPSTR Find( LPCSTR lpszKeyName );
	BOOL Put( LPSTR lpBuffer );
	BOOL Put( LPCSTR lpszKeyName, LPSTR lpBuffer );
	BOOL Get( LPCSTR lpszKeyName, LPSTR lpBuffer, DWORD nSize );

protected:
	typedef std::list<CHAR *>	StrList;
	typedef StrList::iterator	StrListItr;

	StrList m_StrList;
};

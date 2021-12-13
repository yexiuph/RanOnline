#ifndef _CDAUMGAMEAUTH_H_
#define _CDAUMGAMEAUTH_H_

#if _MSC_VER >= 1000
#pragma once
#endif

#include "DaumGameCrypt.h"

class CDaumGameAuth
{
public:
	CDaumGameAuth( void );
	~CDaumGameAuth( void );

	BOOL	Init( IN LPCTSTR szKey );
	BOOL	SetSource( IN LPCTSTR szSourceString );
	BOOL	GetData( IN LPCTSTR szKeyName, OUT LPTSTR szBuffer, IN int nMaxLength );
	BOOL	IsTimeExpired( void );

protected:
	enum {
		MAX_SOURCE_LENGTH	= 1024,
		MAX_KEY_LENGTH		= 256
	};
	enum {
		EXPIRED_SECS		= 6 * 60 * 60		/* 6 hours */
	};

protected:
	CDaumGameCrypt	*m_pCryptor;
	TCHAR			m_szSourceString[MAX_SOURCE_LENGTH];

};

#endif /* _CDAUMGAMEAUTH_H_ */

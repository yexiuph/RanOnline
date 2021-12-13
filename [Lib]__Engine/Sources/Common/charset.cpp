#include "pch.h"

#include "charset.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace CHARSET
{
	int g_nCodePage = 0;

	void SetCodePage ( int nCodePage )
	{
		g_nCodePage = nCodePage;
	}

	bool IsLeadChar ( char cC )
	{
		if( IsDBCSLeadByteEx( g_nCodePage, (BYTE)cC ) )
			return true;
		else
			return false;
	}
};
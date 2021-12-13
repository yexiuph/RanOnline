#include "pch.h"
#include "./seqrandom.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace seqrandom
{
	WORD				n_wDetail = 0;

	WORD				n_wIterate = 0;
	std::vector<WORD>	n_vecBuffer;

	BOOL init ( WORD wdetail, WORD wcopy )
	{
		n_vecBuffer.clear ();

		n_wDetail = wdetail;
		WORD wMax = (WORD) powf(10.0f,n_wDetail);

		for ( WORD c=0; c<wcopy; ++c )
		{
			for ( WORD i=0; i<wMax; ++i )		n_vecBuffer.push_back(i);
		}

		std::random_shuffle ( n_vecBuffer.begin(), n_vecBuffer.end() );

		return TRUE;
	}

	WORD get ()
	{
		if ( n_vecBuffer.size() <= n_wIterate )		n_wIterate = 0;
		return n_vecBuffer[n_wIterate++];
	}

	float getpercent ()
	{
		return ( get()*100.0f / powf(10.0f,n_wDetail) );
	}
};

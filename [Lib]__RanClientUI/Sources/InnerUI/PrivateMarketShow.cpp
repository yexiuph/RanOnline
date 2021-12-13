#include "pch.h"
#include "PrivateMarketShow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CPrivateMarketShow::CPrivateMarketShow ()
	: m_dwGaeaID ( UINT_MAX )
	, m_bUsedMemPool( false )
{
}

CPrivateMarketShow::~CPrivateMarketShow ()
{
}
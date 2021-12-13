#pragma once

#include "./GLInventory.h"

enum
{
	//EMTRADE_INVEN_X = 10,
	//EMTRADE_INVEN_Y = 4,

	EMTRADE_INVEN_X = 6,
	EMTRADE_INVEN_Y = 2,

	EMTRADE_BLOCK_TIME = 3,
};

struct NET_MSG_GENERIC;

class GLTrade
{
protected:
	DWORD		m_dwTargetID;
	BOOL		m_bAgree;

	float		m_fBlockTimer;
	GLInventory	m_sTradeBox;
	LONGLONG	m_lnMoney;

public:
	void Reset ();
	BOOL Valid ()						{	return ( m_dwTargetID != GAEAID_NULL ); }

public:
	void SetTarget ( DWORD dwTarID )	{ m_dwTargetID = dwTarID; }
	DWORD GetTarget ()					{ return m_dwTargetID; }

public:
	BOOL IsAgreeAble ()					{ return m_fBlockTimer==0.0f; }
	BOOL SetAgree ();
	void ReSetAgree ()					{ m_bAgree = FALSE; }
	BOOL GetAgree ()					{ return m_bAgree; }

public:
	void SetBlockTime ()				{ m_fBlockTimer = EMTRADE_BLOCK_TIME; }
	void BlockAgree ();

public:
	void SetMoney ( const LONGLONG lnMoney )	{ m_lnMoney = lnMoney; }
	LONGLONG GetMoney ()						{ return m_lnMoney; }

	//	server.
	BOOL InsertItem ( const WORD wPosX, const WORD wPosY, SINVENITEM *pInvenItem );
	
	//	client
	BOOL InsertItem ( SINVENITEM *pInvenItem );
	BOOL DeleteItem ( const WORD wPosX, const WORD wPosY );

	GLInventory& GetTradeBox ()				{ return m_sTradeBox; }

public:
	SINVENITEM* GetItem ( const WORD wPosX, const WORD wPosY );

public:
	HRESULT FrameMove ( float fTime, float fElapsedTime );

public:
	GLTrade(void);
	~GLTrade(void);
};

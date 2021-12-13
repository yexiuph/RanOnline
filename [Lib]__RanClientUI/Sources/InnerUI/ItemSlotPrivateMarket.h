#pragma	once

#include "ItemSlot.h"

class	CItemSlotPrivateMarket : public CItemSlot
{
public:
	CItemSlotPrivateMarket ();
	virtual	~CItemSlotPrivateMarket ();

	void	CreateFlipImage ( int nMaxColumn );

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

private:
	CUIControl*	CreateFlipImage ( const char* szFlip );

public:
	void	SetFlipItem ( WORD wIndex, BOOL bFlip );
	BOOL	IsFlipItem ( WORD wIndex );

private:
	CUIControl*		m_pFlipImage[nLIMIT_COLUMN];
};
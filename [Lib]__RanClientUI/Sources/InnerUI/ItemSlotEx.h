//	아이템 슬롯
//
//	최초 작성자 : 성기엽
//	이후 수정자 : 
//	로그
//		[2003.12.6]
//			@ 작성
//

#pragma	once

#include "ItemSlot.h"

class	CItemSlotEx : public CItemSlot
{
public:
	CItemSlotEx ();
	virtual	~CItemSlotEx ();

public:
	void	CreateSubTradeInventory ( int nMaxColumn );
	void	CreateSubTrade ( int nMaxColumn );

private:
	CUIControl*	CreateFlipImage ( const char* szFlip );

public:
	void	SetFlipItem ( int nIndex, BOOL bFlip );
	BOOL	IsFlipItem ( int nIndex );

private:
	CUIControl*		m_pFlipImage[nLIMIT_COLUMN];
};
#pragma once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"
#include "GLDefine.h"
#include "GLCharDefine.h"

class	CItemSlot;
class	CBasicScrollBarEx;
class	GLInventory;
struct	SINVENITEM;

class CVNGainInvenPage : public CUIGroup
{
	static	const	int	nSTARTLINE;
	static	const	int	nOUTOFRANGE;

protected:
	enum
	{
		VNGAINSYS_INVEN_X = 6,
		VNGAINSYS_INVEN_Y = 10,
		nMAX_ONE_VIEW_SLOT = 10
	};

	enum
	{
		ITEM_SLOT0 = NO_ID + 1,
		ITEM_SLOT1,
		ITEM_SLOT2,
		ITEM_SLOT3,
		ITEM_SLOT4,
		ITEM_SLOT5,
		ITEM_SLOT6,
		ITEM_SLOT7,
		ITEM_SLOT8,
		ITEM_SLOT9,
		ITEM_SLOT_END = ITEM_SLOT0 + VNGAINSYS_INVEN_Y
	};

public:
	CVNGainInvenPage(void);
	~CVNGainInvenPage(void);


public:
	void CreateSubControl ();

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	void	SetItemIndex ( int nPosX, int nPosY );
	void	GetItemIndex ( int* pnPosX, int* pnPosY );
	void	UpdateBLOCK ();

public:
	void	LoadItemPage ( GLInventory &ItemData );
	void	LoadItem ( SINVENITEM& ref_InvenItem );

	SINVENITEM&	GetItem ( int nPosX, int nPosY );

public:
	void	UnLoadItemPage ();
	void	UnLoadItem ( int nPosX, int nPosY );

private:
	void	ResetAllItemSlotRender ( int nTotal );
	void	SetItemSlotRender ( int nStartIndex, int nTotal );

private:
	CItemSlot*	CreateItemSlot ( CString strKeyword, UIGUID ControlID );
	CUIControl*	CreateControl ( const char* szControl );

private:
	CItemSlot*	m_pItemSlotArray[VNGAINSYS_INVEN_Y];
	CUIControl*	m_pItemSlotArrayDummy[nMAX_ONE_VIEW_SLOT];

//	CBasicScrollBarEx*	m_pScrollBar;

private:
	int		m_nPosX;
	int		m_nPosY;
	int		m_nCurPos;
};

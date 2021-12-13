#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"
#include "GLInventory.h"

class CItemSlotEx;
class CBasicScrollBarEx;

class CItemMixInvenPage : public CUIGroup	// »ﬁ¡ˆ≈Î
{
private:
	static const INT	nSTARTLINE;
	static const INT	nOUTOFRANGE;

protected:
	enum
	{
		nONE_VIEW_SLOT_DEFAULT = 4,
		nMAX_ONE_VIEW_SLOT = 10
	};

	enum
	{
		ITEM_SLOT0 = NO_ID + 1,
		ITEM_SLOT1,
		ITEM_SLOT_END = ITEM_SLOT0 + EM_INVENSIZE_Y
	};

private:
	CItemSlotEx*		m_pItemSlotArray[ EM_INVENSIZE_Y ];
	CUIControl*			m_pItemSlotArrayDummy[ nMAX_ONE_VIEW_SLOT ];
	CBasicScrollBarEx*	m_pScrollBar;
	int					m_nPosX;
	int					m_nPosY;
	int					m_nCurPos;
	int					m_nONE_VIEW_SLOT;

public:
	CItemMixInvenPage();
	virtual	~CItemMixInvenPage();

public:
	VOID CreateSubControl();

protected:
	CItemSlotEx* CreateItemSlot( CString strKeyword, UIGUID ControlID );
	CUIControl* CreateControl( const TCHAR* szControl );

public:
	virtual VOID Update( INT x, INT y, BYTE LB, BYTE MB, BYTE RB, INT nScroll, FLOAT fElapsedTime, BOOL bFirstControl );
	virtual	VOID TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

protected:
	VOID ResetAllItemSlotRender( INT nTotal );
	VOID SetItemSlotRender( INT nStartIndex, INT nTotal );

public:
	VOID SetItemIndex( INT nPosX, INT nPosY );
	VOID GetItemIndex( INT* pnPosX, INT* pnPosY );

public:
	VOID LoadItemPage( GLInventory &ItemData );
	VOID LoadItem( SINVENITEM& ref_InvenItem );

public:
	VOID UnLoadItemPage();
	VOID UnLoadItem( INT nPosX, INT nPosY );

public:
	VOID ResetAllFlipItem();
	VOID UpdateFlipItem( GLInventory& ref_Inventory );
	VOID UpdateFlipItem( SINVEN_POS& ref_RebuildItem );

public:
	VOID UpdateBLOCK();
	SINVENITEM&	GetItem( INT nPosX, INT nPosY );

public:
	VOID SetOneViewSlot( const INT& nMAX_ONE_VIEW );
};
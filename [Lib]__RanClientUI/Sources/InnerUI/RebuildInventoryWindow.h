#pragma	once

#include "UIWindowEx.h"

class CRebuildInventoryPage;

//--------------------------------------------------------------------
// RebuildInventoryWindow : √÷¡ÿ«ı
//--------------------------------------------------------------------
class CRebuildInventoryWindow : public CUIWindowEx	// ITEMREBUILD_MARK
{
protected:
	enum
	{
		REBUILDINVENTORY_MONEY_BUTTON = ET_CONTROL_NEXT,
		REBUILDINVENTORY_PLUS_MONEY_BUTTON,
		REBUILDINVENTORY_MINUS_MONEY_BUTTON,
		REBUILDINVENTORY_PAGE
	};

private:
	CRebuildInventoryPage*	m_pPage;
	CBasicTextBox*			m_pMoneyTextBox;
	INT						m_nONE_VIEW_SLOT;

public:
	CRebuildInventoryWindow();
	virtual	~CRebuildInventoryWindow();

public:
	VOID CreateSubControl();

public:
	virtual	VOID Update( INT x, INT y, BYTE LB, BYTE MB, BYTE RB, INT nScroll, FLOAT fElapsedTime, BOOL bFirstControl );
	virtual	VOID TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	VOID SetOneViewSlot( const INT& nONE_VIEW_SLOT );
};
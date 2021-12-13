#pragma	once

#include "UIWindowEx.h"

class CItemMixInvenPage;

//--------------------------------------------------------------------
// CItemMixInvenWindow : 
//--------------------------------------------------------------------
class CItemMixInvenWindow : public CUIWindowEx	// »ﬁ¡ˆ≈Î
{
protected:
	enum
	{
		ITEMMIXINVEN_MONEY_BUTTON = ET_CONTROL_NEXT,
		ITEMMIXINVEN_PAGE
	};

private:
	CItemMixInvenPage*	m_pPage;
	CBasicTextBox*			m_pMoneyTextBox;
	INT						m_nONE_VIEW_SLOT;

public:
	CItemMixInvenWindow();
	virtual	~CItemMixInvenWindow();

public:
	VOID CreateSubControl();

public:
	virtual	VOID Update( INT x, INT y, BYTE LB, BYTE MB, BYTE RB, INT nScroll, FLOAT fElapsedTime, BOOL bFirstControl );
	virtual	VOID TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	VOID SetOneViewSlot( const INT& nONE_VIEW_SLOT );
};
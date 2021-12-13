#pragma	once

#include "UIWindowEx.h"

class CItemImage;
class CBasicTextButton;

//--------------------------------------------------------------------
// CItemGarbage : 
//--------------------------------------------------------------------
class CItemGarbage : public CUIWindowEx	// »ﬁ¡ˆ≈Î
{
protected:
	enum
	{
		ITEM_GARBAGE_ITEM_SLOT = ET_CONTROL_NEXT,
		ITEM_GARBAGE_OK_BUTTON,
		ITEM_GARBAGE_CANCEL_BUTTON
	};

protected:
	CItemImage*			m_pItemImage;
	CBasicTextButton*	m_pOkButton;

public:
	CItemGarbage();
	virtual	~CItemGarbage();

public:
	VOID CreateSubControl();

public:
	virtual	VOID Update( INT x, INT y, BYTE LB, BYTE MB, BYTE RB, INT nScroll, FLOAT fElapsedTime, BOOL bFirstControl );
	virtual	VOID TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
};
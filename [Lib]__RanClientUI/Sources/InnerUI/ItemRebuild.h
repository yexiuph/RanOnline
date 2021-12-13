#pragma	once

#include "UIWindowEx.h"

class CItemImage;
class CBasicTextButton;

//--------------------------------------------------------------------
// ItemRebuild : √÷¡ÿ«ı
//--------------------------------------------------------------------
class CItemRebuild : public CUIWindowEx	// ITEMREBUILD_MARK
{
protected:
	enum
	{
		ITEM_REBUILD_ITEM_SLOT = ET_CONTROL_NEXT,
		ITEM_REBUILD_COAST_TEXT_STATIC,
		ITEM_REBUILD_COAST_TEXT,
		ITEM_REBUILD_MONEY_BACK,
		ITEM_REBUILD_MONEY_IMAGE,
		ITEM_REBUILD_MONEY_TEXT,
		ITEM_REBUILD_OK_BUTTON,
		ITEM_REBUILD_CANCEL_BUTTON
	};

protected:
	CItemImage*		m_pItemImage;
	CBasicTextBox*	m_pCostTextBoxStatic;
	CBasicTextBox*	m_pCostTextBox;
	CBasicTextBox*	m_pMoneyTextBox;
	CBasicTextButton* m_pOkButton;

public:
	CItemRebuild();
	virtual	~CItemRebuild();

public:
	VOID CreateSubControl();

public:
	virtual	VOID Update( INT x, INT y, BYTE LB, BYTE MB, BYTE RB, INT nScroll, FLOAT fElapsedTime, BOOL bFirstControl );
	virtual	VOID TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
};
#pragma once

#include "UIWindowEx.h"

class	CVNGainInvenPage;
class	CBasicTextBox;
class	CBasicTextButton;

class	CVNGainSysInventory : public CUIWindowEx	
{
protected:
	enum
	{
		VNGAINSYSINVENTORY_MONEY_BUTTON = ET_CONTROL_NEXT,
		VNGAINSYS_ITEMRESET_BUTTON,
		VNGAININVEN_PAGE,
	};

	CVNGainInvenPage*	m_pPage;

public:
	CVNGainSysInventory ();
	virtual	~CVNGainSysInventory ();

private:
	CBasicTextBox*		m_pMoneyTextBox;
	CBasicTextBox*		m_pExpNumberBox;
	CBasicTextBox*		m_pItemNumberBox;
	CBasicTextButton*	m_pItemResetButton;

	WORD	m_wSplitItemPosX;
	WORD	m_wSplitItemPosY;
	BOOL	m_bSplitItem;

private:
	void	SetMoney ( LONGLONG Money );
	void	SetExp ( LONGLONG Exp );
	void	SetItemCnt ( int ItemCnt );

public:
	void	CreateSubControl ();

	void	GetSplitPos ( WORD* pwPosX, WORD* pwPosY );
	void	SetSplitPos ( WORD wPosX, WORD wPosY );

private:
	CBasicTextBox*	CreateTextBox ( char* szKeyword, CD3DFontPar* pFont, int nAlign );

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	void	InitVNGainSys ();
	void	ClearVNGainSys();

	CBasicTextButton*	CreateTextButton ( char* szButton, UIGUID ControlID, char* szText );
};
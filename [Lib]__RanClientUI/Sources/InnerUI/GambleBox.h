#pragma once

#include "./UIWindowEx.h"
#include "../[Lib]__RanClient/Sources/G-Logic/Data/GLItem.h"

class	CBasicProgressBar;
class	CBasicTextBox;
class	CUIEditBox;

class CGambleBox : public CUIWindowEx
{
private:
static	const int nLIMIT_CHAR;

protected:
	enum
	{
		GAMBLE_OK_BUTTON = ET_CONTROL_NEXT,
		GAMBLE_CANCLE_BUTTON,
		GAMBLE_REINPUT_BUTTON,
		GAMBLE_BET_EDIT,
		GAMBLE_GET_EDIT,

	};

public:
	CGambleBox(void);
	virtual ~CGambleBox(void);

public:
	void	CreateSubControl ();
	CUIControl*	CreateControl( const char* szControl );

public:
	virtual	void SetVisibleSingle ( BOOL bVisible );
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	
	BOOL UpdateProgressBar( CBasicProgressBar * pBar, float x );
	void TranslateEditBoxMsg( CUIEditBox * pEditBox, DWORD dwMsg );
	void ClearEditBox();
	const ULONGLONG GetBetMoney();
private:
	CBasicTextBox*				m_pInfoTextBox;
	CBasicTextBox*				m_pBetMoneyTextBox;
	CBasicTextBox*				m_pGetMoneyTextBox;
	CBasicProgressBar*			m_pBetTimeProgressBar;
	CUIEditBox*					m_pBetEditBox;
	CUIEditBox*					m_pGetEditBox;

	float m_TotElapsedTime;
};
#pragma once

#include "UIWindowEx.h"
#include "GLItem.h"

class	CBasicProgressBar;
class	CBasicTextBox;
class	CUIEditBox;

class CGambleAgainBox : public CUIWindowEx
{
private:
static	const int nLIMIT_CHAR;
static	const int nLIMIT_TIME;

protected:
	enum
	{
		GAMBLE_AGAIN_OK_BUTTON = ET_CONTROL_NEXT,
		GAMBLE_AGAIN_CANCLE_BUTTON,
		GAMBLE_AGAIN_GET_EDIT,
	};

public:
	CGambleAgainBox(void);
	virtual ~CGambleAgainBox(void);

public:
	void	CreateSubControl ();
	CUIControl*	CreateControl( const char* szControl );

public:
	virtual	void SetVisibleSingle ( BOOL bVisible );
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	
	BOOL UpdateProgressBar( CBasicProgressBar * pBar, float x );
	void ClearEditBox();
	void SetMoney( WORD wRound, ULONGLONG GetMoney );
private:
	CBasicTextBox*				m_pInfoTextBox;
	CBasicTextBox*				m_pBetMoneyTextBox;
	CBasicProgressBar*			m_pBetTimeProgressBar;
	CUIEditBox*					m_pBetEditBox;


	float m_TotElapsedTime;
};
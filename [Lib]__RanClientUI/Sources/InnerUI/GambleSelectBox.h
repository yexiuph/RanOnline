#pragma once

#include "UIWindowEx.h"
#include "GLItem.h"

class	CBasicProgressBar;
class	CBasicTextBox;

class CGambleSelectBox : public CUIWindowEx
{
private:
static	const int nLIMIT_TIME;

protected:
	enum
	{
		GAMBLE_SELECT_ODD_BUTTON = ET_CONTROL_NEXT,
		GAMBLE_SELECT_EVEN_BUTTON,
	};

public:
	CGambleSelectBox(void);
	virtual ~CGambleSelectBox(void);

public:
	void	CreateSubControl ();
	CUIControl*	CreateControl( const char* szControl );
	void CreateKeyButton( const char * szControlKey, const char * szFlipKey, UIGUID nID );

public:
	virtual	void SetVisibleSingle ( BOOL bVisible );
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	
	BOOL UpdateProgressBar( CBasicProgressBar * pBar, float x );
	
private:
	CBasicTextBox*				m_pInfoTextBox;
	CBasicProgressBar*			m_pBetTimeProgressBar;

	float m_TotElapsedTime;
};
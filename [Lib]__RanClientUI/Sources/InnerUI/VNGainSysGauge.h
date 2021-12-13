#pragma once
#include "../[Lib]__EngineUI/Sources/UIGroup.h"
#include "../[Lib]__EngineUI/Sources/BasicButton.h"

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
//const DWORD UIMSG_BASICINFOVIEW_LBDUP = UIMSG_USER1;
////////////////////////////////////////////////////////////////////

class	CBasicProgressBar;
class	CBasicTextButton;

class CVNGainSysGauge : public CUIGroup
{
public:
	CVNGainSysGauge ();
	virtual	~CVNGainSysGauge ();

public:
	void	CreateSubControl ();

public:

	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

	CBasicTextBox*	CreateTextBox ( char* szKeyword, CD3DFontPar* pFont, int nAlign );

public:
	void	SetGauge ( DWORD dwNow, DWORD dwMax );

private:
	CBasicProgressBar*	m_pGauge;

private:
	DWORD	m_dwOldValue;

	CBasicButton*	m_pVNInvenButton;

};
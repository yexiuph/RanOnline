#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"
#include "GLCharDefine.h"
#include "GLParty.h"
#include "../[Lib]__EngineUI/Sources/BasicTextBox.h"
#include "../[Lib]__EngineUI/Sources/BasicProgressBar.h"

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_MOUSEIN_RB_EVENT = UIMSG_USER1;
////////////////////////////////////////////////////////////////////

class	CBasicTextBox;
class	CBasicProgressBar;

class	CMiniPartySlot : public CUIGroup
{
protected:
	enum
	{
		HP_BAR = NO_ID + 1,
	};

	CBasicTextBox*		m_pPlayerName;
	CBasicProgressBar*	m_pHP;
	CUIControl*			m_pNumber[MAXPARTY];
	//CUIControl*		m_pClass[GLCI_NUM];

public:
	CMiniPartySlot ();
	virtual	~CMiniPartySlot ();

public:
	void	CreateSubControl ();

public:
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	void	SetPlayerName ( CString strName )	{ m_pPlayerName->SetOneLineText( strName ); }
	void	SetHP ( float fPercent )			{ m_pHP->SetPercent( fPercent ); }
	void	SetSameMap ( BOOL bSameMap )		{ m_pHP->SetVisibleSingle( bSameMap ); }
	//void	SetClass ( int nClassType );
	void	SetNumber ( int nSlotNumber );

protected:
	CUIControl*		CreateControl ( const char* szControl );
	CBasicTextBox*	CreateStaticControl ( char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR D3DCOLOR, int nAlign );
};
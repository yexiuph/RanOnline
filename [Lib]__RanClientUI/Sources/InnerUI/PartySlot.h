//	파티 윈도우
//
//	최초 작성자 : 성기엽
//	이후 수정자 : 
//	로그
//		[2003.11.27]
//			@ 작성
//

#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"
#include "GLCharDefine.h"

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_MOUSEIN_LBUP_EVENT = UIMSG_USER1;
const DWORD UIMSG_MOUSEIN_RBUP_EVENT = UIMSG_USER2;
const DWORD UIMSG_MOUSEIN_LBUP_AUTH = UIMSG_USER3;
////////////////////////////////////////////////////////////////////

class	CBasicTextBox;
//class	CBasicButton;
class	CBasicTextButton;
class	CBasicProgressBar;

class	CPartySlot : public CUIGroup
{
protected:
	enum
	{
		PARTY_QUIT_BUTTON = NO_ID + 1,
		PARTY_AUTH_BUTTON,
		PARTY_HP_BAR,
	};

public:
	CPartySlot ();
	virtual	~CPartySlot ();

public:
	void	CreateSubControl ();

public:
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	void	SetPlayerName ( CString strName );
	void	SetPlayerPos ( CString strPos );
	void	SetHP ( float fPercent );
	void	SetMapName ( CString strMapName );
	void	SetClass ( int nClassType );
	void	SetVisibleQuitButton ( BOOL bVisible );
	void	SetVisibleAuthButton( BOOL bVisible );

public:
	void	SetMaster ( BOOL bMaster );

public:
	void	SetSameMap ( BOOL bSameMap );

protected:
	CUIControl*		CreateControl ( const char* szControl );
//	CBasicButton*	CreateFlipButton ( char* szButton, char* szButtonFlip, UIGUID ControlID );
	CBasicTextBox*	CreateStaticControl ( char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR D3DCOLOR, int nAlign );
	CBasicTextButton*	 CreateTextButton ( char* szButton, UIGUID ControlID, char* szText );

private:
	CBasicTextBox*	m_pPlayerName;
	CBasicTextBox*	m_pPosText;
//	CBasicButton*	m_pQuitButton;
	CBasicTextButton*	m_pQuitButton;
	CBasicTextButton*	m_pAuthorityButton;

	CBasicProgressBar*	m_pHP;
	CUIControl*			m_pHPLineBack;
	
	CBasicTextBox*		m_pMapText;
	CUIControl*			m_pMapLineBack;
	
	CUIControl*			m_pClass[GLCI_NUM_EX];

	BOOL	m_bMaster;
};
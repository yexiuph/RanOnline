#pragma	once

#include "../[Lib]__EngineUI/Sources/UIWindow.h"
#include "GLCharClient.h"

class	CBasicButton;
class	CBasicTextBox;
class	CBasicVarTextBox;
class	GLCLUB;
//class	GLCharClient;
//typedef	GLCharClient* PGLCHARCLIENT;

class	CPtoPWindow : public CUIWindow
{
private:
	enum
	{
		PTOPMENU_TRADE = ET_CONTROL_NEXT,
		PTOPMENU_WHISPER,
		PTOPMENU_PARTY,
		PTOPMENU_CONFLICT,
		PTOPMENU_FRIEND,
		PTOPMENU_GUILD,
		PTOPMENU_UNION,
		PTOPMENU_GUILD_BATTLE,
		PTOPMENU_UNION_BATTLE
	};

public:
	CPtoPWindow ();
	virtual	~CPtoPWindow ();

public:
	void	CreateSubControl();
	void	SetPtoPMenu( PGLCHARCLIENT pCharClient, DWORD dwGaeaID, GLCLUB & sClub );
	DWORD	GetPlayerID()	{ return m_dwGaeaID; }

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

protected:
	void	ResizeControl ( char* szWindowKeyword );

private:
	CBasicButton*	CreateFlipButton ( char* szButton, char* szButtonFlip, char* szButtonOver, UIGUID ControlID );
	CBasicTextBox*	CreateStaticControl ( char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR D3DCOLOR, int nAlign );

	void	AddInfoNoSplit ( CString strText, D3DCOLOR dwColor );
	void	RemoveAllInfo ();

private:
	DWORD	m_dwGaeaID;
	CString	m_szPlayerName;

	BOOL	m_bSetThisFrame;
	int		m_nOverMenu;

	CBasicTextBox*	m_pNameBox;
	CBasicVarTextBox*	m_pInfo;

	CBasicButton*	m_pGuildButton;
	CBasicButton*	m_pUnionButton;
	CBasicButton*	m_pGuildBattleButton;
	CBasicButton*	m_pUnionBattleButton;
};
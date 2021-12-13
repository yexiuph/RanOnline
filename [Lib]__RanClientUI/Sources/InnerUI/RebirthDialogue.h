#pragma	once

#include "../[Lib]__EngineUI/Sources/UIWindow.h"

class	CBasicTextBox;
class	CBasicTextButton;

//	ISREVIVE ()

class CRebirthDialogue : public CUIWindow
{
	static const float fDEFAULT_TIME_LEFT;
	static const float fDEFAULT_SELECT_TIME_LEFT;

private:
	enum
	{
		REBIRTH_CHECK_POINT = ET_CONTROL_NEXT,
		REBIRTH_CURRENT_POSITION,
		REBIRTH_RECOVERY_EXP
	};	

public:
	CRebirthDialogue ();
	virtual	~CRebirthDialogue ();

public:
	void	CreateSubControl ();
	CBasicTextButton* CreateTextButton ( char* szButton, UIGUID ControlID , char* szText );

public:
	void	SET_ITEMREBIRTH ( const bool& bItemRebirth, bool bAutoRebirth, bool bCantUseRebirth );

private:
	void	ShowButton ();
	void	HideButton ();	

	void	DoACTION ()							{ m_bDO_ACTION = true; }

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	void	SetCallerID( UIGUID ControlID )		{ m_CallerID = ControlID; }
	UIGUID	GetCallerID()						{ return m_CallerID; }
	bool	IsRecoveryAction()					{ return ( m_nACTION == REBIRTH_RECOVERY_EXP ); }

private:
	CUIControl*	CreateDummyControl ( char* szKeyword );

private:
	CBasicTextBox*		m_pTextBox;
	CBasicTextButton*	m_pCheckPointButton;
	CBasicTextButton*	m_pCurrentPositionButton;
	CBasicTextButton*	m_pRecoveryExpButton;

private:
	CUIControl*		m_pDummy_1BUTTON;
	CUIControl*		m_pDummy_2BUTTON_L;
	CUIControl*		m_pDummy_2BUTTON_R;

private:
	float	m_fTIME_LEFT;
	float	m_fSelTIME_LEFT;
	bool	m_bDO_ACTION;
	int		m_nACTION;

	bool	m_bDONE_ACTION;
	bool	m_bAutoRebirth;

	UIGUID	m_CallerID;
};
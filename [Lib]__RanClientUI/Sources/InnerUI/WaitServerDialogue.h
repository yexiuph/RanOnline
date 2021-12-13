#pragma	once

#include "../[Lib]__EngineUI/Sources/UIModal.h"

class	CBasicTextBox;
class	CBasicTextButton;

enum
{
	WAITSERVER_CLOSEGAME,
	WAITSERVER_TOLOBY,
	WAITSERVER_TOFRIEND,
};

const DWORD UIMSG_MOUSEIN_OKBUTTON = UIMSG_USER1;

const float fDEFAULT_TIME_LEFT = 5.0f;

class CWaitServerDialogue : public CUIModal
{
	enum
	{
		WAITSERVER_OKBUTTON = NO_ID + 1,
	};	

	int		m_nACTION;
	float	m_fTIME_LEFT;

	CBasicTextBox*		m_pTextBox;
	CBasicTextButton*	m_pMoveOKButton;

	CString m_strMessage;

public:
	CWaitServerDialogue ();
	virtual	~CWaitServerDialogue ();

public:
	void	CreateSubControl ();
	CBasicTextButton* CreateTextButton ( char* szButton, UIGUID ControlID , char* szText );

public:
	void	SetDisplayMessage ( const CString& strDisplay, const int nAction, const float fTimer );	
	void	DoACTION_PLAY ();
	int		GetActionTYPE ()	{ return m_nACTION; }

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
};
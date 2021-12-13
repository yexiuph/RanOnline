#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_BASICINFOVIEW_LBDUP = UIMSG_USER1;
////////////////////////////////////////////////////////////////////

class	CBasicProgressBar;
class	CBasicTextBox;

class CBasicInfoView : public CUIGroup
{
private:
	enum
	{
		HP_TEXT = NO_ID + 1,
		MP_TEXT,
		SP_TEXT,
		EXP_TEXT
	};

public:
	CBasicInfoView ();
	virtual	~CBasicInfoView ();

public:
	void	CreateSubControl ();

public:
	void	CheckMouseState ();

	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

private:
	void	SetHP ( WORD wNOW, WORD wMAX );
	void	SetMP ( WORD wNOW, WORD wMAX );
	void	SetSP ( WORD wNOW, WORD wMAX );
	void	SetEXP( LONGLONG lNOW, LONGLONG lMAX );

private:
	CBasicTextBox*		m_pHPText;
	CBasicTextBox*		m_pMPText;
	CBasicTextBox*		m_pSPText;
	CBasicTextBox*		m_pEXPText;

private:
	CBasicProgressBar*	m_pHP;
	CBasicProgressBar*	m_pMP;
	CBasicProgressBar*	m_pSP;
	CBasicProgressBar*	m_pEXP;

private:
	WORD	m_wHPBACK;
	WORD	m_wMPBACK;
	WORD	m_wSPBACK;
	LONGLONG	m_lEXPBACK;

private:
	BOOL		m_bFirstGap;
	D3DXVECTOR2	m_vGap;

	int			m_PosX;
	int			m_PosY;

	bool		m_bCHECK_MOUSE_STATE;
};
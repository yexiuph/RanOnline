//	BasicButton
//
//	최초 작성자 : 서보건
//	이후 수정자 : 성기엽
//	로그
//		[2003.11.7] - 오픈 인터페이스에 맞도록 모든 메시지 변경
//		[2003.9.29] - CUIControl --> CUIGroup으로 변경
#pragma once

#include "UIGroup.h"

class CBasicButton : public CUIGroup
{
public:
static	const	float	fBUTTON_CLICK;

public:
	enum
	{
		NOT_READY	= -1,
		CLICK_FLIP	= 0,
		MOUSEIN_FLIP= 1,
		KEEP_FLIP	= 2,
		RADIO_FLIP	= 3,
	};

public:
	CBasicButton();
	virtual ~CBasicButton();

protected:
	BOOL	m_bDown;
	BOOL	m_bMouseIn;

	union
	{
		struct { BOOL m_bDowned; };
		struct { BOOL m_bInside; };
		struct { BOOL m_bOn;	 };
		struct { BOOL m_bFlip;	 };
	};
	BOOL	m_bOnBack;

protected:
	BOOL	m_bUseDynamic;

public:
	void SetUseDynamic( BOOL bUseDynamic )				{ m_bUseDynamic = bUseDynamic; }
	BOOL IsUseDynamic()									{ return m_bUseDynamic; }

protected:
	BOOL m_bUseGlobalAction;

public:
	void SetUseGlobalAction( BOOL bUseGlobalAction )	{ m_bUseGlobalAction = bUseGlobalAction; }
	BOOL IsUseGlobalAction()							{ return m_bUseGlobalAction; }

protected:
	virtual	void	SetFlipPosition ();
	virtual	void	ResetFlipPosition ();

protected:	
	WORD	m_wFlipMode;

private:
	CUIControl*	m_pFlipControl;
	CUIControl*	m_pMouseOver;

	BYTE		m_KeyCode;
	BYTE		m_KeyCodeEx;

public:
	BOOL IsFlip()						{ return m_bFlip; }
	virtual	void SetFlip( BOOL bFlip );

	BOOL	IsVIsibleMouseOver ();
	void	SetVisibleMouseOver ( BOOL bVisible );

	void SetShortcutKey ( BYTE nKeyCode, BYTE nKeyCodeEx = NULL )
	{ m_KeyCode = nKeyCode; m_KeyCodeEx = nKeyCodeEx; }

	void Reset();

	void	CreateFlip ( const char* keyvalue, WORD FlipType = CLICK_FLIP );
	void	CreateMouseOver ( char* KeyValue );

	void	SetFlip();
	void	DoFlipUpdate ();
	
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

private:
	void	TranslateMouseMessage ( DWORD dwMsg );
};

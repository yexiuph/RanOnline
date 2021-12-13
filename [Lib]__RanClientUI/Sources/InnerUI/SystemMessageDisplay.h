//	채팅 박스
//
//	최초 작성자 : 성기엽
//	이후 수정자 : 
//	로그
//		[2003.11.25]
//			@ 최초 작성
//

#pragma once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"

class	CBasicTextBox;

class	CSystemMessageDisplay : public CUIGroup
{
private:
static	float	fLIFETIME;
static	float	fFADE_START_TIME;
static	float	fMINIMUM_ALPHA;
static	const	float	fMAX_ALPHA;
static	const	float	fTIME_FACTOR;

private:
static	void	SetLifeTime ( const float fTime );
static	void	SetFadeStartTime ( const float fTime );
static	void	SetMinimumAlpha ( const float fAlpha );

private:
	enum
	{
		nLIMITLINE = 3
	};

public:
	CSystemMessageDisplay ();
	virtual	~CSystemMessageDisplay ();

public:
	void	CreateSubControl ();	

public:
	void	AddText ( CString strText, D3DCOLOR dwColor );

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	
private:
	CBasicTextBox*		m_pMessageBox;
};
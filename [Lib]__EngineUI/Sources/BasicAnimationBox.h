//	BasicAnimationBox
//
//	최초 작성자 : 김재우
#pragma once

#include "UIGroup.h"
#include <vector>

using namespace std;

class CBasicAnimationBox : public CUIGroup
{
public:
	enum
	{
		NORMAL_ANI		= 0,
		PINGPONG_ANI	= 1,
		BACK_ANI		= 2,
	};

public:
	CBasicAnimationBox();
	virtual ~CBasicAnimationBox();

public:
	void	Reset();
	void	InitAnimateFrame();
	void	CreateAni ( const char* keyvalue, DWORD AniType = NORMAL_ANI );
	void	AddAni (const char* keyvalue);
	void	SetBackImage( const char* keyvalue );

	void	SetAniTime ( float fTime );
	float	GetAniTime ();

	void	SetEndTime ( float fTime );
	float	GetEndTime ();

	void	SetAniType ( DWORD dwAni);
	DWORD	GetAniType ();

	void	SetPause ( BOOL bPause);
	BOOL	GetPause () {return m_bPause;}
	void	SetAniDiffuse( D3DXCOLOR d3dColor );

	void	SetLoop ( bool bLoop) { m_bLoop = bLoop; }
	bool	GetLoop() {return m_bLoop;}

	bool	GetEndAnimate() { return m_bEndAnimate; }



public:
	virtual	void SetVisibleSingle ( BOOL bVisible );
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

private:
	void	TranslateMouseMessage ( DWORD dwMsg );


protected:
	vector<CUIControl*>		m_pAniControl;
	CUIControl*				m_pBackImage;

	BOOL			m_bPause;
	BOOL			m_bMoveDir;
	DWORD			m_dwAniMode;
	DWORD			m_dwRenderPic;

	float			m_fAniTime;
	float			m_fEndTime;
	float			m_TotElapsedTime;
	float			m_fLastRenderTime;
	bool			m_bLoop;
	bool			m_bEndAnimate;

	D3DXCOLOR	m_d3dColor;

};

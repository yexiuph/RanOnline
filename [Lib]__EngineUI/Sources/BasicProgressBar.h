//	BasicProgressBar 클래스
//
//	최초 작성자 : 성기엽
//	이후 수정자 :
//	로그
//		[2003.11.19]
//			@ 최초제작

#pragma	once

#include "UIGroup.h"

class	CBasicProgressBar : public CUIGroup
{
public:
	enum
	{
		HORIZONTAL,
		VERTICAL
	};

private:
	static	const	float	fZERO_PERCENT;
	static	const	float	fHUNDRED_PERCENT;

private:
	int		m_nType;
	float	m_fPercent;

	CUIControl*	m_pOverImage;
	CUIControl*	m_pOverImageDummy;

public:
	CBasicProgressBar ();
	virtual	~CBasicProgressBar ();

public:
	void	SetType ( int nType )			{ m_nType = nType; }
	int		GetType ()						{ return m_nType; }

public:	
	void	CreateOverImage ( char* szOverImage );

public:
	void	SetOverImageDiffuse ( DWORD dwDiffuse );
	DWORD	GetOverImageDiffuse ();
	void	SetOverImageUseRender ( BOOL bUseRender );
	BOOL	IsOverImageUseRender ();

	void	SetAlignFlag ( WORD wFlag );

public:
	virtual void	SetGlobalPos ( const UIRECT& rcPos );
	virtual void	SetGlobalPos ( const D3DXVECTOR2& vPos );
	virtual	void	AlignSubControl ( const UIRECT& rcParentOldPos, const UIRECT& rcParentNewPos );

public:
	virtual	void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );		

protected:
	void	UpdateProgress ();

public:
	void	SetPercent ( float fPercent );
	float	GetPercent ( void );
};
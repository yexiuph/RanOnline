//	스킬 이동
//
//	최초 작성자 : 성기엽
//	이후 수정자 : 
//	로그
//		[2003.12.8]
//			@ 작성
//

#pragma	once

#include "../[Lib]__EngineUI/Sources/UIGroup.h"

enum
{
	UI_UNDERATTACK = 0,
	UI_ATTACK = 1
};

class CDamageDisplay : public CUIGroup
{
private:
static	const	float	fDEFAULT_SCALE;
static	const	float	fMOVING_DELAY;
static	const	float	fDEFAULT_VELOCITY;
static	const	float	fDEFAULT_ACCEL;
static	const	float	fDEFAULT_FADETIME;

private:
	enum
	{
		nMAXNUMBER = 10,
		nMAXDIGIT = 5,
	};

public:
	CDamageDisplay ();
	virtual	~CDamageDisplay ();

public:
	void	CreateSubControl ();

public:
	virtual	HRESULT	InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

public:	
	void	SetDamage ( int nDamage, DWORD dwDamageFlag, BOOL bAttack );
	
	void	SetAccel ( float fAccel )				{ m_fAccel = fAccel; }
	void	SetVelocity ( float fVelocity )			{ m_fVelocity = fVelocity; }

	float	GetAccel ()								{ return m_fAccel; }
	float	GetVelocity ()							{ return m_fVelocity; }

public:
	void	SetScale ( float fScale )				{ m_fScale = fScale; }
	
private:
	BOOL	SetFadeOut ();

private:
	HRESULT	ResetRenderData ();
	void	UpdatePos ( float fElapsedTime );

private:
	CUIControl*		CreateControl ( const char* szControl );
	BOOL	CloneControl ( CUIControl* const pDst, CUIControl* const pSrc );

private:
	CUIControl*		m_pBLUE[nMAXNUMBER];
	CUIControl*		m_pRED[nMAXNUMBER];
	CUIControl*		m_pCRITICAL;
	CUIControl*		m_pMISS;
	CUIControl*		m_pDUMMY;
	CUIControl*		m_pCRUSHING_BLOW;

private:
	int			m_nNumberLength;
	CUIControl*	m_pMISS_RENDER;
	CUIControl*	m_pCRITICAL_RENDER;
	CUIControl* m_pCRUSHING_BLOW_RENDER;
	CUIControl*	m_pCombineArray[nMAXDIGIT];

	float	m_fScale;

private:	
	float	m_fCurVelocity;

	float	m_fVelocity;
	float	m_fAccel;
	float	m_fLength;

private:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;
};
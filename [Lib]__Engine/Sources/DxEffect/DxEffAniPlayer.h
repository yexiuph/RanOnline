#pragma once

#include <list>

class DxEffAni;
class DxSkinChar;
struct SKINEFFDATA;

//	Note : "class DxEffSingleGroup"인스턴스들을 (즉, 지금 동작하고 있는 "이펙트 그룹"개체들을 같이 모아서 관리하고
//			이를 동작시키며 렌더링 한다. ( Update & Render ) 또한 이들이 소멸시점인지를 체크한후 소멸되게 한다.
//
class DxEffAniPlayer
{
protected:
	typedef std::list<DxEffAni*>		EFFLIST;
	typedef EFFLIST::iterator			EFFLIST_ITER;

protected:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;

	float				m_fTimer;
	EFFLIST				m_listAni;		// Ani Key에 따른 효과의 생성.
	EFFLIST				m_listOneSet;	// 1번만 효과를 생성.

protected:
	void	CreateEff ( DxEffAniData* pEff, PSANIMCONTAINER pAnimContainer );

public:
	void	CheckCreateEff( PSANIMCONTAINER pAnimContainer );										// Note : 1번 설정후 다음부턴 안함.
	void	CheckCreateEff( PSANIMCONTAINER pAnimContainer, int dwPrevFrame, DWORD dwCurKeyTime );	// Note : 매번 Ani Key를 본 후 Eff 셋팅.
	void	ResetEff( PSANIMCONTAINER pAnimContainer );

public:
	DWORD	GetAmount ()				{ return DWORD(m_listAni.size()); }
	
	void	SetTime ( float fTimer )	{ m_fTimer = fTimer; }
	float	GetTime ()					{ return m_fTimer; }

	HRESULT RemoveAllEff ();
	HRESULT DeleteEff ( const TCHAR* szFile );

public:
	HRESULT FrameMove ( PSANIMCONTAINER pAnimContainer, float fTime, float fElapsedTime, SKINEFFDATA& sSKINEFFDATA );
	HRESULT	Render ( const LPDIRECT3DDEVICEQ pd3dDevice, DxSkinChar* pSkinChar, const D3DXMATRIX& matCurPos );

public:
	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RestoreDeviceObjects ();

	HRESULT InvalidateDeviceObjects ();
	HRESULT DeleteDeviceObjects ();

public:
	DxEffAniPlayer (void);

public:
	~DxEffAniPlayer (void);
};

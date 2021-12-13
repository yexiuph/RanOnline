#pragma once

#include "DxEffAni.h"
#include "DxMethods.h"

#include <list>

class DxSkinChar;

struct EFF_PROPGROUP;
class DxEffSingleGroup;

struct EFFANI_PROPERTY_GHOSTING : public EFFANI_PROPERTY
{
	DWORD			m_dwFlags;
	DWORD			m_dwFrame_1;		// 시작 프레임
	DWORD			m_dwFrame_2;		// 끝 프레임
	DWORD			m_dwGhostingNum;		// 고스팅 하는 갯수
	DWORD			m_dwAlphaMax;
	DWORD			m_dwUnit;
	D3DCOLOR		m_dwColor;

	EFFANI_PROPERTY_GHOSTING () :
		m_dwFlags(0L),
		m_dwFrame_1(0),
		m_dwFrame_2(0),
		m_dwGhostingNum(3),
		m_dwAlphaMax(130),
		m_dwUnit(3),
		m_dwColor(0x00ffffff)
	{
	}
};

class DxEffAniData_Ghosting : public DxEffAniData
{
public:
	static DWORD		VERSION;
	static DWORD		TYPEID;
	static char			NAME[64];

public:
	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual DWORD GetFlag ()		{ return NULL; }
	virtual const char* GetName ()	{ return NAME; }

public:
	union
	{
		struct
		{
			EFFANI_PROPERTY_GHOSTING		m_Property;
		};

		struct
		{
			DWORD			m_dwFlags;
			DWORD			m_dwFrame_1;		// 시작 프레임
			DWORD			m_dwFrame_2;		// 끝 프레임
			DWORD			m_dwGhostingNum;		// 고스팅 하는 갯수
			DWORD			m_dwAlphaMax;
			DWORD			m_dwUnit;
			D3DCOLOR		m_dwColor;
		};
	};

public:
	BOOL				m_bCreated;
	void	SetCreated ( BOOL bCreated )	{ m_bCreated = bCreated; }

public:
	BOOL CheckEff ( DWORD dwCurKeyTime, DWORD dwPrevFrame );

public:
	virtual void SetProperty ( EFFANI_PROPERTY *pProperty )
	{
		m_Property = *((EFFANI_PROPERTY_GHOSTING*)pProperty);
	}
	
	virtual EFFANI_PROPERTY* GetProperty ()
	{
		return &m_Property;
	}

public:
	virtual DxEffAni* NEWOBJ( SAnimContainer* pAnimContainer );		//	Note : 이팩트 생성자.
	virtual HRESULT LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( CSerialFile &SFile );

public:
	DxEffAniData_Ghosting () : 
		DxEffAniData(),
		m_bCreated(FALSE)
	{
	}
};

class DxEffAniGhosting : public DxEffAni
{
public:
	DWORD			m_dwFlags;
	DWORD			m_dwFrame_1;		// 시작 프레임
	DWORD			m_dwFrame_2;		// 끝 프레임
	DWORD			m_dwGhostingNum;		// 고스팅 하는 갯수
	DWORD			m_dwAlphaMax;
	DWORD			m_dwUnit;
	D3DCOLOR		m_dwColor;

protected:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;

	LPDIRECT3DSTATEBLOCK9	m_pSavedBaseSB;
	LPDIRECT3DSTATEBLOCK9	m_pEffectBaseSB;

protected:
	int				m_nPrevKeyTime;

	struct	GHOSTING_INFORMATION
	{
		BOOL		m_Use;
		DWORD		m_UseNum;		// 사용한 횟수
		DWORD		m_dwKeyTime;
		D3DXMATRIX	m_matCurAni;

		GHOSTING_INFORMATION() :
			m_Use(FALSE),
			m_UseNum(0),
			m_dwKeyTime(0)
		{
			D3DXMatrixIdentity ( &m_matCurAni );
		}
	};

	typedef std::list<GHOSTING_INFORMATION>		GHOSTINGLIST;
	typedef GHOSTINGLIST::iterator				GHOSTINGLIST_ITER;

	GHOSTINGLIST								m_listGhosting;

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	virtual HRESULT OneTimeSceneInit ();
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT InvalidateDeviceObjects ();
	virtual HRESULT DeleteDeviceObjects ();
	virtual HRESULT FinalCleanup ();

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual HRESULT	Render ( const LPDIRECT3DDEVICEQ pd3dDevice, DxSkinChar* pSkinChar, const D3DXMATRIX& matCurPos );

public:
//	virtual DxEffAni* CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	DxEffAniGhosting(void);
	~DxEffAniGhosting(void);
};


#pragma once

#include "DxEffChar.h"

#include "./TextureManager.h"
#include "DxMethods.h"


struct EFFCHAR_PROPERTY_ATTRIBUTE : public EFFCHAR_PROPERTY
{
	DWORD			m_dwFlag;
	BOOL			m_bUse;
	int				m_nAttribute;
	int				m_nBlend;
	char			m_szTraceUP[STRACE_NSIZE];		//	���� ������ �ε���.
	char			m_szTraceDOWN[STRACE_NSIZE];	//	���� ������ �ε���.
	float			m_fLength;
	BOOL			m_bGravity;		// �߷�
	float			m_fGravity;		// �߷�
	BOOL			m_bFlarePos;	// ������..
	float			m_fFlarePos;
	float			m_fRandLength;
	float			m_fBlurRate;		// �þ�� ���� ���� ����
	DWORD			m_dwFaces;
	D3DCOLOR		m_vColorUP;
	D3DCOLOR		m_vColorDOWN;

	char			m_szTexture[MAX_PATH];

	EFFCHAR_PROPERTY_ATTRIBUTE () :
		m_bUse(TRUE),
		m_nAttribute(1),		//	1. ����� ���� 2... ��,����,����,��
		m_nBlend(1), 
		m_fLength(1.f),
		m_bFlarePos(TRUE),
		m_fGravity(1.f),
		m_bGravity(TRUE),
		m_fFlarePos(2.f),
		m_fRandLength(1.f),
		m_fBlurRate(0.5f),
		m_dwFaces(10),
		m_vColorUP(D3DCOLOR_ARGB(255,255,255,255)),
		m_vColorDOWN(D3DCOLOR_ARGB(0,255,255,255))
	{
		m_dwFlag = 0;
		StringCchCopy( m_szTexture, MAX_PATH, "Test_Day.dds" );

		StringCchCopy( m_szTraceUP,		STRACE_NSIZE, STRACE_IMAGE01 );
		StringCchCopy( m_szTraceDOWN,	STRACE_NSIZE, STRACE_IMAGE02 );
	}
};

class DxEffCharAttribute : public DxEffChar
{
public:
	static DWORD		TYPEID;
	static DWORD		VERSION;
	static char			NAME[MAX_PATH];

	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual DWORD GetFlag ()		{ return NULL; }
	virtual const char* GetName ()	{ return NAME; }

protected:
	union
	{
		struct
		{
			EFFCHAR_PROPERTY_ATTRIBUTE m_Property;
		};
		
		struct
		{
			DWORD			m_dwFlag;
			BOOL			m_bUse;
			int				m_nAttribute;
			int				m_nBlend;
			char			m_szTraceUP[STRACE_NSIZE];		//	���� ������ �ε���.
			char			m_szTraceDOWN[STRACE_NSIZE];	//	���� ������ �ε���.
			float			m_fLength;
			BOOL			m_bGravity;		// �߷�
			float			m_fGravity;		// �߷�
			BOOL			m_bFlarePos;	// ������..
			float			m_fFlarePos;
			float			m_fRandLength;
			float			m_fBlurRate;		// �þ�� ���� ���� ����
			DWORD			m_dwFaces;
			D3DCOLOR		m_vColorUP;
			D3DCOLOR		m_vColorDOWN;

			char			m_szTexture[MAX_PATH];
		};
	};

public:
	virtual void SetProperty ( EFFCHAR_PROPERTY *pProperty )
	{
		m_Property = *((EFFCHAR_PROPERTY_ATTRIBUTE*)pProperty);
	}
	
	virtual EFFCHAR_PROPERTY* GetProperty ()
	{
		return &m_Property;
	}

protected:	
	static LPDIRECT3DSTATEBLOCK9	m_pSavedStateBlock;
	static LPDIRECT3DSTATEBLOCK9	m_pEffectStateBlock;

	LPDIRECT3DDEVICEQ m_pd3dDevice;

protected:
	D3DXVECTOR3			m_vTransUP;				//	��ġ.
	D3DXVECTOR3			m_vTransDOWN;			//	��ġ.
	D3DXVECTOR3			m_vPrevUP;				//	��ġ.
	D3DXVECTOR3			m_vPrevDOWN;			//	��ġ.

protected:
	float	m_fTime;
	float	m_fThisTime;
	float	m_fElapsedTime;
	float	m_fFPSSumTime;

	struct VERTEX
	{
		D3DXVECTOR3	p;
		D3DCOLOR    d;
		D3DXVECTOR2	t;
		const static DWORD FVF;
	};

	LPDIRECT3DVERTEXBUFFERQ		m_pRightVB;
	LPDIRECT3DVERTEXBUFFERQ		m_pLeftVB;

	LPDIRECT3DTEXTUREQ	m_pddsTexture;

public:
	HRESULT	ResetDiffuse ();

protected:
	HRESULT CreateBlurVB ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT	ResetVB ();
	HRESULT	UpdatePos ();

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
	virtual void	Render ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPieceRender=FALSE );

public:
	virtual DxEffChar* CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece );

	virtual HRESULT LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( basestream &SFile );

public:
	DxEffCharAttribute(void);
	~DxEffCharAttribute(void);
};
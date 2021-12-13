#pragma once

#include "DxEffChar.h"

#include "./TextureManager.h"
#include "DxMethods.h"
#include "dxsimplemeshman.h"

#define	USEA			0x0001
#define	USEB			0x0002
#define	USEC			0x0004
#define	USED			0x0008
#define	USESEC			0x0010
#define	USESIZE			0x0020
#define	USERANDROTATE	0x0040
#define	USEARROW		0x0080
#define	USEEFFECT		0x0100

struct EFFCHAR_PROPERTY_ARROW_100 // Ver.100
{
	DWORD			m_dwFlag;
	BOOL			m_bUse;
	int				m_nBlend;
	int				m_nSec;
	float			m_fSetRate;				//	꼽히는 비율
	float			m_fFullTime;			// 
	float			m_fAlphaTime;			// 
	float			m_fDepth;
	float			m_fSize;
	D3DCOLOR		m_vColor;

	char			m_szMeshFile[MAX_PATH];

	EFFCHAR_PROPERTY_ARROW_100()
	{
		memset( m_szMeshFile, 0, sizeof(char)*MAX_PATH );
	}
};

struct EFFCHAR_PROPERTY_ARROW : public EFFCHAR_PROPERTY		// Ver.101
{
	DWORD			m_dwFlag;
	BOOL			m_bUse;
	int				m_nBlend;
	int				m_nSec;
	float			m_fSetRate;				//	꼽히는 비율
	float			m_fFullTime;			// 
	float			m_fAlphaTime;			// 
	float			m_fDepth;
	float			m_fSize;

	D3DXVECTOR3		m_vEffStart;
	D3DXVECTOR3		m_vEffEnd;

	D3DCOLOR		m_vColor;

	char			m_szMeshFile[MAX_PATH];
	char			m_szFileName_R[MAX_PATH];
	char			m_szFileName_C[MAX_PATH];

	EFFCHAR_PROPERTY_ARROW () :
		m_bUse(TRUE),
		m_nBlend(4),
		m_nSec(10),
		m_fSetRate(-0.3f),
		m_fFullTime(10.f),
		m_fAlphaTime(2.f),
		m_fDepth(-1.4f),
		m_fSize(1.f),
		m_vEffStart(0.f,0.f,0.f),
		m_vEffEnd(0.f,-5.f,0.f),
		m_vColor(0xffffffff)
	{
		m_dwFlag = 0;
		m_dwFlag |= USEA|USEB;
		m_dwFlag |= USERANDROTATE;
		StringCchCopy( m_szMeshFile, MAX_PATH, "sd_stone.x" );

		memset( m_szFileName_R, 0, sizeof(char)*MAX_PATH );
		memset( m_szFileName_C, 0, sizeof(char)*MAX_PATH );
	}
};

class DxEffCharArrow : public DxEffChar
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
			EFFCHAR_PROPERTY_ARROW m_Property;
		};
		
		struct
		{
			DWORD			m_dwFlag;
			BOOL			m_bUse;
			int				m_nBlend;
			int				m_nSec;
			float			m_fSetRate;				//	꼽히는 비율
			float			m_fFullTime;			// 
			float			m_fAlphaTime;			// 
			float			m_fDepth;
			float			m_fSize;

			D3DXVECTOR3		m_vEffStart;
			D3DXVECTOR3		m_vEffEnd;

			D3DCOLOR		m_vColor;

			char			m_szMeshFile[MAX_PATH];
			char			m_szFileName_R[MAX_PATH];
			char			m_szFileName_C[MAX_PATH];
		};
	};

public:
	virtual void SetProperty ( EFFCHAR_PROPERTY *pProperty )
	{
		m_Property = *((EFFCHAR_PROPERTY_ARROW*)pProperty);
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
	D3DXVECTOR3			m_vTrans;				//	위치.
	D3DXVECTOR3			m_vNormal;				//	위치.
	D3DXMATRIX			m_matWorld;
	D3DXMATRIX			m_matScale;

protected:
	float	m_fTime;
	float	m_fElapsedTime;

	struct SARROW
	{
		char		szTrace[STRACE_NSIZE];
		D3DXVECTOR3	vDir;
		float		fStartTime;
		float		fRotate;
		float		fHeight;
		SARROW*		pNext;

		SARROW () :
			pNext(NULL)
		{
			memset( szTrace, 0, sizeof(char)*STRACE_NSIZE );
		}
	};

	//struct VERTEX 
	//{ 
	//	D3DXVECTOR3 p; 
	//	D3DCOLOR	cColor; 
	//	D3DXVECTOR2 t; 
	//};

	float		m_fTarDirection;
	D3DXVECTOR3	m_vDirection;
	float		m_fAlphaDelta;
	float		m_fAlphaStart;
	D3DXCOLOR	m_cBaseColor;
	D3DXCOLOR	m_cColor;

	float		m_fNumSec;		// 갯수 / 1초

	SARROW*		m_pArrowHead;

	DxSimMesh*	m_pMesh;					//	Note: 렌더링 메쉬 저장 부분 

protected:
	EFF_PROPGROUP*		m_pPropGroup_R;			//	속성값.
	DxEffSingleGroup*	m_pSingleGroup_R;		//	활성 개체.
	EFF_PROPGROUP*		m_pPropGroup_C;			//	속성값.
	DxEffSingleGroup*	m_pSingleGroup_C;		//	활성 개체.

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
	HRESULT	InsertArrow ( D3DXVECTOR3& vDirc );	

protected:
	HRESULT RenderSingle ( LPDIRECT3DDEVICEQ pd3dDevice, D3DMATRIX *matWorld, D3DXVECTOR3 vStart, D3DXVECTOR3 vEnd );

public:
	virtual DxEffChar* CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece );
	virtual void SetDirection ( D3DXVECTOR3 *pDir, float fTarDir )	{ m_vDirection = *pDir; m_fTarDirection = fTarDir; }

	virtual HRESULT LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( basestream &SFile );

public:
	DxEffCharArrow(void);
	~DxEffCharArrow(void);
};
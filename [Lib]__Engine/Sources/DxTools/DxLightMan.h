#pragma once

#include <vector>
#include <map>
#include "./CList.h"
#include "DxCustomTypes.h"
#include "DxVertexFVF.h"

class	CSerialFile;

#define STRING_LENGTH_256	256

class DXLIGHT
{
public:
	char			m_szLightName[STRING_LENGTH_256];
	BOOL			m_bEnable;
	D3DCOLORVALUE	m_BaseDiffuse;
	D3DXMATRIX		m_matWorld;
	
	D3DLIGHTQ		m_Light;

	BOOL			m_bEditCheck;

	//	Note : 단방향 리스트.
	DXLIGHT			*pNext;

public:
	DXLIGHT () :
		m_bEnable(TRUE),
		m_bEditCheck(FALSE),
		pNext(NULL)
	{
		memset( m_szLightName, 0, sizeof(char)*STRING_LENGTH_256 );
		D3DXMatrixIdentity( &m_matWorld );
	}

	virtual ~DXLIGHT ()
	{
		SAFE_DELETE(pNext);
	}

	void SetDefault ()
	{
		StringCchCopy( m_szLightName, STRING_LENGTH_256, "UNNAMED" );

		SecureZeroMemory( &m_Light, sizeof(D3DLIGHTQ) );
		m_Light.Type        = D3DLIGHT_DIRECTIONAL;
		m_Light.Diffuse.r   = 1.f;
		m_Light.Diffuse.g   = 1.f;
		m_Light.Diffuse.b   = 1.f;
		m_Light.Ambient.r	= 0.4f;
		m_Light.Ambient.g	= 0.4f;
		m_Light.Ambient.b	= 0.4f;
		m_Light.Specular.r   = 1.0f;
		m_Light.Specular.g   = 1.0f;
		m_Light.Specular.b   = 1.0f;

		D3DXVECTOR3 vDirection = D3DXVECTOR3(-1.0f, -2.0f, 2.0f);
		D3DXVec3Normalize ( &vDirection, &vDirection );
		m_Light.Direction = vDirection;

		m_Light.Range		= 80.0f;

		m_Light.Attenuation0 = 1.0f;
		m_Light.Attenuation1 = 0.0f;
		m_Light.Attenuation2 = 0.0f;

		//m_Ambient.a = 0.0f;
		m_BaseDiffuse.r = 1.0f;
		m_BaseDiffuse.g = 1.0f;
		m_BaseDiffuse.b = 1.0f;
	}

	void operator= ( const DXLIGHT& Ligth )
	{
		StringCchCopy( m_szLightName, STRING_LENGTH_256, Ligth.m_szLightName );
		m_bEnable		= Ligth.m_bEnable;
		m_BaseDiffuse	= Ligth.m_BaseDiffuse;
		m_Light			= Ligth.m_Light;
		m_matWorld		= Ligth.m_matWorld;

		// pNext 는 대입 연산에서 제외된다.
	}

	void SetRange ( float fRange );
	virtual void SerialData ();

	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	//	<--	데이타 Save/Load
	void	SaveSet ( CSerialFile &SFile );
	void	LoadSet ( CSerialFile &SFile );
	void	LoadSet_Ver100( CSerialFile &SFile );
	void	LoadSet_Ver101_2( CSerialFile &SFile );
	//	-->	데이타 Save/Load
};

typedef DXLIGHT* PDXLIGHT;

struct DXLIGHTNODE
{
	union
	{
		struct { float fMaxX, fMaxY, fMaxZ; };
		struct { D3DXVECTOR3 vMax; };
	};

	union
	{
		struct { float fMinX, fMinY, fMinZ; };
		struct { D3DXVECTOR3 vMin; };
	};

	DXLIGHT			*pLight;

	//	Note : 정렬 트리용.
	//
	DXLIGHTNODE		*pLeftChild;
	DXLIGHTNODE		*pRightChild;

	BOOL IsCollisionVolume ( CLIPVOLUME & cv );

	DXLIGHTNODE () :
		fMaxX(0.0f),
		fMaxY(0.0f),
		fMaxZ(0.0f),
		fMinX(0.0f),
		fMinY(0.0f),
		fMinZ(0.0f),
		pLight(NULL),
		pLeftChild(NULL),
		pRightChild(NULL)
	{
	}

	~DXLIGHTNODE ()
	{
		SAFE_DELETE(pLeftChild);
		SAFE_DELETE(pRightChild);
	}

public:
	//	<--	데이타 Save/Load
	void	SaveSet ( CSerialFile &SFile );
	void	LoadSet ( CSerialFile &SFile, DXLIGHT	*pLtListHead );
	//	-->	데이타 Save/Load
};

class DxLightMan
{
public:
	typedef std::multimap<float,DXLIGHT*>		CLMAN;
	typedef CLMAN::iterator						CLMAN_ITER;

	typedef std::multimap<float,D3DLIGHT9*>		MANPOINTLIGHT;
	typedef MANPOINTLIGHT::iterator				MANPOINTLIGHT_ITER;

public:
	enum { VERSION = 0x00103, _MAX_LIGHT = 8};

protected:	// SAVE / LOAD 필요.
	BOOL		m_bLighting;
	BOOL		m_bNightAndDay;
	BOOL		m_bWhenDayPointOff;	//	낮일때 포인트 라이트 끔.

	DXLIGHT		m_LtDirectNoon;
	DXLIGHT		m_LtDirectNight;
	
	DXLIGHT		*m_pLtListHead;
	DXLIGHTNODE	*m_pTreeHead;

private:
	typedef std::vector<DXLIGHT*>	VECDXLIGHT;
	typedef VECDXLIGHT::iterator	VECDXLIGHT_ITER;

	VECDXLIGHT	m_vecPointLight;	// 참조만 하기 때문에 삭제 포인터를 Delete 할 필요는 없다.

	void ListToVectorLIGHT();

protected:
	DXLIGHT		*m_pLtTest;

	DWORD		m_dwRenderCount;
	DWORD		m_dwBackRenderCount;
	DWORD		m_dwRegistCount;

	BOOL		m_bRendRange;

	DXLIGHT		m_LtDirectNow;
	CLMAN		m_mmapClosedLight;


	float		m_fThunderTimer;
	int			m_nThunder;
	int			m_nThunderFULL;
	float		m_fThunderPOWER;
	D3DLIGHTQ	m_sThunderLight;

	BOOL		m_bDSPName;

protected:
	D3DLIGHTQ	m_sDPLight[ _MAX_LIGHT ];
	DWORD		m_dwDPLight_Count;

	D3DLIGHTQ	m_sSaveDPLight[ _MAX_LIGHT ];
	DWORD		m_dwSaveDPLight_Count;

	DWORD		m_dwDPLight;

public:
	BOOL		ISColsedOnePL ( D3DXVECTOR3 vPos );										// 지금 위치에 영향을 주는 동적 라이트가 있는가 ?
	BOOL		ISDynamicLight ()						{ return m_dwDPLight_Count; }	// 동적 라이트가 있는가 ?
	void		SetDPLight ( D3DLIGHTQ sLight );		// 셋팅하면 효과는 다음 프레임에 나타난다.
	D3DLIGHTQ*	GetClosedDPLight ()						{ return &m_sDPLight[m_dwDPLight]; }
	void		Clone2DPLight ();
	void		SetDPLight ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	void SetPiecePointIndex( const D3DXVECTOR3& vPos, const float& fLength, VEC_DWORD& vecPointIndex );
	D3DLIGHTQ* GetPointLight( const DWORD& dwIndex );
	char* GetStrPointLight( const DWORD& dwIndex );

public:
	LPDIRECT3DDEVICEQ m_pd3dDevice;

public:
	void SetDefault ();

	void EnableLighting ( BOOL bEnable )	{ m_bLighting = bEnable; }
	BOOL IsEnableLighting ()				{ return m_bLighting; }

	void EnableNightAndDay ( BOOL bEnable )	{ m_bNightAndDay = bEnable; }
	BOOL IsNightAndDay ()					{ return m_bNightAndDay; }

	void EnableWhenDayPointOff ( BOOL bEnable )	{ m_bWhenDayPointOff = bEnable; }
	BOOL IsWhenDayPointOff ()					{ return m_bWhenDayPointOff; }

	void SetDirectNoonLight ( const DXLIGHT& ltDirectional );
	void SetDirectNightLight ( const DXLIGHT& ltDirectional );

	DXLIGHT* GetDirectLight ();

	DXLIGHT* GetDirectNoonLight ();
	DXLIGHT* GetDirectNightLight ();

	DXLIGHT* GetClosedLight ()
	{
		if ( m_mmapClosedLight.empty() )	return NULL;
		return (*m_mmapClosedLight.begin()).second;
	};

	DXLIGHT* GetClosed2Light ()
	{
		if ( m_mmapClosedLight.size() < 2 )	return NULL;
		
		CLMAN_ITER iter = m_mmapClosedLight.begin();
		++iter;
		return (*iter).second;
	};

	DXLIGHT* GetClosedLight ( int nPoint )
	{
		if ( m_mmapClosedLight.size() < (DWORD)nPoint )	return NULL;
		
		CLMAN_ITER iter = m_mmapClosedLight.begin();
		for ( int i=0; i<(nPoint-1); i++ )
		{
			++iter;
		}
		return (*iter).second;
	};

	void EnableRangeRend ( BOOL bRend )	{ m_bRendRange = bRend; }
	BOOL IsEnableRangeRend ()			{ return m_bRendRange; }

	void Thunder ();
	float GetThunderPOWER ()	{ return m_fThunderPOWER; }		// 번개 빛의 세기

	// Note : 인터페이스
public:
	void Clone_MouseShift( LPD3DXMATRIX& pMatrix );
	void SetDSPName( BOOL bUse )						{ m_bDSPName = bUse; }

public:
	DXLIGHT*	GetLight ( const char* szLightName );
	void		AddLight ( DXLIGHT* pLight );
	void		DelLight ( const char* szLightName );

	DWORD		GetRegistCount ()			{ return m_dwRegistCount; }
	DXLIGHT*	GetLightHead ()				{ return m_pLtListHead; }

	void		TestLight ( DXLIGHT* pLight );
	void		ResetTestLight ();

	DWORD		ComputeClosedPointLight( D3DXVECTOR3* pPos, D3DXVECTOR3* pNor );

protected:
	void		ComputePointLight( DXLIGHTNODE *pLightNode, D3DXVECTOR3* pPos, D3DXVECTOR3* pNor, D3DXVECTOR3& vColor );

	// Note : 캐릭터 관련
public:
	void SetCharStaticPLight( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vPos );	// 지형 Light를 중심으로 셋팅
	void SetCharDynamicPLight( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vPos );	// 효과 Light를 중심으로 셋팅

protected:
	void MakeAABBTree ( PDXLIGHT *ppLightArray, DWORD dwArraySize, DXLIGHTNODE* pLightNode,
		const D3DXVECTOR3 &vMax, const D3DXVECTOR3 &vMin );

public:
	void MakeAABBTree ();

public:
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT DeleteDeviceObjects ();

public:
	void CleanUp ();

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice );

	void TreeRender ( DXLIGHTNODE *pLightNode, LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME& cv );

	void ReSetLight( LPDIRECT3DDEVICEQ pd3dDevice );	// Render을 한 후 나중에 변할 경우
	void LightDisable34567( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	void	SaveSet ( CSerialFile &SFile, DWORD dwSetLandVer );
	void	LoadSet ( CSerialFile &SFile, DWORD dwSetLandVer );

public:
	void	Save ( CSerialFile &SFile );
	void	Load ( CSerialFile &SFile, DWORD dwLandVer, BOOL bSETLAND );

public:
	void	CloneInstance ( DxLightMan* pLightMan );

public:
	DxLightMan(void);
	virtual ~DxLightMan(void);

public:
	static void SetDefDirect ( const DXLIGHT& ltDirectional );

protected:
	static DxLightMan	InstanceDefault;
	static DxLightMan*	pInstance;

public:
	static DxLightMan* GetInstance ()
	{
		if ( pInstance )	return pInstance;
		return &InstanceDefault;
	}

	static void SetInstance ( DxLightMan* pLightMan )
	{
		if ( pLightMan==NULL )	pInstance = NULL;
		else					pInstance = pLightMan;
	}
};

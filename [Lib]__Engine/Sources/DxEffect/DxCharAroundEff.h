#pragma once


#include <vector>
#include "CMemPool.h"
#include "DxVertexFVF.h"
#include "DxSequenceUV.h"

#include "DxEffChar.h"

#define	AROUNDEFFECT_TEXNAME_SIZE	32
#define	MAX_EFFECT					100
#define	USE_SEQUENCE	0x0008	// 시퀀스 사용




struct EFFCHAR_PROPERTY_CHAR_AROUND : public EFFCHAR_PROPERTY
{
	DWORD					m_dwFlag;
	char					m_szTexture[AROUNDEFFECT_TEXNAME_SIZE];
	DWORD					m_dwColor;
	DWORD					m_dwSequenceCol;
	DWORD					m_dwSequenceRow;
	FLOAT					m_fMinTextureSpeed;	// 파티클의 텍스쳐 애니메이션 속도
	FLOAT					m_fMaxTextureSpeed;	// 파티클의 텍스쳐 애니메이션 속도
	D3DXVECTOR2				m_vMinSize;				// 파티클 크기
	D3DXVECTOR2				m_vMaxSize;				// 파티클 크기
	float					m_fMinLifeTime;			// 파티클의 수명
	float					m_fMaxLifeTime;			// 파티클의 수명
	float					m_fMinRivalTime;			// 파티클의 재생 간격
	float					m_fMaxRivalTime;			// 파티클의 재생 간격

	EFFCHAR_PROPERTY_CHAR_AROUND() :
		m_dwFlag(0L), 
		m_dwSequenceCol(8), 
		m_dwSequenceRow(8),
		m_dwColor(0xffffffff),	
		m_fMinLifeTime(5.0f),
		m_fMaxLifeTime(10.0f),
		m_fMinRivalTime(1.0f),
		m_fMaxRivalTime(10.0f)
	{
		ZeroMemory( m_szTexture, AROUNDEFFECT_TEXNAME_SIZE );
		m_fMinTextureSpeed = 25.0f;
		m_fMaxTextureSpeed = 25.0f;
		m_vMinSize		   = D3DXVECTOR2( 1.0f, 1.0f );
		m_vMaxSize		   = D3DXVECTOR2( 1.0f, 1.0f );
	}
};





struct SAROUNDEFF_DATA
{	
	D3DXVECTOR3 vPos;			// 이펙트 위치
	std::string	strBoneName;	// 이펙트가 링크된 본의 이름
	D3DXVECTOR3	vNormal;		// 메시와의 노말 벡터
	float		fMeshDist;		// 메시와의 거리


	D3DXVECTOR2 vRenderSize;			// 파티클 크기
	float		fRenderLifeTime;		// 파티클의 수명
	float		fRenderRivalTime;		// 파티클의 재생 간격
	float		fRunTexAnimateSpeed;	// 파티클의 텍스쳐 애니메이션 속도
	float		fRenderRotate;

	float		fRunTime;			// 파티클의 애니메이션 프레임 및 나이
	float		fRunTextureNum;		// 파티클 텍스쳐의 애니메이션 번호
	bool		bAlive;				// 현재 파티클이 죽었는지 살았는지 체크

	SAROUNDEFF_DATA()
	{		
		bAlive		   = FALSE;
		fRunTime	   = 0;
		fRunTextureNum = 0;
		fRenderRotate  = 0.0f;
	}

	void CreateParticleData( EFFCHAR_PROPERTY_CHAR_AROUND property );
	void FrameMove( float fElapsedTime, EFFCHAR_PROPERTY_CHAR_AROUND property );
};




class DxCharAroundEff : public DxEffChar
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
			EFFCHAR_PROPERTY_CHAR_AROUND m_Property;
		};

		struct
		{
			DWORD					m_dwFlag;
			char					m_szTexture[AROUNDEFFECT_TEXNAME_SIZE];
			DWORD					m_dwColor;
			DWORD					m_dwSequenceCol;
			DWORD					m_dwSequenceRow;
			float					m_fMinTextureSpeed;	// 파티클의 텍스쳐 애니메이션 속도
			float					m_fMaxTextureSpeed;	

		};
	};
private:
	float					m_fAnimateNum;

	LPDIRECT3DTEXTUREQ		m_pTexture;
	vector<SAROUNDEFF_DATA> m_vecAroundEffList;

	VERTEXCOLORTEX2 m_Vertices[MAX_EFFECT*4];



public:
	virtual void SetProperty ( EFFCHAR_PROPERTY *pProperty )
	{
		m_Property = *((EFFCHAR_PROPERTY_CHAR_AROUND*)pProperty);
	}

	virtual EFFCHAR_PROPERTY* GetProperty ()
	{
		return &m_Property;
	}

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );

	virtual HRESULT OneTimeSceneInit ();
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT InvalidateDeviceObjects ();
	virtual HRESULT DeleteDeviceObjects ();
	virtual HRESULT FinalCleanup ();

	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual void	RenderEff( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMATRIX pMatrix, const float fScale );
	virtual void	RenderEDIT( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMATRIX pMatrix, const float fScale );


	DxEffChar* CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece );

	virtual HRESULT LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT SaveFile ( basestream &SFile );

	virtual DWORD GetStateOrder () const						{ return EMEFFSO_SINGLE_EFF; }

	HRESULT CleanUp ()
	{
		InvalidateDeviceObjects();
		DeleteDeviceObjects();
		FinalCleanup();

		return S_OK;
	}

public:
	void					InsertAroundEffect( SAROUNDEFF_DATA addData, bool bTool = FALSE );
	void					DeleteAroundEffect( int nIndex );
	vector<SAROUNDEFF_DATA> GetAroundEffect() { return m_vecAroundEffList; }
	void RenderCountText( DWORD nCount, const D3DXVECTOR3& vPos );


public:
	DxCharAroundEff(void);
	~DxCharAroundEff(void);
};

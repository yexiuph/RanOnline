#pragma once


#include <vector>
#include "CMemPool.h"
#include "DxVertexFVF.h"
#include "DxSequenceUV.h"

#include "DxEffChar.h"

#define	AROUNDEFFECT_TEXNAME_SIZE	32
#define	MAX_EFFECT					100
#define	USE_SEQUENCE	0x0008	// ������ ���




struct EFFCHAR_PROPERTY_CHAR_AROUND : public EFFCHAR_PROPERTY
{
	DWORD					m_dwFlag;
	char					m_szTexture[AROUNDEFFECT_TEXNAME_SIZE];
	DWORD					m_dwColor;
	DWORD					m_dwSequenceCol;
	DWORD					m_dwSequenceRow;
	FLOAT					m_fMinTextureSpeed;	// ��ƼŬ�� �ؽ��� �ִϸ��̼� �ӵ�
	FLOAT					m_fMaxTextureSpeed;	// ��ƼŬ�� �ؽ��� �ִϸ��̼� �ӵ�
	D3DXVECTOR2				m_vMinSize;				// ��ƼŬ ũ��
	D3DXVECTOR2				m_vMaxSize;				// ��ƼŬ ũ��
	float					m_fMinLifeTime;			// ��ƼŬ�� ����
	float					m_fMaxLifeTime;			// ��ƼŬ�� ����
	float					m_fMinRivalTime;			// ��ƼŬ�� ��� ����
	float					m_fMaxRivalTime;			// ��ƼŬ�� ��� ����

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
	D3DXVECTOR3 vPos;			// ����Ʈ ��ġ
	std::string	strBoneName;	// ����Ʈ�� ��ũ�� ���� �̸�
	D3DXVECTOR3	vNormal;		// �޽ÿ��� �븻 ����
	float		fMeshDist;		// �޽ÿ��� �Ÿ�


	D3DXVECTOR2 vRenderSize;			// ��ƼŬ ũ��
	float		fRenderLifeTime;		// ��ƼŬ�� ����
	float		fRenderRivalTime;		// ��ƼŬ�� ��� ����
	float		fRunTexAnimateSpeed;	// ��ƼŬ�� �ؽ��� �ִϸ��̼� �ӵ�
	float		fRenderRotate;

	float		fRunTime;			// ��ƼŬ�� �ִϸ��̼� ������ �� ����
	float		fRunTextureNum;		// ��ƼŬ �ؽ����� �ִϸ��̼� ��ȣ
	bool		bAlive;				// ���� ��ƼŬ�� �׾����� ��Ҵ��� üũ

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
			float					m_fMinTextureSpeed;	// ��ƼŬ�� �ؽ��� �ִϸ��̼� �ӵ�
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

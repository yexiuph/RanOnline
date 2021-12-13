//  interface for the DxEffectSpecular class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__DXEFFCTMULTITEX_INCLUDED__)
#define __DXEFFCTMULTITEX_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DxVertexFVF.h"
#include "./DxEffectFrame.h"

struct DxMeshes;

struct DXMATERIAL_MULTITEX
{
	D3DMATERIALQ		rgMaterial;
	LPDIRECT3DTEXTUREQ	pTexture;
	char				szTexture[MAX_PATH];

	DXMATERIAL_MULTITEX () :
		pTexture(NULL)
	{
		memset( szTexture, 0, sizeof(char)*MAX_PATH );
	}

	~DXMATERIAL_MULTITEX();
};

struct MULTITEX_PROPERTY_100
{
	BOOL			m_bTex1;
	BOOL			m_bTex2;
	BOOL			m_bTex3;
	BOOL			m_bSequence2;
	BOOL			m_bSequence3;
	BOOL			m_bSequenceLoop2;
	BOOL			m_bSequenceLoop3;

	float			m_fAlphaAVG;
	float			m_fAlphaDelta;
	float			m_fAlphaSpeed;

	float			m_fAniTime01;
	int				m_nCol01;
	int				m_nRow01;
	float			m_fAniTime02;
	int				m_nCol02;
	int				m_nRow02;

	DWORD			m_SrcBlend; 
	DWORD			m_DestBlend;

	D3DXVECTOR2		m_vMoveTex1;
	D3DXVECTOR2		m_vMoveTex2;
	D3DXVECTOR2		m_vMoveTex3;

	D3DCOLOR		m_vColor;
	D3DXMATRIX		m_matFrameComb;

	D3DXVECTOR3		m_vMax;
	D3DXVECTOR3		m_vMin;

	char			m_szTexture2[MAX_PATH];
	char			m_szTexture3[MAX_PATH];

	MULTITEX_PROPERTY_100()
	{
		memset( m_szTexture2, 0, sizeof(char)*MAX_PATH );
		memset( m_szTexture3, 0, sizeof(char)*MAX_PATH );
	}
};

struct MULTITEX_PROPERTY
{
	BOOL			m_bTex1;
	BOOL			m_bTex2;
	BOOL			m_bTex3;
	BOOL			m_bSequence2;
	BOOL			m_bSequence3;
	BOOL			m_bSequenceLoop2;
	BOOL			m_bSequenceLoop3;

	float			m_fAlphaAVG;
	float			m_fAlphaDelta;
	float			m_fAlphaSpeed;

	float			m_fAniTime01;
	int				m_nCol01;
	int				m_nRow01;
	float			m_fAniTime02;
	int				m_nCol02;
	int				m_nRow02;

	DWORD			m_SrcBlend; 
	DWORD			m_DestBlend;

	D3DXVECTOR2		m_vMoveTex1;
	D3DXVECTOR2		m_vMoveTex2;
	D3DXVECTOR2		m_vMoveTex3;

	D3DXVECTOR2		m_vScaleTex2;
	D3DXVECTOR2		m_vScaleTex3;

	D3DCOLOR		m_vColor;
	D3DXMATRIX		m_matFrameComb;

	D3DXVECTOR3		m_vMax;
	D3DXVECTOR3		m_vMin;

	char			m_szTexture2[MAX_PATH];
	char			m_szTexture3[MAX_PATH];

	MULTITEX_PROPERTY()
	{
		memset( m_szTexture2, 0, sizeof(char)*MAX_PATH );
		memset( m_szTexture3, 0, sizeof(char)*MAX_PATH );
	}
};

class DxEffectMultiTex : public DxEffectBase
{
	//	Note : 이펙트 타입 정의.
	//
public:
	const static DWORD	TYPEID;
	const static DWORD	VERSION;
	const static DWORD	FLAG;
	const static char	NAME[];

public:
	virtual DWORD GetTypeID ()		{ return TYPEID; }
	virtual DWORD GetFlag ()		{ return FLAG; }
	virtual const char* GetName ()	{ return NAME; }

	virtual void GetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD &dwVer );
	virtual void SetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD dwVer );

protected:
	static LPDIRECT3DSTATEBLOCK9	m_pSB_Tex2;
	static LPDIRECT3DSTATEBLOCK9	m_pSB_Tex2_SAVE;
	static LPDIRECT3DSTATEBLOCK9	m_pSB_Tex3;
	static LPDIRECT3DSTATEBLOCK9	m_pSB_Tex3_SAVE;
	static LPDIRECT3DSTATEBLOCK9	m_pSB_Tex23;
	static LPDIRECT3DSTATEBLOCK9	m_pSB_Tex23_SAVE;

public:
	void SetProperty ( MULTITEX_PROPERTY& Property )
	{
		m_Property = Property;
	}
	MULTITEX_PROPERTY& GetProperty () { return m_Property; }

	//	Note : 속성.
	//
protected:
	union
	{
		struct
		{
			MULTITEX_PROPERTY	m_Property;
		};

		struct
		{
			BOOL			m_bTex1;
			BOOL			m_bTex2;
			BOOL			m_bTex3;
			BOOL			m_bSequence2;
			BOOL			m_bSequence3;
			BOOL			m_bSequenceLoop2;
			BOOL			m_bSequenceLoop3;

			float			m_fAlphaAVG;
			float			m_fAlphaDelta;
			float			m_fAlphaSpeed;

			float			m_fAniTime01;
			int				m_nCol01;
			int				m_nRow01;
			float			m_fAniTime02;
			int				m_nCol02;
			int				m_nRow02;

			DWORD			m_SrcBlend; 
			DWORD			m_DestBlend;

			D3DXVECTOR2		m_vMoveTex1;
			D3DXVECTOR2		m_vMoveTex2;
			D3DXVECTOR2		m_vMoveTex3;

			D3DXVECTOR2		m_vScaleTex2;
			D3DXVECTOR2		m_vScaleTex3;

			D3DCOLOR		m_vColor;
			D3DXMATRIX		m_matFrameComb;

			D3DXVECTOR3		m_vMax;
			D3DXVECTOR3		m_vMin;

			char			m_szTexture2[MAX_PATH];
			char			m_szTexture3[MAX_PATH];
		};
	};

protected:
	LPDIRECT3DDEVICEQ		_pd3dDevice;
	float					m_fTime;

protected:
	LPDIRECT3DVERTEXBUFFERQ		m_pSrcVB;
	LPDIRECT3DVERTEXBUFFERQ		m_pColorVB;
	LPDIRECT3DINDEXBUFFERQ		m_pIB;			// 저장
	D3DXVECTOR2*				m_pTexUV;		// 저장	// 텍스쳐 초기 값을 위해서

	DWORD						m_dwVertices;					// 저장
	DWORD						m_dwFaces;						// 저장

	DWORD						m_dwAttribTableSize;
	LPD3DXATTRIBUTERANGE		m_pAttribTable;

	DWORD						m_cMaterials;
	DXMATERIAL_MULTITEX*		m_pMaterials;

	D3DXVECTOR3					m_vCenter;

	D3DXVECTOR2					m_vAddTex1;		// 누적 값 - 이동
	D3DXVECTOR2					m_vAddTex2;		// 누적 값 - 이동
	D3DXVECTOR2					m_vAddTex3;		// 누적 값 - 이동

	LPDIRECT3DTEXTUREQ			m_pddsTexture2;
	LPDIRECT3DTEXTUREQ			m_pddsTexture3;

public:
	virtual HRESULT AdaptToDxFrame ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT AdaptToDxFrameChild ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice );

	HRESULT AdaptToEffList ( const DxFrame *const pFrame, LPDIRECT3DDEVICEQ pd3dDevice );

public:
	void	ResetTexture ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT DeleteDeviceObjects ();

	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT InvalidateDeviceObjects ();

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual HRESULT Render ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan );

protected:
	HRESULT	CreateObject( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes *pmsMeshs );
	HRESULT	ChangeObject ( VERTEX* pSrcVert, VERTEXCOLORTEX2* pDestVert, WORD* pIndices, DWORD dwVert, DWORD dwFaces, float fScale );
	VOID	UpdateUV();

public:
	VOID	UpdateDiffuse ();

public:
	DxEffectMultiTex ();
	virtual ~DxEffectMultiTex ();

protected:
	virtual void SaveBuffer ( CSerialFile &SFile );
	virtual void LoadBufferSet ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual void LoadBuffer ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice );

	//	Note : DxFrameMesh 의 효과를 DxLandMan 에 삽입 할 때 대량의 버퍼 데이타를
	//		복제하기 위해서 사용한다.
public:
	virtual HRESULT CloneData ( DxEffectBase* pSrcEffect, LPDIRECT3DDEVICEQ pd3dDevice );

public:
	HRESULT SetBoundBox ();
	virtual void	GetAABBSize( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );
	virtual	BOOL	IsDetectDivision( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin );

};

#endif // !defined(__DXEFFCTMULTITEX_INCLUDED__)

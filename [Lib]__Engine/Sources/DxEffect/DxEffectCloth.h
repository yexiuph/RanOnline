// DxEffectCloth.h: interface for the DxEffectAniso class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DXEFFECTCLOTH_H__INCLUDED_)
#define AFX_DXEFFECTCLOTH_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DxSpring.h"
#include "DxWind.h"
#include "DxVertexFVF.h"
#include "./DxEffectFrame.h"

struct DxFrame;

struct CLOTH_PROPERTY_100
{
	bool			m_bPlay;
	int				m_iNodeCount;
	float			m_fCoEfficient;			//탄성 계수    
	float			m_fDamping;				//감쇠기 상수 [고정]
	D3DXVECTOR3		m_vWindDirection;		//바람의 방향 벡터 
	float			m_fWindForce;			//바람의 세기 
	D3DXVECTOR3		m_vGravityDirection;
	float			m_fGravity;				//중력 부분	
	char			m_szTexture[MAX_PATH];

	CLOTH_PROPERTY_100()
	{
		memset( m_szTexture, 0, sizeof(char)*MAX_PATH );
	};
};

struct CLOTH_PROPERTY_101
{
	bool			m_bPlay;
	int				m_iNodeCount;
	float			m_fCoEfficient;			//탄성 계수    
	D3DXVECTOR3		m_vWindDirection;		//바람의 방향 벡터 
	float			m_fWindForce;			//바람의 세기 	
	float			m_fGravity;				//중력 부분	
	char			m_szTexture[MAX_PATH];

	CLOTH_PROPERTY_101()
	{
		memset( m_szTexture, 0, sizeof(char)*MAX_PATH );
	};
};


struct CLOTH_PROPERTY_102_103
{
	bool			m_bPlay;
	int				m_iNodeCount;
	float			m_fCoEfficient;			//탄성 계수    
	D3DXVECTOR3		m_vWindDirection;		//바람의 방향 벡터 
	float			m_fWindForce;			//바람의 세기 	
	float			m_fGravity;				//중력 부분	
	char			m_szTexture[MAX_PATH];

	CLOTH_PROPERTY_102_103()
	{
		memset( m_szTexture, 0, sizeof(char)*MAX_PATH );
	}
};

struct CLOTH_PROPERTY		// Ver.104
{
	BOOL			m_bPlay;
	BOOL			m_bEachRender;
	int				m_iNodeCount;
	float			m_fCoEfficient;			//탄성 계수    
	D3DXVECTOR3		m_vWindDirection;		//바람의 방향 벡터 
	float			m_fWindForce;			//바람의 세기 	
	float			m_fGravity;				//중력 부분	
	char			m_szTexture[MAX_PATH];

	CLOTH_PROPERTY()
	{
		memset( m_szTexture, 0, sizeof(char)*MAX_PATH );
	}
};


class DxEffectCloth : public DxEffectBase
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
	virtual BOOL  GetEachRender()	{ return m_bEachRender; }

	virtual void GetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD &dwVer );
	virtual void SetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD dwVer );

public:
	struct PARTICLE
	{		
		D3DXVECTOR3   vPosition;
		D3DXVECTOR3   vVelocity;
		D3DXVECTOR3   vAccelerate;
		D3DXVECTOR3   vForce;
		BOOL          bLocked;
	};

	struct NODE
	{
		WORD Index1;
		WORD Index2;
		float fLength;
	};

	//	Note : 속성.
	//
protected:
	union
	{
		struct
		{
			CLOTH_PROPERTY	m_Property;
		};

		struct
		{
			BOOL			m_bPlay;
			BOOL			m_bEachRender;
			int				m_iNodeCount;
			float			m_fCoEfficient;          //탄성 계수    			
			D3DXVECTOR3		m_vWindDirection;		 //바람의 방향 벡터 
			float			m_fWindForce;            //바람의 세기 			
			float			m_fGravity;				 //중력 부분	
			char			m_szTexture[MAX_PATH];
		};
	};

public:
	void SetProperty ( CLOTH_PROPERTY& Property )
	{
		m_Property = Property;
	}
	CLOTH_PROPERTY& GetProperty () { return m_Property; }

public:
	DWORD		m_dwNumLockIndex;
	WORD		*m_pLockIndex;

protected:
	DWORD						m_dwNumVertices;
	DWORD						m_dwNumIndices;

	VERTEX*		m_pBaseVertArray;
	WORD*		m_pBaseIndexArray;
	VERTEX*		m_pVertArray;
	WORD*		m_pIndexArray;
	LPDIRECT3DINDEXBUFFERQ		m_pIB;

	NODE		*m_pNodeTable;
	PARTICLE	*m_pParticle;

	WORD		*m_pTempLockIndex;

	DxSpring	m_Spring;
	DxWind		m_Wind;      //바람 클래스 

	float		m_fTime;	

	int			m_iWindMin;	
	D3DXMATRIX	m_matWorld;
	float		m_fWindConstant;

	float		m_fSumTime;

	BOOL				m_bAlphaMap;
	LPDIRECT3DTEXTUREQ	m_pddsTexture;

public:
	static HRESULT CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice );
	static HRESULT ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice );

protected:
	HRESULT AdaptToDxFrameChild ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice );

public:
	HRESULT AdaptToDxFrame ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT AdaptToEffList ( const DxFrame *const pFrame, LPDIRECT3DDEVICEQ pd3dDevice );

	//	Note : 
	//
public:
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT DeleteDeviceObjects ();

public:
	void XFileLoad( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pLocalMesh );
	void CreateNodeTable( VERTEX *pVertices, DWORD dwNumIndices, WORD *pIndices );
	void SetParticle( int index, D3DXVECTOR3 &vPosition );
	BOOL CreateVertex( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwNumVertices, VERTEX* pVertices, DWORD dwNumIndices, WORD* pIndices );	
	void DeleteClothData( int a );

public:
	HRESULT FrameMove ( float fTime, float fElapsedTime );	
	HRESULT Render ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan );

protected:
	HRESULT MeshUpdate ();
	void ComputeNormal(int nVertices,  VERTEX *pVertices, int nIndices, WORD *pIndices  );
	inline  float Random( int nMin, int nMax );

public:
	void GetIndexLock();
	DWORD GetdwNumLockIndex() { return m_dwNumLockIndex; }
	char* GetStrList( DWORD i );
	DWORD GetdwVertex( DWORD i );
	NODE* GetNodeTable () { return m_pNodeTable; }
	int GetNodeTableSize () { return m_iNodeCount; }

public:
	void SetIndexLock();
	void BaseBuffToDynamicBuff ( LPDIRECT3DDEVICEQ pd3dDevice );
	void SetParticleLock( int index , BOOL bLook );	

public:
	DxEffectCloth();
	virtual ~DxEffectCloth();

protected:
	virtual void SaveBuffer ( CSerialFile &SFile );
	virtual void LoadBufferSet ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual void LoadBuffer ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice );

	//	Note : DxFrameMesh 의 효과를 DxLandMan 에 삽입 할 때 대량의 버퍼 데이타를
	//		복제하기 위해서 사용한다.
public:
	virtual HRESULT CloneData ( DxEffectBase* pSrcEffect, LPDIRECT3DDEVICEQ pd3dDevice );

	//	Note : OBJAABB
	//
public:
	virtual void GetAABBSize ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );
	virtual BOOL IsDetectDivision ( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin );
};

#endif // !defined(AFX_DXEFFECTCLOTH_H__INCLUDED_)

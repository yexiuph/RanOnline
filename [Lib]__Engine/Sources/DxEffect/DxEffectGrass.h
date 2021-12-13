// DxEffectRain.h: interface for the DxEffectRain class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DXEFFECTGRASS_H__INCLUDED_)
#define AFX_DXEFFECTGRASS_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
 
#include "./DxEffectFrame.h"
#include <vector>

struct DxFrame;

struct GRASS_PROPERTY
{
	DWORD		m_dwFlags;
	float		m_fPower;
	D3DXVECTOR3	m_vMax;
	D3DXVECTOR3	m_vMin;
	D3DCOLOR	m_cColor;
	D3DXMATRIX	m_matWorld;
	char		m_szTexture[MAX_PATH];

	GRASS_PROPERTY::GRASS_PROPERTY() :
		m_dwFlags(0),
		m_fPower(2.0f),
		m_vMax( 0.f, 0.f, 0.f ),
		m_vMin( 0.f, 0.f, 0.f ),
		m_cColor(0xffffffff)
	{
		D3DXMatrixIdentity ( &m_matWorld );
		StringCchCopy( m_szTexture, MAX_PATH, "grass04.TGA" );
	}
};


class DxEffectGrass : public DxEffectBase
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
	static LPDIRECT3DSTATEBLOCK9	m_pSB;
	static LPDIRECT3DSTATEBLOCK9	m_pSB_SAVE;

	//	Note : 속성.
protected:
	union
	{
		struct
		{
			GRASS_PROPERTY	m_Property;
		};

		struct
		{
			DWORD		m_dwFlags;
			float		m_fPower;
			D3DXVECTOR3	m_vMax;
			D3DXVECTOR3	m_vMin;
			D3DCOLOR	m_cColor;
			D3DXMATRIX	m_matWorld;
			char		m_szTexture[MAX_PATH];
		};
	};

public:
	void SetProperty ( GRASS_PROPERTY& Property )
	{
		m_Property = Property;
	}
	GRASS_PROPERTY& GetProperty () { return m_Property; }

protected:
	float		m_fElapsedTime;
	float		m_fTime;

	LPDIRECT3DTEXTUREQ	m_pddsTexture;

	struct VERTEX
	{
		D3DXVECTOR3 vPos;
		D3DXVECTOR3 vNormal;
		D3DXVECTOR2 vTex;
	};

	struct GRASSVERTEX
	{
		D3DXVECTOR3 vPos;
		D3DXVECTOR3 vNormal;
		D3DCOLOR	cColor;
		D3DXVECTOR2 vTex;

		static const DWORD	FVF;
	};

	struct GRASS
	{
		D3DXVECTOR3	vPos;
		D3DXVECTOR3	vDirect;
		float		fWidth;
		float		fHeight;
		float		fRotate;
		float		fCatch;
		
		GRASS () :
			vPos(0,0,0),
			vDirect(0,0,0),
			fWidth(0),
			fHeight(0),
			fRotate(0),
			fCatch(0)
		{
		}
	};

	struct BASEGRASS
	{
		D3DXVECTOR3		vPos;
		GRASS*			pGrass;
		DWORD			dwObjects;
		float			fWidth;
		float			fHeight;
		float			fLength;

		BASEGRASS () :
			vPos(0,0,0),
			pGrass(NULL),
			dwObjects(30),
			fWidth(5.f),
			fHeight(5.f),
			fLength(100)
		{
		}

		~BASEGRASS ()
		{
			SAFE_DELETE_ARRAY(pGrass);
		}

		const BASEGRASS& operator = ( const BASEGRASS &value )
		{
			vPos		= value.vPos;
			pGrass		= new GRASS[value.dwObjects];
			memcpy ( pGrass, value.pGrass, value.dwObjects*sizeof(GRASS) );

			dwObjects	= value.dwObjects;
			fWidth		= value.fWidth;
			fHeight		= value.fHeight;
			fLength		= value.fLength;

			return *this;
		}

		void		SaveFile ( CSerialFile &SFile );
		void		LoadFile ( CSerialFile &SFile );
	};

	typedef std::vector<BASEGRASS*>		BASSVECTOR;
	typedef BASSVECTOR::iterator		BASSVECTOR_ITER;

	BASSVECTOR				m_vectorBass;

	DWORD					m_dwObjects;

	DxFrame					*m_pframeCur;

protected:
	struct SEPARATE_PROPERTY		// WORD 3개일 경우 사이즈가 안 맞게 나온다. 
	{
		D3DXVECTOR3			vMax;
		D3DXVECTOR3			vMin;
		DWORD				dwIndex;
	};

	class SEPARATEOBJ : public OBJAABB
	{
	public:										// 저장하는것
		union
		{
			struct
			{
				SEPARATE_PROPERTY	sSeparateProp;
			};
			struct
			{
				D3DXVECTOR3			vMax, vMin;
				DWORD				dwIndex;
			};
		};

		SEPARATEOBJ*		pNext;

		SEPARATEOBJ () :
			vMax(0,0,0),
			vMin(0,0,0),
			dwIndex(0)
		{
		}

		~SEPARATEOBJ ()
		{
			SAFE_DELETE(pNext);
		}

		virtual void GetAABBSize ( D3DXVECTOR3 &_vMax, D3DXVECTOR3 &_vMin )
		{
			_vMax = vMax; _vMin = vMin;
		}

		virtual BOOL IsDetectDivision ( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin )
		{
			return COLLISION::IsWithInPoint ( vDivMax, vDivMin, (vMax+vMin)/2 );
		}

		virtual OBJAABB* GetNext () { return pNext; }

		virtual HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV );

	public:
		void		SaveFile ( CSerialFile &SFile );
		void		LoadFile ( CSerialFile &SFile );

	};

	SEPARATEOBJ*		m_pSeparateList;
	OBJAABBNode*		m_pSeparateTree;

protected:
	static	DWORD*					m_pStaticIndies;
	static	DWORD					m_dwRenderObjects;
	static	LPDIRECT3DVERTEXBUFFERQ	m_pVB_1;
	static	LPDIRECT3DVERTEXBUFFERQ	m_pVB_2;
	static	LPDIRECT3DVERTEXBUFFERQ	m_pVB;
	static	LPDIRECT3DINDEXBUFFERQ	m_pIB;
	static	const	DWORD			MAX_OBJECTS;
	static	const	DWORD			MAX_BASE;
	static	const	float			MAX_ALPHA_LENGTH;
	static	const	float			MAX_SEE_LENGTH;

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
	HRESULT FrameMove ( float fTime, float fElapsedTime );	
	HRESULT Render ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan );

protected:
	void SeparateIndex ( D3DXVECTOR3* vMax, D3DXVECTOR3* vMin, DWORD dwIndex );

public:
	void	AddParticle ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 vPos, DWORD dwObjects, float fWidth, float fHeight, float fLength );
	void	DelParticle ( LPDIRECT3DDEVICEQ pd3dDevice, DWORD	dwIndex );

	D3DXVECTOR3	GetVector3 ( DWORD	dwIndex );
	DWORD		GetdwObjects ( DWORD	dwIndex );
	float		GetfWidth ( DWORD	dwIndex );
	float		GetfHeight ( DWORD	dwIndex );
	float		GetfLength ( DWORD	dwIndex );

	void		SetdwObjects ( DWORD dwIndex, DWORD dwObjects );
	void		SetfWidth ( DWORD	dwIndex, float fWidth );
	void		SetfHeight ( DWORD	dwIndex, float fHeight );
	void		SetfLength ( DWORD	dwIndex, float fLength );

	char*	GetStrList( DWORD dwIndex );
	DWORD	GetBassGrassNum ();

private:
	HRESULT SetBoundBox ( LPD3DXMESH pMesh );

	void	CreatePrivatePos ( BASEGRASS* pBass, DWORD dwObjects, float fWidth, float fHeight);
	BOOL	ReSetGrassHeight ( D3DXVECTOR3& pVertex );

	void	UpdateGrass ( LPDIRECT3DVERTEXBUFFERQ pVB );

public:
	DxEffectGrass();
	virtual ~DxEffectGrass();

	//Note: OBJAABB 부부분 
public:
	virtual void GetAABBSize( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );
	virtual BOOL IsDetectDivision( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin );

protected:
	virtual void SaveBuffer ( CSerialFile &SFile );
	virtual void LoadBufferSet ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual void LoadBuffer ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice );

	//	Note : DxFrameMesh 의 효과를 DxLandMan 에 삽입 할 때 대량의 버퍼 데이타를
	//		복제하기 위해서 사용한다.
public:
	virtual HRESULT CloneData ( DxEffectBase* pSrcEffect, LPDIRECT3DDEVICEQ pd3dDevice );

};

#endif // !defined(AFX_DXEFFECTGRASS_H__INCLUDED_)

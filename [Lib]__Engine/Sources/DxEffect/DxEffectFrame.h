#if !defined(_DXEFFECTFRAME_H__INCLUDED_)
#define _DXEFFECTFRAME_H__INCLUDED_

//	_EFF_REPLACE|_EFF_SINGLE -> _EFF_REPLACE_AFTER|_EFF_SINGLE_AFTER_0 -> _EFF_SINGLE_AFTER_1 -> _EFF_SINGLE_AFTER_2 -> _EFF_AFTER 
#define _EFF_REPLACE		(0x00000001)	// MultiTex, Neon, RenderState
#define _EFF_REPLACE_AFTER	(0x00000002)	// 바다, 
#define _EFF_SINGLE			(0x00000010)	// 타일링, 잔디
#define _EFF_SINGLE_AFTER_0	(0x00000020)	// 반사, 포자
#define _EFF_SINGLE_AFTER_1	(0x00000040)	// 그림자
#define _EFF_SINGLE_AFTER_2 (0x00000080)	// <비,눈,낙엽,반딧불>
#define _EFF_AFTER			(0x00000100)

inline DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }

struct DxFrame;
class CSerialFile;

struct DxEffFrameBase
{
	DxEffFrameBase() {}
	virtual ~DxEffFrameBase() {}
};

struct DxEffFrameBlur : public DxEffFrameBase
{
	D3DXMATRIX	matOldWorld;

	DxEffFrameBlur() {}
	virtual ~DxEffFrameBlur() {}
};

struct DxEffectType
{
	DWORD	TYPEID;
	char	NAME[MAX_PATH];

	DxEffectType *pNextType;

	DxEffectType() :
		TYPEID(0xffffffff),
		pNextType(NULL)
	{
		memset( NAME, 0, sizeof(char)*MAX_PATH );
	}

	~DxEffectType ()
	{
		SAFE_DELETE(pNextType);
	}
};

#include "DxMethods.h"
#include "Collision.h"

class	DxOctree;

class DxEffectBase : public DXAFFINEMATRIX, public OBJAABB
{
protected:
	DxFrame*		m_pAdaptFrame;
	char*			m_szAdaptFrame;

public:
	DxEffectBase *pEffectNext;			//	DxFrameMesh 에서의 전체 이펙트 리스트용.
	DxEffectBase *pLocalFrameEffNext;	//	DxFrame 에서의 해당 프레임 이펙트 리스트용.

public:
	virtual HRESULT AdaptToDxFrame ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice ) = 0;
	virtual HRESULT AdaptToEffList ( const DxFrame *const pFrame, LPDIRECT3DDEVICEQ pd3dDevice ) { return S_FALSE; }

public:
	DxFrame* GetAdaptFrame ()		{ return m_pAdaptFrame; }
	char* GetAdaptFrameName ()		{ return m_szAdaptFrame; }

	virtual void GetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD &dwVer ) { pProp=NULL, dwSize=0; dwVer=0xffffffff; }
	virtual void SetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD dwVer ) {}

public:
	virtual DWORD GetTypeID ()				{ return 0xffffffff; }
	virtual const char* GetName ()			{ return "[0]_정의되지않은_[UnDefined]"; }
	virtual DWORD GetFlag ()				{ return NULL; }
	virtual BOOL  GetEachRender()			{ return TRUE; }
	
public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime ) { return S_OK; }
	virtual HRESULT Render ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh = NULL, DxLandMan* pLandMan = NULL ) { return S_OK; }

public:
	virtual HRESULT OneTimeSceneInit() { return S_OK; };
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice ) { return S_OK; };
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice ) { return S_OK; };

public:
	virtual HRESULT InvalidateDeviceObjects () { return S_OK; };
	virtual HRESULT DeleteDeviceObjects () { return S_OK; };
	virtual HRESULT FinalCleanup() { return S_OK; };

public:
	virtual HRESULT Create ( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		if ( FAILED(OneTimeSceneInit()) )
			return E_FAIL;

		if ( FAILED(InitDeviceObjects(pd3dDevice)) )
			return E_FAIL;

		if ( FAILED(RestoreDeviceObjects(pd3dDevice)) )
			return E_FAIL;

		return S_OK;
	}

	virtual HRESULT CleanUp ()
	{
		InvalidateDeviceObjects();

		DeleteDeviceObjects();

		FinalCleanup();

		return S_OK;
	}

public:
	void	SaveSet ( CSerialFile &SFile );
	void	SaveFile ( CSerialFile &SFile );
	
public:
	virtual void SaveBuffer ( CSerialFile &SFile );
	virtual void LoadBufferSet ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice );
	virtual void LoadBuffer ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice );

	//	Note : DxFrameMesh 의 효과를 DxLandMan 에 붓일때 대량의 버퍼 데이타를
	//		복제하기 위해서 사용한다.
public:
	virtual HRESULT CloneData ( DxEffectBase* pSrcEffect, LPDIRECT3DDEVICEQ pd3dDevice ) { return S_FALSE; }

public:
	DxEffectBase() :
		m_pAdaptFrame(NULL),
		m_szAdaptFrame(NULL),
		pLocalFrameEffNext(NULL),
		pEffectNext(NULL)
	{
		UseAffineMatrix ();
	}

	virtual ~DxEffectBase()
	{
		SAFE_DELETE_ARRAY(m_szAdaptFrame);
		SAFE_DELETE(pEffectNext);
	}

public:
	virtual void GetAABBSize ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin )
	{
		vMax = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);
		vMin = D3DXVECTOR3(-FLT_MAX,-FLT_MAX,-FLT_MAX);
	}

	virtual BOOL IsDetectDivision ( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin ) { return FALSE; }

	virtual OBJAABB* GetNext ()
	{
		return pEffectNext;
	}
};

typedef DxEffectBase* PDXEFFECTBASE;

#endif // !defined(_DXEFFECTFRAME_H__INCLUDED_)
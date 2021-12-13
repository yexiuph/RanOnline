#pragma once
#include "DxEffSinglePropGMan.h"

#include "basestream.h"
#include "DxMaterial.h"
#include "DxSkinEffResult.h"

struct DxCharPart;
#include "DxSkinPieceContainer.h"
//struct DxSkinPiece;
//struct DxSkeleton;
//class DxSkinAniControl;

template < class T > class CMinMax
{
public:
	CMinMax()				
	{	
		min = T();	
		max = T();	
	}
	CMinMax( T min, T max ) 
	{	
		min = min;	
		max = max;	
	}

	virtual ~CMinMax() { }

public:
	T min;
	T max;


	T GetRange()
	{
		return abs( max - min ); 
	}	
};


enum EFFCHAR_TYPES
{
	EMEFFCHAR_SINGLE		= 0,
	EMEFFCHAR_BLUR			= 1,
	EMEFFCHAR_CLONEBLUR		= 2,
	EMEFFCHAR_DUST			= 3,
	EMEFFCHAR_SHOCK			= 4,
	EMEFFCHAR_ATTRIBUTE		= 5,
	EMEFFCHAR_ARROW			= 6,
	EMEFFCHAR_SPECULAR		= 7,
	EMEFFCHAR_LEVEL			= 8,	//	EMEFFCHAR_SPECULAR2
	EMEFFCHAR_EMIT			= 9,
	EMEFFCHAR_ALPHA			= 10,
	EMEFFCHAR_NEON			= 11,
	EMEFFCHAR_MARK			= 12,
	EMEFFCHAR_NOALPHA		= 13,
	EMEFFCHAR_REFLECTION2	= 14,
	EMEFFCHAR_AMBIENT		= 15,
	EMEFFCHAR_DOT3			= 16,
	EMEFFCHAR_MULTITEX		= 17,
	EMEFFCHAR_GHOSTING		= 18,
	EMEFFCHAR_SPECULAR2		= 19,	//	EMEFFCHAR_LEVEL
	EMEFFCHAR_TOON			= 20,
	EMEFFCHAR_TEXDIFF		= 21,
	EMEFFCHAR_PARTICLE		= 22,
	EMEFFCHAR_BONEPOSEFF	= 23,
	EMEFFCHAR_BONELISTEFF	= 24,
	EMEFFCHAR_USERCOLOR		= 25,
	EMEFFCHAR_NORMALMAP		= 26,
	EMEFFCHAR_LINE2BONE		= 27,
	EMEFFCHAR_AROUNDEFFECT	= 28,
};

enum EMEFFCHAR_FLAGS			// 내가 누구인지 확실히 알려야 한다.
{
	EMECF_SPECULAR		= 0x00001,
	EMECF_DOT3			= 0x00002,
	EMECF_LEVEL			= 0x00004,
	EMECF_TOON			= 0x00008,
	EMECF_AMBIENT		= 0x00010,
	EMECF_NOALPHA		= 0x00020,
	EMECF_ALPHA			= 0x00040,
	EMECF_SHOCK			= 0x00080,
	EMECF_TEXDIFF		= 0x00100,
	EMECF_NORMALMAP		= 0x00200,
	EMECF_SPECULAR2		= 0x00400,	// MulpiPass Rendering
	EMECF_RELFECT		= 0x00800,	// MulpiPass Rendering

	EMECF_NONHLSL		= (EMECF_SPECULAR2|EMECF_RELFECT),	// EMECF_NORMALMAP 랑 같이 쓰면 안되는 것.~!!!								
	EMECF_ORGINNON		= (EMECF_SPECULAR|EMECF_DOT3|EMECF_TOON),	// 원본을 뿌리지 않고 내가 원본이 된다.
};

enum EMCHAR_FLAG				// Flag Save, Load
{
	EMCF_SHADOW	= 0x00001,
};

enum CRDT_TYPE					// 이젠 쓸모 없는 거 같다.
{
	CRDT_DEFAULT		= 0,
	CRDT_SPECULAR		= 1,
	CRDT_DOT3_SPECULAR	= 2,
	CRDT_DOT3_REFLECT	= 3,
	CRDT_GLOW			= 4,
};

enum EMEFFCHAR_STATE_ORDER		// State 변화 할때 보통 쓰임
{
	//	renderstate 조정 순서에 따라 정렬.
	EMEFFSO_SHOCK		= 1,
	EMEFFSO_NOALPHA		= 2,
	EMEFFSO_ALPHA		= 3,
	EMEFFSO_MARK		= 4,
	EMEFFSO_AMBIENT		= 5,
	EMEFFSO_TOON		= 6,
	
	EMEFFSO_RENDERSTATE	= 6,	//	여기까지 renderstate 관련 효과.

	EMEFFSO_SINGLE_EFF	= 11,
	EMEFFSO_GLOW		= 12,
	EMEFFSO_GHOSTING	= 13,
	EMEFFSO_NORMAL		= 14
};

struct EFFCHAR_PROPERTY
{
};

class DxEffChar
{
public:
	enum
	{
		RF_DIE	= 0x0010,
	};

public:
	virtual DWORD GetTypeID () = 0;
	virtual DWORD GetFlag () = 0;
	virtual const char* GetName () = 0;

protected:
	DWORD			m_dwRunFlag;
	DWORD			m_dwFlag;
	std::string		m_strFileName;

	float			m_fAge;

	DxCharPart*		m_pCharPart;
	DxSkinPiece*	m_pSkinPiece;

	DWORD				m_dwKeyTime;
	EMANI_MAINTYPE		m_CurAniMType;
	EMANI_SUBTYPE		m_CurAniSType;
	DxSkinAniControl*	m_pSkinAniControl;
	DxSkeleton*			m_pSkeleton;
	D3DXMATRIX			m_matCurAni;

public:
	void SetEnd ()										{ m_dwRunFlag |= RF_DIE; }
	void SetFileName ( std::string strFileName )		{ m_strFileName = strFileName; }
	std::string GetFileName ()							{ return m_strFileName; }

public:
	BOOL IsEnd ()	{ return (m_dwRunFlag&RF_DIE); }

public:
	virtual HRESULT OneTimeSceneInit () { return S_OK; }
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice ) { return S_OK; }
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice ) { return S_OK; }
	virtual HRESULT InvalidateDeviceObjects () { return S_OK; }
	virtual HRESULT DeleteDeviceObjects () { return S_OK; }
	virtual HRESULT FinalCleanup () { return S_OK; }

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime ) = 0;
	virtual void	Render( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPieceRender=FALSE )							{}
	virtual void	RenderEff( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMATRIX pMatrix, const float fScale )		{}
	virtual void	RenderEDIT( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMATRIX pMatrix, const float fScale )	{}

public:
	virtual DWORD GetStateOrder () const														{ return EMEFFSO_NORMAL; }
	virtual HRESULT	SettingState ( LPDIRECT3DDEVICEQ pd3dDevice, SKINEFFDATA& sSKINEFFDATA )	{ return S_OK; }
	virtual HRESULT	RestoreState ( LPDIRECT3DDEVICEQ pd3dDevice )								{ return S_OK; }

	virtual void ConvertTracePoint()	{}

public:
	virtual void SetProperty ( EFFCHAR_PROPERTY *pProperty ) = 0;
	virtual EFFCHAR_PROPERTY* GetProperty () = 0;

	virtual void SetMaterials ( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwMaterials, DXMATERIAL_CHAR_EFF* pMaterial )	{}
	virtual DXMATERIAL_CHAR_EFF*	GetMaterials ()					{ return NULL; }

	void SetCurrentAni ( EMANI_MAINTYPE MType, EMANI_SUBTYPE SType, DWORD dwKeyTime )
	{
		m_CurAniMType	= MType;
		m_CurAniSType	= SType;
		m_dwKeyTime		= dwKeyTime;
	}

	void SetAniControl( DxSkinAniControl* _pSkinAniControl, const D3DXMATRIX &_matCurAni )
	{
		m_pSkinAniControl = _pSkinAniControl;
		m_matCurAni = _matCurAni;
	}

	void SetAniControl( DxSkinAniControl* _pSkinAniControl )
	{
		m_pSkinAniControl = _pSkinAniControl;
	}

	virtual void SetSkeleton( DxSkeleton* pSkeleton ) { m_pSkeleton = pSkeleton; }

	virtual DxEffChar* CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece ) = 0;
	
	virtual void SetDirection ( D3DXVECTOR3 *pDir, float fTarDir ) { };
	virtual void SetLinkObj ( DxCharPart* pCharPart, DxSkinPiece* pSkinPiece )
	{
		m_pCharPart = pCharPart;
		m_pSkinPiece = pSkinPiece;
	}

	virtual HRESULT LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice ) = 0;
	virtual HRESULT SaveFile ( basestream &SFile ) = 0;

public:
	HRESULT Create ( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		if ( FAILED(OneTimeSceneInit()) )				return E_FAIL;
		if ( FAILED(InitDeviceObjects(pd3dDevice)) )	return E_FAIL;
		if ( FAILED(RestoreDeviceObjects(pd3dDevice)) )	return E_FAIL;

		return S_OK;
	}

	HRESULT CleanUp ()
	{
		InvalidateDeviceObjects();
		DeleteDeviceObjects();
		FinalCleanup();

		return S_OK;
	}

public:
	DxEffChar(void);
	virtual ~DxEffChar(void);
};

class DXEFFCHAR_OPER
{
public:
	bool operator() ( const DxEffChar* lvalue, const DxEffChar* rvalue )
	{
		return lvalue->GetStateOrder() < rvalue->GetStateOrder();
	}

	bool operator() ( const DxEffChar* lvalue, const DWORD dwORDER )
	{
		return lvalue->GetStateOrder() < dwORDER;
	}
};

class DXEFFCHAR_DISOPER
{
public:
	bool operator() ( const DxEffChar* lvalue, const DxEffChar* rvalue )
	{
		return lvalue->GetStateOrder() > rvalue->GetStateOrder();
	}

	bool operator() ( const DxEffChar* lvalue, const DWORD dwORDER )
	{
		return lvalue->GetStateOrder() > dwORDER;
	}
};

struct DxEffCharType
{
	DWORD	TYPEID;
	char	NAME[MAX_PATH];

	DxEffCharType *pNext;

	DxEffCharType() :
		TYPEID(0xffffffff),
		pNext(NULL)
	{
		memset( NAME, 0, sizeof(char)*MAX_PATH );
	}

	~DxEffCharType ()
	{
		SAFE_DELETE(pNext);
	}
};

class DxEffCharMan
{
protected:
	void RegistType ( const DWORD TypeID, const char *Name );

protected:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;

public:
	D3DXVECTOR3			m_vViewMax; 
	D3DXVECTOR3			m_vViewMin;
	D3DXVECTOR3&		GetViewMax ()	{ return m_vViewMax; }
	D3DXVECTOR3&		GetViewMin ()	{ return m_vViewMin; }
	
protected:
	VOID				SetViewBoard ();

protected:
	DxEffCharType*		m_pEffectTypeList;

public:
	DxEffCharType* GetEffectList ()	{ return m_pEffectTypeList; }

public:
	DxEffChar*	CreateEffInstance ( DWORD TypeID );
	DxEffChar*	CreateEffect ( DWORD TypeID, EFFCHAR_PROPERTY* pProp, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece );
	void		DeleteEffect ( DxEffChar* pEffChar );
	DxEffChar*  AddDefaultAmbient ( DxCharPart* pCharPart, DxSkinPiece* pSkinPiece );

public:
	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RestoreDeviceObjects ();
	HRESULT InvalidateDeviceObjects ();
	HRESULT DeleteDeviceObjects();

protected:
	DxEffCharMan ();

public:
	~DxEffCharMan ();

public:
	static DxEffCharMan& GetInstance();
};
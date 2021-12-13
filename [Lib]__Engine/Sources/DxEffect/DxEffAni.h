#pragma once
#include "basestream.h"

#include <vector>

class DxEffAni;
class DxSkinChar;
class CSerialFile;
struct SAnimContainer;

enum EFFANI_TYPES
{
	EMEFFANI_SINGLE		= 0,
	EMEFFANI_GHOSTING	= 1,
	EMEFFANI_TRACE		= 2,
	EMEFFANI_FACEOFF	= 3,
};

struct EFFANI_PROPERTY
{
};

//--------------------------------------------------------------------------------------------------------------
//	DxEffAniData : 저장하는 되는 값.
//--------------------------------------------------------------------------------------------------------------
class DxEffAniData
{
protected:
	LPDIRECT3DDEVICEQ m_pd3dDevice;

public:
	virtual DWORD GetTypeID ()		= 0;
	virtual DWORD GetFlag ()		= 0;
	virtual const TCHAR* GetName ()	= 0;

public:
	virtual BOOL CheckEff( DWORD dwCurKeyTime, int nPrevFrame )	{ return FALSE; }

public:
	virtual void SetProperty ( EFFANI_PROPERTY *pProperty ) = 0;
	virtual EFFANI_PROPERTY* GetProperty () = 0;
	virtual void SetEffAniData ( DxEffAniData*	pData );

public:	
	virtual DxEffAni* NEWOBJ( SAnimContainer* pAnimContainer ) = 0;
	virtual HRESULT LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice ) = 0;
	virtual HRESULT SaveFile ( CSerialFile &SFile ) = 0;

public:
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )	{ return S_OK; }

	virtual HRESULT InvalidateDeviceObjects ()								{ return S_OK; }
	virtual HRESULT DeleteDeviceObjects ();

public:
	virtual HRESULT Create(LPDIRECT3DDEVICEQ pd3dDevice)
	{
		if ( FAILED(InitDeviceObjects(pd3dDevice)) )		return E_FAIL;
		if ( FAILED(RestoreDeviceObjects(pd3dDevice)) )		return E_FAIL;
		return S_OK;
	}

	virtual HRESULT CleanUp ()
	{
		InvalidateDeviceObjects();
		DeleteDeviceObjects();
		return S_OK;
	}

public:
	DxEffAniData() :
		m_pd3dDevice (NULL)
	{
	}

	~DxEffAniData()
	{
		m_pd3dDevice = NULL;
	}
};

class DxEffAni
{
public:
	enum
	{
		RF_DIE	= 0x0010,
	};

protected:
	DWORD			m_dwRunFlag;
	DWORD			m_dwFlag;
	std::string		m_strFileName;

	float			m_fAge;

public:
	BOOL			m_bFrameMove;			// 자신의 애니메이션일 경우만 효과를 생성가능 하도록 한다.
	SAnimContainer* m_rAnimContainer;

public:
	void SetEnd ()										{ m_dwRunFlag |= RF_DIE; }
	void SetFileName ( std::string strFileName )		{ m_strFileName = strFileName; }
	std::string GetFileName ()							{ return m_strFileName; }
	void CheckFrameMove( SAnimContainer* pAnimContainer );

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
	virtual HRESULT FrameMove( float fTime, float fElapsedTime ) = 0;
	virtual HRESULT	Render( const LPDIRECT3DDEVICEQ pd3dDevice, DxSkinChar* pSkinChar, const D3DXMATRIX& matCurPos ) = 0;

public:
	virtual HRESULT	SettingState ( LPDIRECT3DDEVICEQ pd3dDevice )	{ return S_OK; }
	virtual HRESULT	RestoreState ( LPDIRECT3DDEVICEQ pd3dDevice )	{ return S_OK; }

public:
	virtual BOOL IsHeadDrawChild()		{ return FALSE; }

public:
	HRESULT Create ( LPDIRECT3DDEVICEQ pd3dDevice, SAnimContainer* pAnimContainer )
	{
		m_rAnimContainer = pAnimContainer;

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
	DxEffAni(void);
	virtual ~DxEffAni(void);
};

struct DxEffAniType
{
	DWORD	TYPEID;
	TCHAR	NAME[MAX_PATH];

	DxEffAniType *pNext;

	DxEffAniType() :
		TYPEID(0xffffffff),
		pNext(NULL)
	{
		memset( NAME, 0, sizeof(TCHAR)*MAX_PATH );
	}

	~DxEffAniType ()
	{
		SAFE_DELETE(pNext);
	}
};

class DxEffAniMan
{
protected:
	void RegistType ( const DWORD TypeID, const TCHAR *Name );

protected:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;

protected:
	DxEffAniType*		m_pEffectTypeList;

public:
	DxEffAniType* GetEffectList ()	{ return m_pEffectTypeList; }

public:
	DxEffAniData*	CreateEffInstance ( DWORD TypeID );
	DxEffAniData*	CreateEffect ( DWORD TypeID, EFFANI_PROPERTY* pProp );

	LPDIRECT3DDEVICEQ	GetDirect3DDevice8 () { return m_pd3dDevice; }

public:
	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RestoreDeviceObjects ();
	HRESULT InvalidateDeviceObjects ();
	HRESULT DeleteDeviceObjects();

protected:
	DxEffAniMan ();

public:
	~DxEffAniMan ();

public:
	static DxEffAniMan& GetInstance();
};
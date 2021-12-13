#pragma once

struct SKINEFFDATA;
class DxEffKeep;
class basestream;
class CSerialFile;

#include <list>

enum EFFKEEP_TYPES
{
	EFFKT_CT_EDGE	= 0,
	EFFKT_CT_SHADE	= 1,
	EFFKT_CT_COLOR	= 2,
};

struct KEEP_COLORARGB
{
	DWORD dwColor;				// SAVE // LOAD 
	WORD  wA, wR, wG, wB;
};

struct EFFKEEP_PROPERTY
{
};

//--------------------------------------------------------------------------------------------------------------
//	DxEffKeepData : 저장하는 되는 값.
//--------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//							D	x		E	f	f		K	e	e	p		D	a	t	a
//----------------------------------------------------------------------------------------------------------------------
class DxEffKeepData
{
protected:
	LPDIRECT3DDEVICEQ m_pd3dDevice;

public:
	virtual DWORD GetTypeID ()		= 0;
	virtual DWORD GetFlag ()		= 0;
	virtual const char* GetName ()	= 0;

public:
	virtual BOOL CheckEff( DWORD dwCurKeyTime, int nPrevFrame )	{ return FALSE; }

public:
	virtual void SetProperty ( EFFKEEP_PROPERTY *pProperty ) = 0;
	virtual EFFKEEP_PROPERTY* GetProperty () = 0;
	virtual void SetEffAniData ( DxEffKeepData*	pData );

public:	
	virtual DxEffKeep* NEWOBJ() = 0;
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
	DxEffKeepData() :
		m_pd3dDevice (NULL)
	{
	}

	~DxEffKeepData()
	{
		m_pd3dDevice = NULL;
	}
};

//----------------------------------------------------------------------------------------------------------------------
//					D	x		E	f	f		K	e	e	p		D	a	t	a		M	a	i	n
//----------------------------------------------------------------------------------------------------------------------
class DxEffKeepDataMain
{
private:
	static DWORD VERSION;

public:
	std::string		m_strFileName;

public:
	typedef std::list<DxEffKeepData*>	LIST_EFFKEEPDATA;
	typedef LIST_EFFKEEPDATA::iterator	LIST_EFFKEEPDATA_ITER;

	LIST_EFFKEEPDATA	m_listEffKeepData;

	void Insert( DxEffKeepData* pEff );
	void Delete( DxEffKeepData* pEff );
	void CleanUp();


public:
	void Save( const TCHAR* pName );
	void Load( const TCHAR* pName, LPDIRECT3DDEVICEQ pd3dDevice );

private:
	void Save( CSerialFile &SFile );
	void Load( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );

public:
	DxEffKeepDataMain();
	~DxEffKeepDataMain();
};

//----------------------------------------------------------------------------------------------------------------------
//										D	x		E	f	f		K	e	e	p	
//----------------------------------------------------------------------------------------------------------------------
class DxEffKeep
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
	virtual HRESULT FrameMove( float fElapsedTime, SKINEFFDATA& sSKINEFFDATA ) = 0;
	virtual HRESULT	Render( const LPDIRECT3DDEVICEQ pd3dDevice ) = 0;

public:
	virtual HRESULT	SettingState ( LPDIRECT3DDEVICEQ pd3dDevice )	{ return S_OK; }
	virtual HRESULT	RestoreState ( LPDIRECT3DDEVICEQ pd3dDevice )	{ return S_OK; }

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
	DxEffKeep(void);
	virtual ~DxEffKeep(void);
};

//----------------------------------------------------------------------------------------------------------------------
//							D	x		E	f	f		K	e	e	p		M	a	i	n
//----------------------------------------------------------------------------------------------------------------------
class DxEffKeepMain
{
public:
	std::string		m_strFileName;

public:
	typedef std::list<DxEffKeep*>	LIST_EFFKEEP;
	typedef LIST_EFFKEEP::iterator	LIST_EFFKEEP_ITER;

	LIST_EFFKEEP	m_listEffKeep;

	void FrameMove( float fElapsedTime, SKINEFFDATA& sSKINEFFDATA );
	void Render( LPDIRECT3DDEVICEQ pd3dDevice );

	void Convert( DxEffKeepDataMain* pEff );

public:
	DxEffKeepMain(void);
	~DxEffKeepMain(void);
};

//----------------------------------------------------------------------------------------------------------------------
//							D	x		E	f	f		K	e	e	p		T	y	p	e
//----------------------------------------------------------------------------------------------------------------------
struct DxEffKeepType
{
	DWORD	TYPEID;
	char	NAME[MAX_PATH];

	DxEffKeepType *pNext;

	DxEffKeepType() :
		TYPEID(0xffffffff),
		pNext(NULL)
	{
		memset( NAME, 0, sizeof(char)*MAX_PATH );
	}

	~DxEffKeepType ()
	{
		SAFE_DELETE(pNext);
	}
};

//----------------------------------------------------------------------------------------------------------------------
//							D	x		E	f	f		K	e	e	p		M	a	n
//----------------------------------------------------------------------------------------------------------------------
class DxEffKeepMan
{
protected:
	void RegistType ( const DWORD TypeID, const char *Name );

protected:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;

protected:
	DxEffKeepType*		m_pEffectTypeList;

public:
	DxEffKeepType* GetEffectList ()	{ return m_pEffectTypeList; }

public:
	DxEffKeepData*	CreateEffInstance ( DWORD TypeID );
	DxEffKeepData*	CreateEffect ( DWORD TypeID, EFFKEEP_PROPERTY* pProp );

	LPDIRECT3DDEVICEQ	GetDirect3DDevice () { return m_pd3dDevice; }

public:
	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RestoreDeviceObjects ();
	HRESULT InvalidateDeviceObjects ();
	HRESULT DeleteDeviceObjects();

protected:
	DxEffKeepMan ();

public:
	~DxEffKeepMan ();

public:
	static DxEffKeepMan& GetInstance();
};
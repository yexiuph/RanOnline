#include "pch.h"
#include <algorithm>
#include "./DxEffKeep.h"
#include "./DxEffKeepCTColor.h"
#include "./DxEffKeepCTEdge.h"
#include "./DxEffKeepCTShade.h"

#include "./DxEffcharData.h"

#include "./SerialFile.h"
#include "./StlFunctions.h"

#include "./GLogic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//----------------------------------------------------------------------------------------------------------------------
//							D	x		E	f	f		K	e	e	p		D	a	t	a
//----------------------------------------------------------------------------------------------------------------------
HRESULT DxEffKeepData::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice; 
	return S_OK; 
}

HRESULT DxEffKeepData::DeleteDeviceObjects ()
{
	m_pd3dDevice = NULL; 
	return S_OK; 
}

void DxEffKeepData::SetEffAniData ( DxEffKeepData*	pData )
{
	m_pd3dDevice = pData->m_pd3dDevice;
}

//----------------------------------------------------------------------------------------------------------------------
//					D	x		E	f	f		K	e	e	p		D	a	t	a		M	a	i	n
//----------------------------------------------------------------------------------------------------------------------
DWORD DxEffKeepDataMain::VERSION = 0x100;

DxEffKeepDataMain::DxEffKeepDataMain()
{
	m_strFileName = "";
}

DxEffKeepDataMain::~DxEffKeepDataMain()
{
	CleanUp();
}

void DxEffKeepDataMain::CleanUp()
{
	std::for_each( m_listEffKeepData.begin(), m_listEffKeepData.end(), std_afunc::DeleteObject() );
	m_listEffKeepData.clear();

	m_strFileName = "";
}

void DxEffKeepDataMain::Insert( DxEffKeepData* pEff )
{
	m_listEffKeepData.push_back( pEff );
}

void DxEffKeepDataMain::Delete( DxEffKeepData* pEff )
{
}

void DxEffKeepDataMain::Save( const TCHAR* pName )
{
	m_strFileName = pName;

	std::string strPathName;
	strPathName = DxEffcharDataMan::GetInstance().GetPath ();
	strPathName += pName;

	CSerialFile SFile;
	BOOL bOk = SFile.OpenFile( FOT_WRITE, strPathName.c_str() );
	if ( !bOk )		return;

	Save( SFile );

	SFile.CloseFile();
}

void DxEffKeepDataMain::Load( const TCHAR* pName, LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_strFileName = pName;

	std::string strPathName;
	strPathName = DxEffcharDataMan::GetInstance().GetPath ();
	strPathName += pName;

	std::auto_ptr<basestream> pBStream( GLOGIC::openfile_basestream(GLOGIC::bENGLIB_ZIPFILE, 
																	GLOGIC::strEFFECT_CHAR_ZIPFILE.c_str(),
																	strPathName.c_str(), 
																	pName ) );

	if ( !pBStream.get() )
		return;

	basestream &SFile = *pBStream;

	Load( SFile, pd3dDevice );
}

void DxEffKeepDataMain::Save( CSerialFile &SFile )
{
	SFile << VERSION;

	SFile << (DWORD)m_listEffKeepData.size();
	LIST_EFFKEEPDATA_ITER iter = m_listEffKeepData.begin();
	for( ; iter!=m_listEffKeepData.end(); ++iter )
	{
		SFile << (DWORD) (*iter)->GetTypeID();
		(*iter)->SaveFile( SFile );
	}
}

void DxEffKeepDataMain::Load( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwVer;
	SFile >> dwVer;

	DWORD	dwSize;
	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; ++i )
	{
		DWORD	dwType;
		SFile >> dwType;
		DxEffKeepData* pEff = DxEffKeepMan::GetInstance().CreateEffInstance ( dwType );
		if ( pEff )
		{
			pEff->LoadFile( SFile, pd3dDevice );
			m_listEffKeepData.push_back ( pEff );
		}
	}

}

//----------------------------------------------------------------------------------------------------------------------
//										D	x		E	f	f		K	e	e	p	
//----------------------------------------------------------------------------------------------------------------------
DxEffKeep::DxEffKeep(void) :
	m_dwRunFlag(NULL),
	m_dwFlag(NULL),
	m_fAge(0.0f)
{
}

DxEffKeep::~DxEffKeep(void)	
{
	CleanUp ();
}

//----------------------------------------------------------------------------------------------------------------------
//							D	x		E	f	f		K	e	e	p		M	a	i	n
//----------------------------------------------------------------------------------------------------------------------
DxEffKeepMain::DxEffKeepMain(void)
{
}

DxEffKeepMain::~DxEffKeepMain(void)
{
	std::for_each( m_listEffKeep.begin(), m_listEffKeep.end(), std_afunc::DeleteObject() );
	m_listEffKeep.clear();
}

void DxEffKeepMain::FrameMove( float fElapsedTime, SKINEFFDATA& sSKINEFFDATA )
{
	LIST_EFFKEEP_ITER iter = m_listEffKeep.begin();
	for( ; iter!=m_listEffKeep.end(); ++iter )
	{
		(*iter)->FrameMove( fElapsedTime, sSKINEFFDATA );
	}
}

void DxEffKeepMain::Render( LPDIRECT3DDEVICEQ pd3dDevice )
{
	LIST_EFFKEEP_ITER iter = m_listEffKeep.begin();
	for( ; iter!=m_listEffKeep.end(); ++iter )
	{
		(*iter)->Render( pd3dDevice );
	}
}

void DxEffKeepMain::Convert( DxEffKeepDataMain* pEff )
{
	m_strFileName = pEff->m_strFileName.c_str();

	DxEffKeepDataMain::LIST_EFFKEEPDATA_ITER iterDATA = pEff->m_listEffKeepData.begin();
	for( ; iterDATA!=pEff->m_listEffKeepData.end(); ++iterDATA )
	{
		DxEffKeep* pNew = (*iterDATA)->NEWOBJ();
		m_listEffKeep.push_back( pNew );
	}
}

//----------------------------------------------------------------------------------------------------------------------
//							D	x		E	f	f		K	e	e	p		M	a	n
//----------------------------------------------------------------------------------------------------------------------
DxEffKeepMan& DxEffKeepMan::GetInstance()
{
	static DxEffKeepMan Instance;
	return Instance;
}

DxEffKeepMan::DxEffKeepMan ()
{
	RegistType( DxEffKeepData_CTEdge::TYPEID,		DxEffKeepData_CTEdge::NAME );
	RegistType( DxEffKeepData_CTShade::TYPEID,		DxEffKeepData_CTShade::NAME );
	RegistType( DxEffKeepData_CTColor::TYPEID,		DxEffKeepData_CTColor::NAME );
}

DxEffKeepMan::~DxEffKeepMan ()
{
	SAFE_DELETE(m_pEffectTypeList);
}

void DxEffKeepMan::RegistType ( const DWORD TypeID, const char *Name )
{
	GASSERT(Name);

	DxEffKeepType *pCurEff = m_pEffectTypeList;
	while(pCurEff)
	{
		GASSERT ( pCurEff->TYPEID != TypeID && "TypeID 중복 등록될수 없습니다." );
		GASSERT ( strcmp(pCurEff->NAME,Name) && "NAME가 중복 등록될수 없습니다." );

		pCurEff = pCurEff->pNext;
	}

	//	Note : 새 노드 생성후 대입.
	//
	DxEffKeepType *pNewType = new DxEffKeepType;
	pNewType->TYPEID = TypeID;
	StringCchCopy( pNewType->NAME, MAX_PATH, Name );

	//	Note : 리스트에 등록.
	//
	pNewType->pNext = m_pEffectTypeList;
	m_pEffectTypeList = pNewType;
}

DxEffKeepData* DxEffKeepMan::CreateEffInstance ( DWORD TypeID )
{
	switch ( TypeID )
	{
	case EFFKT_CT_EDGE:
		return new DxEffKeepData_CTEdge;
	case EFFKT_CT_SHADE:
		return new DxEffKeepData_CTShade;
	case EFFKT_CT_COLOR:
		return new DxEffKeepData_CTColor;
	};

	return NULL;
}

DxEffKeepData* DxEffKeepMan::CreateEffect ( DWORD TypeID, EFFKEEP_PROPERTY* pProp )
{
	GASSERT(m_pd3dDevice);

	HRESULT hr = S_OK;

	DxEffKeepData* pEffProp = CreateEffInstance ( TypeID );
	if ( !pEffProp )	return NULL;

	pEffProp->SetProperty ( pProp );
	hr = pEffProp->Create ( m_pd3dDevice );
	if ( FAILED(hr) )
	{
		SAFE_DELETE(pEffProp);
		return NULL;
	}

	return pEffProp;
}

HRESULT DxEffKeepMan::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	return S_OK;
}

HRESULT DxEffKeepMan::RestoreDeviceObjects ()
{
	DxEffKeepCTEdge::CreateDevice( m_pd3dDevice );

	return S_OK;
}

HRESULT DxEffKeepMan::InvalidateDeviceObjects ()
{
	DxEffKeepCTEdge::ReleaseDevice( m_pd3dDevice );

	return S_OK;
}

HRESULT DxEffKeepMan::DeleteDeviceObjects()
{
	m_pd3dDevice = NULL;

	return S_OK;
}
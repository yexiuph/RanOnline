#include "pch.h"

#include "./DxEffAni.h"

#include "./DxEffAniSingle.h"
#include "./DxEffAniGhosting.h"
#include "./DxEffAniTrace.h"
#include "./DxEffAniFaceOff.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//	-------------------------------------------		---------------------------------------------------------------

HRESULT DxEffAniData::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice; 
	return S_OK; 
}

HRESULT DxEffAniData::DeleteDeviceObjects ()
{
	m_pd3dDevice = NULL; 
	return S_OK; 
}

void DxEffAniData::SetEffAniData ( DxEffAniData*	pData )
{
	m_pd3dDevice = pData->m_pd3dDevice;
}

//	-------------------------------------------		---------------------------------------------------------------

DxEffAni::DxEffAni(void) :
	m_dwRunFlag(NULL),
	m_dwFlag(NULL),
	m_fAge(0.0f),
	m_bFrameMove(FALSE),
	m_rAnimContainer(NULL)
{
}

DxEffAni::~DxEffAni(void)	
{
	m_rAnimContainer = NULL;

	CleanUp ();
}

void DxEffAni::CheckFrameMove( SAnimContainer* pAnimContainer )
{
	if( m_rAnimContainer==pAnimContainer )	m_bFrameMove = TRUE;
	else									m_bFrameMove = FALSE;
}

//	-------------------------------------------		---------------------------------------------------------------

DxEffAniMan& DxEffAniMan::GetInstance()
{
	static DxEffAniMan Instance;
	return Instance;
}

DxEffAniMan::DxEffAniMan ()
{
	RegistType ( DxEffAniData_Single::TYPEID,		DxEffAniData_Single::NAME );
	RegistType ( DxEffAniData_Ghosting::TYPEID,		DxEffAniData_Ghosting::NAME );
	RegistType ( DxEffAniData_Trace::TYPEID,		DxEffAniData_Trace::NAME );
	RegistType ( DxEffAniData_FaceOff::TYPEID,		DxEffAniData_FaceOff::NAME );
}

DxEffAniMan::~DxEffAniMan ()
{
	SAFE_DELETE(m_pEffectTypeList);
}

void DxEffAniMan::RegistType ( const DWORD TypeID, const TCHAR *Name )
{
	GASSERT(Name);

	DxEffAniType *pCurEff = m_pEffectTypeList;
	while(pCurEff)
	{
		GASSERT ( pCurEff->TYPEID != TypeID && _T("TypeID 중복 등록될수 없습니다.") );
		GASSERT ( strcmp(pCurEff->NAME,Name) && _T("NAME가 중복 등록될수 없습니다.") );

		pCurEff = pCurEff->pNext;
	}

	//	Note : 새 노드 생성후 대입.
	//
	DxEffAniType *pNewType = new DxEffAniType;
	pNewType->TYPEID = TypeID;
	StringCchCopy( pNewType->NAME, MAX_PATH, Name );

	//	Note : 리스트에 등록.
	//
	pNewType->pNext = m_pEffectTypeList;
	m_pEffectTypeList = pNewType;
}

DxEffAniData* DxEffAniMan::CreateEffInstance ( DWORD TypeID )
{
	switch ( TypeID )
	{
	case EMEFFANI_SINGLE:
		return new DxEffAniData_Single;
	case EMEFFANI_GHOSTING:
		return new DxEffAniData_Ghosting;
	case EMEFFANI_TRACE:
		return new DxEffAniData_Trace;
	case EMEFFANI_FACEOFF:
		return new DxEffAniData_FaceOff;
	};

	return NULL;
}

DxEffAniData* DxEffAniMan::CreateEffect ( DWORD TypeID, EFFANI_PROPERTY* pProp )
{
	GASSERT(m_pd3dDevice);

	HRESULT hr = S_OK;

	DxEffAniData* pEffProp = CreateEffInstance ( TypeID );
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

HRESULT DxEffAniMan::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	return S_OK;
}

HRESULT DxEffAniMan::RestoreDeviceObjects ()
{
	DxEffAniSingle::CreateDevice( m_pd3dDevice );
	DxEffAniGhosting::CreateDevice( m_pd3dDevice );
	DxEffAniTrace::CreateDevice( m_pd3dDevice );
	DxEffAniFaceOff::CreateDevice( m_pd3dDevice );

	return S_OK;
}

HRESULT DxEffAniMan::InvalidateDeviceObjects ()
{
	DxEffAniSingle::ReleaseDevice( m_pd3dDevice );
	DxEffAniGhosting::ReleaseDevice( m_pd3dDevice );
	DxEffAniTrace::ReleaseDevice( m_pd3dDevice );
	DxEffAniFaceOff::ReleaseDevice( m_pd3dDevice );

	return S_OK;
}

HRESULT DxEffAniMan::DeleteDeviceObjects()
{
	m_pd3dDevice = NULL;

	return S_OK;
}
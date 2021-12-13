#include "pch.h"

#include "./editmeshs.h"

#include "./DxEffectMan.h"

#include "./DxEffSingle.h"
#include "./DxEffSinglePropGMan.h"
#include "./SerialFile.h"

#include "./DxSkinChar.h"
#include "./DxBoneCollector.h"
#include "./DxWeatherMan.h"
#include "./GLPeriod.h"

#include "./DxEffAniSingle.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern BOOL g_bCHAR_EDIT_RUN;

DWORD		DxEffAniData_Single::TYPEID			= EMEFFANI_SINGLE;
DWORD		DxEffAniData_Single::VERSION		= 0x0101;
char		DxEffAniData_Single::NAME[64]		= "Single EFFECT";

DxEffAni* DxEffAniData_Single::NEWOBJ( SAnimContainer* pAnimContainer )
{
	DxEffAniSingle*	pEff = new DxEffAniSingle;

	pEff->m_dwFlags		= m_dwFlags;
	pEff->m_dwFrame		= m_dwFrame;
	pEff->m_fFact		= m_fFact;
	pEff->m_fScale		= m_fScale;
	pEff->m_matBase		= m_matBase;

	pEff->m_pPropGroup = m_pPropGroup;

	StringCchCopy( pEff->m_szEff,		STRING_NUM_128, m_szEff );
	StringCchCopy( pEff->m_szBipFirst,	STRING_NUM_128, m_szBipFirst );
	StringCchCopy( pEff->m_szBipSecond,	STRING_NUM_128, m_szBipSecond );

	pEff->Create ( m_pd3dDevice, pAnimContainer );

	return	pEff;
}

HRESULT	DxEffAniData_Single::LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;
	DWORD dwVer, dwSize, dwTypeID;

	SFile >> dwTypeID;
	SFile >> dwVer;
	SFile >> dwSize;

	//	Note : 버전이 일치할 경우. 
	//
	if ( dwVer == VERSION )
	{
		//	Note : 이팩트의 Property 를 로드
		//
		SFile.ReadBuffer ( &m_Property, sizeof(m_Property) );
	}
	else if ( dwVer == 0x0100 )
	{
		//	Note : 이팩트의 Property 를 로드.
		EFFANI_PROPERTY_SINGLE_100 sProp;
		SFile.ReadBuffer( &sProp, sizeof(EFFANI_PROPERTY_SINGLE_100) );
		m_dwFlags	= sProp.m_dwFlags;
		m_dwFrame	= sProp.m_dwFrame;
		m_fFact		= sProp.m_fFact;
		m_fScale	= sProp.m_fScale;

		StringCchCopy( m_szBipFirst, STRING_NUM_128, sProp.m_szBipFirst );
		StringCchCopy( m_szBipSecond, STRING_NUM_128, sProp.m_szBipSecond );
		StringCchCopy( m_szEff, STRING_NUM_128, sProp.m_szEff );

		D3DXMatrixIdentity( &m_matBase );
	}
	else
	{
		//	Note : 버전이 틀릴 경우에는 파일에 쓰여진 DATA영역을 건더 띄는 작업을 진행.
		SFile.SetOffSet ( SFile.GetfTell()+dwSize );

		CDebugSet::ToLogFile( _T("DxEffAniData_Single::LoadFile() - CURRENT_VERSION_%d, SAVE_VERSION_%d"), VERSION, dwVer );
		
		return E_FAIL;
	}

	//	Note : Device 자원을 생성한다.
	//
	if ( pd3dDevice )
	{
		hr = Create ( pd3dDevice );
		if ( FAILED(hr) )	return hr;
	}

	return S_OK;
}

HRESULT	DxEffAniData_Single::SaveFile ( CSerialFile &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	SFile << (DWORD) ( sizeof(m_Property) );

	//	Note : 이팩트의 Property 를 저장.
	//
	SFile.WriteBuffer ( &m_Property, sizeof(m_Property) );

	return S_OK;
}

BOOL DxEffAniData_Single::CheckEff ( DWORD dwCurKeyTime, int nPrevFrame )
{
	if( nPrevFrame == (int)dwCurKeyTime )	return FALSE;		// 같은 프레임이라면 새로 생성 안한다.

	if( nPrevFrame > (int)dwCurKeyTime )	// 시간이 초기화 됨.
	{
		return FALSE;
	}

	if( ((int)m_dwFrame > nPrevFrame) && (m_dwFrame <= dwCurKeyTime) )
	{
		return TRUE;
	}

	return FALSE;
}

void DxEffAniData_Single::SetEffAniData ( DxEffAniData*	pData )
{
	DxEffAniData::SetEffAniData ( pData );

	m_pPropGroup = ((DxEffAniData_Single*)pData)->m_pPropGroup;
}

HRESULT DxEffAniData_Single::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	DxEffAniData::InitDeviceObjects ( pd3dDevice );

	m_pPropGroup = DxEffSinglePropGMan::GetInstance().LoadEffectGProp ( m_szEff );
	if ( !m_pPropGroup )	return E_FAIL;

	return S_OK;
}

HRESULT DxEffAniData_Single::DeleteDeviceObjects ()
{
	DxEffAniData::DeleteDeviceObjects ();

	return S_OK;
}


//	-----------------------------------------------	--------------------------------------------------

DxEffAniSingle::DxEffAniSingle(void) :
	DxEffAni(),
	m_bDataSetting(FALSE),
	m_vPos_1(0.f,0.f,0.f),
	m_vPos_2(0.f,0.f,0.f),
	m_fRandomFact(0.f),

	m_pPropGroup(NULL)
{
	D3DXMatrixIdentity ( &m_matBase );
	D3DXMatrixIdentity ( &m_matWorld );
	m_pSingleGroup = NULL;
}

DxEffAniSingle::~DxEffAniSingle(void)
{
	CleanUp ();
}

HRESULT DxEffAniSingle::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT DxEffAniSingle::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT DxEffAniSingle::OneTimeSceneInit ()
{

	if ( m_pSingleGroup )		m_pSingleGroup->OneTimeSceneInit ();

	return S_OK;
}

HRESULT DxEffAniSingle::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;

	BOOL	bUse = TRUE;
	bUse = SetWeather();		// 날씨에 대한 적용
	if ( bUse )
	{
		bUse = SetTime();		// 낮 밤에 대한 적용
	}

	if( g_bCHAR_EDIT_RUN )	bUse = TRUE;		// 캐릭터 에디터에서는 계속 나오게 함.

	if ( m_pPropGroup && bUse )
	{	
		SAFE_DELETE(m_pSingleGroup);						// 이전 데이터는 지워준다.
		m_pSingleGroup = m_pPropGroup->NEWEFFGROUP ();
		if ( !m_pSingleGroup )	return E_FAIL;

		//	Note : Eff Group Create Dev.
		//
		m_pSingleGroup->Create ( pd3dDevice );

		m_fRandomFact = ((float)(rand()%101)/100);			// 0.f~1.f
		m_pSingleGroup->m_dwFlag &= ~EFF_CFG_NEVERDIE;		// 효과는 무조건 사라져야 한다.
	}

	if ( m_pSingleGroup )		m_pSingleGroup->InitDeviceObjects ( pd3dDevice );

	return S_OK;
}

HRESULT DxEffAniSingle::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_pSingleGroup )		m_pSingleGroup->RestoreDeviceObjects ( pd3dDevice );

	return S_OK;
}

HRESULT DxEffAniSingle::InvalidateDeviceObjects ()
{
	if ( m_pSingleGroup )		m_pSingleGroup->InvalidateDeviceObjects ();

	return S_OK;
}

HRESULT DxEffAniSingle::DeleteDeviceObjects ()
{
	if ( m_pSingleGroup )		m_pSingleGroup->DeleteDeviceObjects ();

	return S_OK;
}

HRESULT DxEffAniSingle::FinalCleanup ()
{
	if ( m_pSingleGroup )		m_pSingleGroup->FinalCleanup();
	SAFE_DELETE(m_pSingleGroup);

	return S_OK;
}

HRESULT DxEffAniSingle::FrameMove ( float fTime, float fElapsedTime )
{
	HRESULT hr = S_OK;

	if ( !m_pSingleGroup )
	{
		SetEnd ();
		return S_OK;
	}

	m_pSingleGroup->FrameMove ( fTime, fElapsedTime );

	if ( !m_pSingleGroup->m_dwAliveCount )		// 지워야 할 경우.
	{
		SetEnd ();
		return S_OK;
	}

	return S_OK;
}

HRESULT	DxEffAniSingle::Render ( const LPDIRECT3DDEVICEQ pd3dDevice, DxSkinChar* pSkinChar, const D3DXMATRIX& matCurPos )
{
	if ( !m_pSingleGroup)	return S_OK;

	if ( m_dwFlags&USECHARMOVE )		// 이동하는 물체
	{
		LPD3DXMATRIX pBoneMatrix(NULL);
		if ( !SetPosition ( pSkinChar, pBoneMatrix ) )	return S_OK;

		SetMatrix ( matCurPos, pBoneMatrix );
	}
	else							// 처음 사용 할 경우
	{
		if ( !m_bDataSetting )
		{
			LPD3DXMATRIX pBoneMatrix(NULL);
			if ( !SetPosition ( pSkinChar, pBoneMatrix ) )	return S_OK;

			SetMatrix ( matCurPos, pBoneMatrix );

			m_bDataSetting = TRUE;			// 데이터 셋팅 완료
		}

		m_pSingleGroup->m_matWorld = m_matWorld;
	}

	BOOL	bUse = TRUE;
	bUse = SetWeather();		// 날씨에 대한 적용
	if ( bUse )
	{
		bUse = SetTime();		// 낮 밤에 대한 적용
	}

	if( g_bCHAR_EDIT_RUN )	
	{
		bUse = TRUE;		// 캐릭터 에디터에서는 계속 나오게 함.

		EDITMESHS::RENDERSPHERE ( pd3dDevice, m_vPos_1, 1.0f );
		EDITMESHS::RENDERSPHERE ( pd3dDevice, m_vPos_2, 1.0f );
		
	//	if( m_dwFlags&USEDRAWMATRIX )
	//	{
	//		DxSkeleton::BONETRANSMAP* BoneMap = &pSkinChar->GetSkeleton()->BoneMap;

	//		std::string strname = m_szBipFirst;
	//		DxSkeleton::BONETRANSMAP_ITER iter = BoneMap->find ( strname );
	//		if ( iter==BoneMap->end() )			return FALSE;

	//		
	//		(*iter).second->matCombined;
	//	}
	}

	m_pSingleGroup->m_bOut_Particle = bUse;
	m_pSingleGroup->m_fOut_Scale	= m_fScale;				// 스케일

	m_pSingleGroup->Render ( pd3dDevice );

	return S_OK;
}

BOOL	DxEffAniSingle::SetPosition ( DxSkinChar* pSkinChar, LPD3DXMATRIX& pMatrix )
{
	DxSkeleton::BONETRANSMAP* BoneMap = &pSkinChar->GetSkeleton()->BoneMap;

	std::string strname = m_szBipFirst;
	DxSkeleton::BONETRANSMAP_ITER iter = BoneMap->find ( strname );
	if ( iter==BoneMap->end() )			return FALSE;					// 에러 코드를 넣자

	pMatrix = &(*iter).second->matCombined;			// 주소를 얻는다.
	m_vPos_1.x = (*iter).second->matCombined._41;
	m_vPos_1.y = (*iter).second->matCombined._42;
	m_vPos_1.z = (*iter).second->matCombined._43;

	strname = m_szBipSecond;
	iter = BoneMap->find ( strname );
	if ( iter==BoneMap->end() )			return FALSE;					// 에러 코드를 넣자
	m_vPos_2.x = (*iter).second->matCombined._41;
	m_vPos_2.y = (*iter).second->matCombined._42;
	m_vPos_2.z = (*iter).second->matCombined._43;

	return TRUE;
}

void	DxEffAniSingle::SetMatrix ( const D3DXMATRIX& matCurPos, D3DXMATRIX* pmatBone )
{
	D3DXMATRIX	matWorld;
	D3DXMatrixIdentity ( &matWorld );

	if ( m_dwFlags&USEDRAWPOINT )
	{
		matWorld._41 = m_vPos_1.x;
		matWorld._42 = m_vPos_1.y;
		matWorld._43 = m_vPos_1.z;

		D3DXMATRIX	matRotate, matRotateAni;
		D3DXMatrixRotationY ( &matRotate, (D3DX_PI*0.5f) );
		matRotateAni = matCurPos;
		matRotateAni._41 = 0.f;
		matRotateAni._42 = 0.f;
		matRotateAni._43 = 0.f;

		D3DXMatrixMultiply ( &matRotate, &matRotate, &matRotateAni );
		D3DXMatrixMultiply ( &matWorld, &matRotate, &matWorld );

		m_pSingleGroup->m_matWorld = matWorld;
	}
	else if ( m_dwFlags&USEDRAWRANGE )
	{
		float fTemp = 1.f - m_fRandomFact;
		matWorld._41 = m_vPos_1.x*m_fRandomFact + m_vPos_2.x*fTemp;
		matWorld._42 = m_vPos_1.y*m_fRandomFact + m_vPos_2.y*fTemp;
		matWorld._43 = m_vPos_1.z*m_fRandomFact + m_vPos_2.z*fTemp;

		D3DXMATRIX	matRotate, matRotateAni;
		D3DXMatrixRotationY ( &matRotate, (D3DX_PI*0.5f) );
		matRotateAni = matCurPos;
		matRotateAni._41 = 0.f;
		matRotateAni._42 = 0.f;
		matRotateAni._43 = 0.f;

		D3DXMatrixMultiply ( &matRotate, &matRotate, &matRotateAni );
		D3DXMatrixMultiply ( &matWorld, &matRotate, &matWorld );

		m_pSingleGroup->m_matWorld = matWorld;
	}
	else if ( m_dwFlags&USEDRAWDIRECT )
	{
		matWorld._41 = m_vPos_1.x;
		matWorld._42 = m_vPos_1.y;
		matWorld._43 = m_vPos_1.z;

		m_pSingleGroup->m_vTargetPos	= m_vPos_2;	// 목표 위치
		m_pSingleGroup->m_vStartPos		= m_vPos_1;	// 시작 위치
		m_pSingleGroup->m_vDirect		= m_pSingleGroup->m_vTargetPos - m_pSingleGroup->m_vStartPos;	// 방향

		m_pSingleGroup->m_bOut_Direct	= TRUE;					// 방향까지 넣어 준다.

		m_pSingleGroup->m_matOutRotate	= DxBillboardLookAtDir ( &m_pSingleGroup->m_vDirect );

		m_pSingleGroup->m_matWorld = matWorld;
	}
	else if( m_dwFlags&USEDRAWMATRIX )
	{
		D3DXMatrixMultiply( &matWorld, &m_matBase, pmatBone );
		m_pSingleGroup->m_matWorld = matWorld;
	}

	m_matWorld = matWorld;
}

BOOL	DxEffAniSingle::SetTime ()
{
	if ( GLPeriod::GetInstance().IsOffLight() )	// 낮
	{
		if ( m_dwFlags & USETIMEDAY )	return TRUE;
		else							return FALSE;
	}
	else									// 밤
	{
		if ( m_dwFlags & USETIMENIGHT )	return TRUE;
		else							return FALSE;
	}

	return TRUE;
}

BOOL	DxEffAniSingle::SetWeather ()
{
	if ( DxWeatherMan::GetInstance()->GetRain()->UseRain() )			// 비 오는 날
	{
		if ( m_dwFlags & USESKYRAIN )	return TRUE;
		else							return FALSE;
	}
	else if ( DxWeatherMan::GetInstance()->GetSnow()->UseSnow() )		// 눈 오는 날
	{
		if ( m_dwFlags & USESKYSNOW )	return TRUE;
		else							return FALSE;
	}
	else if ( DxWeatherMan::GetInstance()->GetLeaves()->UseLeaves() )	// 낙엽 내리는 날
	{
		if ( m_dwFlags & USESKYLEAVES )	return TRUE;
		else							return FALSE;
	}
	else																// 맑은 날씨
	{
		if ( m_dwFlags & USESKYFINE )	return TRUE;
		else							return FALSE;
	}

	return TRUE;
}

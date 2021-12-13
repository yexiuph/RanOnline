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

	//	Note : ������ ��ġ�� ���. 
	//
	if ( dwVer == VERSION )
	{
		//	Note : ����Ʈ�� Property �� �ε�
		//
		SFile.ReadBuffer ( &m_Property, sizeof(m_Property) );
	}
	else if ( dwVer == 0x0100 )
	{
		//	Note : ����Ʈ�� Property �� �ε�.
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
		//	Note : ������ Ʋ�� ��쿡�� ���Ͽ� ������ DATA������ �Ǵ� ��� �۾��� ����.
		SFile.SetOffSet ( SFile.GetfTell()+dwSize );

		CDebugSet::ToLogFile( _T("DxEffAniData_Single::LoadFile() - CURRENT_VERSION_%d, SAVE_VERSION_%d"), VERSION, dwVer );
		
		return E_FAIL;
	}

	//	Note : Device �ڿ��� �����Ѵ�.
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

	//	Note : ����Ʈ�� Property �� ����.
	//
	SFile.WriteBuffer ( &m_Property, sizeof(m_Property) );

	return S_OK;
}

BOOL DxEffAniData_Single::CheckEff ( DWORD dwCurKeyTime, int nPrevFrame )
{
	if( nPrevFrame == (int)dwCurKeyTime )	return FALSE;		// ���� �������̶�� ���� ���� ���Ѵ�.

	if( nPrevFrame > (int)dwCurKeyTime )	// �ð��� �ʱ�ȭ ��.
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
	bUse = SetWeather();		// ������ ���� ����
	if ( bUse )
	{
		bUse = SetTime();		// �� �㿡 ���� ����
	}

	if( g_bCHAR_EDIT_RUN )	bUse = TRUE;		// ĳ���� �����Ϳ����� ��� ������ ��.

	if ( m_pPropGroup && bUse )
	{	
		SAFE_DELETE(m_pSingleGroup);						// ���� �����ʹ� �����ش�.
		m_pSingleGroup = m_pPropGroup->NEWEFFGROUP ();
		if ( !m_pSingleGroup )	return E_FAIL;

		//	Note : Eff Group Create Dev.
		//
		m_pSingleGroup->Create ( pd3dDevice );

		m_fRandomFact = ((float)(rand()%101)/100);			// 0.f~1.f
		m_pSingleGroup->m_dwFlag &= ~EFF_CFG_NEVERDIE;		// ȿ���� ������ ������� �Ѵ�.
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

	if ( !m_pSingleGroup->m_dwAliveCount )		// ������ �� ���.
	{
		SetEnd ();
		return S_OK;
	}

	return S_OK;
}

HRESULT	DxEffAniSingle::Render ( const LPDIRECT3DDEVICEQ pd3dDevice, DxSkinChar* pSkinChar, const D3DXMATRIX& matCurPos )
{
	if ( !m_pSingleGroup)	return S_OK;

	if ( m_dwFlags&USECHARMOVE )		// �̵��ϴ� ��ü
	{
		LPD3DXMATRIX pBoneMatrix(NULL);
		if ( !SetPosition ( pSkinChar, pBoneMatrix ) )	return S_OK;

		SetMatrix ( matCurPos, pBoneMatrix );
	}
	else							// ó�� ��� �� ���
	{
		if ( !m_bDataSetting )
		{
			LPD3DXMATRIX pBoneMatrix(NULL);
			if ( !SetPosition ( pSkinChar, pBoneMatrix ) )	return S_OK;

			SetMatrix ( matCurPos, pBoneMatrix );

			m_bDataSetting = TRUE;			// ������ ���� �Ϸ�
		}

		m_pSingleGroup->m_matWorld = m_matWorld;
	}

	BOOL	bUse = TRUE;
	bUse = SetWeather();		// ������ ���� ����
	if ( bUse )
	{
		bUse = SetTime();		// �� �㿡 ���� ����
	}

	if( g_bCHAR_EDIT_RUN )	
	{
		bUse = TRUE;		// ĳ���� �����Ϳ����� ��� ������ ��.

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
	m_pSingleGroup->m_fOut_Scale	= m_fScale;				// ������

	m_pSingleGroup->Render ( pd3dDevice );

	return S_OK;
}

BOOL	DxEffAniSingle::SetPosition ( DxSkinChar* pSkinChar, LPD3DXMATRIX& pMatrix )
{
	DxSkeleton::BONETRANSMAP* BoneMap = &pSkinChar->GetSkeleton()->BoneMap;

	std::string strname = m_szBipFirst;
	DxSkeleton::BONETRANSMAP_ITER iter = BoneMap->find ( strname );
	if ( iter==BoneMap->end() )			return FALSE;					// ���� �ڵ带 ����

	pMatrix = &(*iter).second->matCombined;			// �ּҸ� ��´�.
	m_vPos_1.x = (*iter).second->matCombined._41;
	m_vPos_1.y = (*iter).second->matCombined._42;
	m_vPos_1.z = (*iter).second->matCombined._43;

	strname = m_szBipSecond;
	iter = BoneMap->find ( strname );
	if ( iter==BoneMap->end() )			return FALSE;					// ���� �ڵ带 ����
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

		m_pSingleGroup->m_vTargetPos	= m_vPos_2;	// ��ǥ ��ġ
		m_pSingleGroup->m_vStartPos		= m_vPos_1;	// ���� ��ġ
		m_pSingleGroup->m_vDirect		= m_pSingleGroup->m_vTargetPos - m_pSingleGroup->m_vStartPos;	// ����

		m_pSingleGroup->m_bOut_Direct	= TRUE;					// ������� �־� �ش�.

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
	if ( GLPeriod::GetInstance().IsOffLight() )	// ��
	{
		if ( m_dwFlags & USETIMEDAY )	return TRUE;
		else							return FALSE;
	}
	else									// ��
	{
		if ( m_dwFlags & USETIMENIGHT )	return TRUE;
		else							return FALSE;
	}

	return TRUE;
}

BOOL	DxEffAniSingle::SetWeather ()
{
	if ( DxWeatherMan::GetInstance()->GetRain()->UseRain() )			// �� ���� ��
	{
		if ( m_dwFlags & USESKYRAIN )	return TRUE;
		else							return FALSE;
	}
	else if ( DxWeatherMan::GetInstance()->GetSnow()->UseSnow() )		// �� ���� ��
	{
		if ( m_dwFlags & USESKYSNOW )	return TRUE;
		else							return FALSE;
	}
	else if ( DxWeatherMan::GetInstance()->GetLeaves()->UseLeaves() )	// ���� ������ ��
	{
		if ( m_dwFlags & USESKYLEAVES )	return TRUE;
		else							return FALSE;
	}
	else																// ���� ����
	{
		if ( m_dwFlags & USESKYFINE )	return TRUE;
		else							return FALSE;
	}

	return TRUE;
}

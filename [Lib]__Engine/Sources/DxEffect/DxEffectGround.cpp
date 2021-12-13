#include "pch.h"

#include "./DxViewPort.h"
#include "./TextureManager.h"
#include "./DxDynamicVB.h"

#include "./DxEffectMan.h"
#include "./DxEffSingleMan.h"
#include "./SerialFile.h"

#include "./DxEffectGround.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//---------------------------------------------------------------------[PARTICLESYS_PROPERTY]
const DWORD	GROUND_PROPERTY::VERSION	= 0x0102;
const char	GROUND_PROPERTY::NAME[]	= "지면효과-캐릭";

DxEffSingle* GROUND_PROPERTY::NEWOBJ ()
{
	//DxEffectGround *pEffect = new DxEffectGround;
	DxEffectGround *pEffect = DxEffectGround::m_pPool->New();

	//	Note : TransPorm Set.		[공통설정]
	pEffect->m_matLocal = m_matLocal;

	//	Note : 시간 설정.			[공통설정]
	pEffect->m_fGBeginTime = m_fGBeginTime;
	pEffect->m_fGLifeTime = m_fGLifeTime;

	//	Note : 날아가는 오브젝트	[공통설정]
	pEffect->m_bMoveObj = m_bMoveObj;

	pEffect->m_dwFlag = m_dwFlag;

	pEffect->m_fAniTime = m_fAniTime;
	pEffect->m_iCol = m_iCol;
	pEffect->m_iRow = m_iRow;

	pEffect->m_fRotateAngel = m_fRotateAngel;

	//	Note : 높이 초기화
	pEffect->m_fHeight = m_fHeightStart;
	pEffect->m_fHeightStart = m_fHeightStart;

	pEffect->m_fHeightTime1 = m_fGBeginTime + m_fGLifeTime*m_fHeightRate1/100.f;
	pEffect->m_fHeightTime2 = m_fGBeginTime + m_fGLifeTime*m_fHeightRate2/100.f;

	if ( m_fHeightRate1 == 0.f )				pEffect->m_fHeightDelta1	= 0.f;
	else										pEffect->m_fHeightDelta1 = ( m_fHeightMid1 - m_fHeightStart ) / (pEffect->m_fHeightTime1-m_fGBeginTime);
	if ( m_fHeightRate2-m_fHeightRate1 == 0.f )	pEffect->m_fHeightDelta2	= 0.f;
	else										pEffect->m_fHeightDelta2 = ( m_fHeightMid2 - m_fHeightMid1 ) / (pEffect->m_fHeightTime2-pEffect->m_fHeightTime1);
	if ( m_fHeightRate2 == 100.f )				pEffect->m_fHeightDelta3	= 0.f;
	else										pEffect->m_fHeightDelta3 = ( m_fHeightEnd - m_fHeightMid2 ) / (pEffect->m_fGBeginTime+m_fGLifeTime-pEffect->m_fHeightTime2);

	//	Note : 범위(사이즈) 초기화
	pEffect->m_fSize = m_fSizeStart/2;
	pEffect->m_fSizeStart = m_fSizeStart/2;

	pEffect->m_fSizeTime1 = m_fGBeginTime + m_fGLifeTime*m_fSizeRate1/100.f;
	pEffect->m_fSizeTime2 = m_fGBeginTime + m_fGLifeTime*m_fSizeRate2/100.f;

	if ( m_fSizeRate1 == 0.f )				pEffect->m_fSizeDelta1	= 0.f;
	else									pEffect->m_fSizeDelta1 = ( m_fSizeMid1 - m_fSizeStart ) / (pEffect->m_fSizeTime1-m_fGBeginTime);
	if ( m_fSizeRate2-m_fSizeRate1 == 0.f )	pEffect->m_fSizeDelta2	= 0.f;
	else									pEffect->m_fSizeDelta2 = ( m_fSizeMid2 - m_fSizeMid1 ) / (pEffect->m_fSizeTime2-pEffect->m_fSizeTime1);
	if ( m_fSizeRate2 == 100.f )			pEffect->m_fSizeDelta3	= 0.f;
	else									pEffect->m_fSizeDelta3 = ( m_fSizeEnd - m_fSizeMid2 ) / (m_fGBeginTime+m_fGLifeTime-pEffect->m_fSizeTime2);

	//	Note : 알파 초기화
	pEffect->m_fAlpha = m_fAlphaStart;
	pEffect->m_fAlphaStart = m_fAlphaStart;

	pEffect->m_fAlphaTime1 = m_fGBeginTime + m_fGLifeTime*m_fAlphaRate1/100.f;
	pEffect->m_fAlphaTime2 = m_fGBeginTime + m_fGLifeTime*m_fAlphaRate2/100.f;

	if ( m_fAlphaRate1 == 0.f )					pEffect->m_fAlphaDelta1	= 0.f;
	else										pEffect->m_fAlphaDelta1 = ( m_fAlphaMid1 - m_fAlphaStart ) / (pEffect->m_fAlphaTime1-m_fGBeginTime);
	if ( m_fAlphaRate2-m_fAlphaRate1 == 0.f )	pEffect->m_fAlphaDelta2	= 0.f;
	else										pEffect->m_fAlphaDelta2 = ( m_fAlphaMid2 - m_fAlphaMid1 ) / (pEffect->m_fAlphaTime2-pEffect->m_fAlphaTime1);
	if ( m_fAlphaRate2 == 100.f || m_fAlphaEnd == m_fAlphaMid2 )	pEffect->m_fAlphaDelta3	= 0.f;
	else															pEffect->m_fAlphaDelta3 = ( m_fAlphaEnd - m_fAlphaMid2 - 0.1f ) / (m_fGBeginTime+m_fGLifeTime-pEffect->m_fAlphaTime2);

	pEffect->m_cColorStart = m_cColorStart;
	pEffect->m_cColorVar = m_cColorVar;
	pEffect->m_cColorEnd = m_cColorEnd;

	//	Note : 스프라이트 갯수 초기화
	pEffect->m_iAllSprite = m_iCol * m_iRow;

	pEffect->m_nBlend = m_nBlend;

	//	Note : 텍스쳐 설정.
	pEffect->m_strTexture = m_szTexture;
	pEffect->m_pTexture = m_pTexture;

	//	Note : 사운드 설정
	pEffect->m_MovSound = m_MovSound;

	return pEffect;
}

HRESULT GROUND_PROPERTY::SaveFile ( CSerialFile &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	//	Note : DATA의 사이즈를 기록한다. Load 시에 버전이 틀릴 경우 사용됨.
	//
	SFile << (DWORD) ( GetSizeBase() + sizeof(m_Property) );

	//	Note : 부모 클레스의 정보.
	SFile.WriteBuffer( m_matLocal, sizeof(D3DXMATRIX) );
	
	SFile << m_bMoveObj;
	SFile << m_fGBeginTime;
	SFile << m_fGLifeTime;

	//	Note : 이팩트의 Property 를 저장.
	//
	SFile.WriteBuffer ( &m_Property, sizeof(PROPERTY) );

	//	Note : 형제, 자식 저장.
	//
	EFF_PROPERTY::SaveFile ( SFile );

	return S_OK;
}

HRESULT GROUND_PROPERTY::LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	//	Note : 버전이 일치할 경우. 
	//
	if( dwVer == VERSION )
	{
		//	Note : 부모 클레스의 정보.
		SFile.ReadBuffer ( m_matLocal, sizeof(D3DXMATRIX) );
		
		SFile >> m_bMoveObj;
		SFile >> m_fGBeginTime;
		SFile >> m_fGLifeTime;

		//	Note : 이팩트의 Property 를 저장.
		//
		SFile.ReadBuffer ( &m_Property, sizeof(PROPERTY) );

		m_pTexture = NULL;
	}
	else if( dwVer == 0x0101 )
	{
		//	Note : 부모 클레스의 정보.
		DXAFFINEPARTS sAffineParts;
		SFile.ReadBuffer( m_matLocal, sizeof(D3DXMATRIX) );
		SFile.ReadBuffer( &sAffineParts, sizeof(DXAFFINEPARTS) );
		D3DXMatrixCompX( m_matLocal, sAffineParts );
		
		SFile >> m_bMoveObj;
		SFile >> m_fGBeginTime;
		SFile >> m_fGLifeTime;

		//	Note : 이팩트의 Property 를 저장.
		//
		SFile.ReadBuffer ( &m_Property, sizeof(PROPERTY) );

		m_pTexture = NULL;
	}
	else if( dwVer == 0x0100 )
	{
		//	Note : 부모 클레스의 정보.
		DXAFFINEPARTS sAffineParts;
		SFile.ReadBuffer( m_matLocal, sizeof(D3DXMATRIX) );
		SFile.ReadBuffer( &sAffineParts, sizeof(DXAFFINEPARTS) );
		D3DXMatrixCompX( m_matLocal, sAffineParts );
		
		SFile >> m_bMoveObj;
		SFile >> m_fGBeginTime;
		SFile >> m_fGLifeTime;

		//	Note : 이팩트의 Property 를 저장.
		//
		PROPERTY_100 pProp;
		SFile.ReadBuffer ( &pProp, sizeof(PROPERTY_100) );
		

		m_Property.m_fAniTime		= pProp.m_fAniTime;		
		m_Property.m_iCol			= pProp.m_iCol;
		m_Property.m_iRow			= pProp.m_iRow;
		m_Property.m_fRotateAngel	= pProp.m_fRotateAngel;
		m_Property.m_fHeightRate1	= pProp.m_fHeightRate1;
		m_Property.m_fHeightRate2	= pProp.m_fHeightRate2;
		m_Property.m_fHeightStart	= pProp.m_fHeightStart;
		m_Property.m_fHeightMid1	= pProp.m_fHeightMid1;
		m_Property.m_fHeightMid2	= pProp.m_fHeightMid2;
		m_Property.m_fHeightEnd		= pProp.m_fHeightEnd;
		m_Property.m_fSizeRate1		= pProp.m_fSizeRate1;
		m_Property.m_fSizeRate2		= pProp.m_fSizeRate2;
		m_Property.m_fSizeStart		= pProp.m_fSizeStart;
		m_Property.m_fSizeMid1		= pProp.m_fSizeMid1;
		m_Property.m_fSizeMid2		= pProp.m_fSizeMid2;
		m_Property.m_fSizeEnd		= pProp.m_fSizeEnd;
		m_Property.m_fAlphaRate1	= pProp.m_fAlphaRate1;
		m_Property.m_fAlphaRate2	= pProp.m_fAlphaRate2;
		m_Property.m_fAlphaStart	= pProp.m_fAlphaStart;
		m_Property.m_fAlphaMid1		= pProp.m_fAlphaMid1;
		m_Property.m_fAlphaMid2		= pProp.m_fAlphaMid2;
		m_Property.m_fAlphaEnd		= pProp.m_fAlphaEnd;
		m_Property.m_cColorStart	= pProp.m_cColorStart;
		m_Property.m_cColorVar		= pProp.m_cColorVar;
		m_Property.m_cColorEnd		= pProp.m_cColorEnd;
		m_Property.m_nBlend			= pProp.m_nBlend;

		StringCchCopy( m_Property.m_szTexture, 256, pProp.m_szTexture );

		if ( pProp.m_bAni )			m_Property.m_dwFlag		|= USEANI;
		if ( pProp.m_bRotateUse )	m_Property.m_dwFlag		|= USEROTATE;
		if ( pProp.m_bHeight )		m_Property.m_dwFlag		|= USEHEIGHT;
	}
	else
	{
		//	Note : 버전이 틀릴 경우에는 파일에 쓰여진 DATA영역을 건더 띄는 작업을 진행.
		//
		DWORD dwCur = SFile.GetfTell ();
		SFile.SetOffSet ( dwCur+dwSize );
	}

	//	Note : Device 자원을 생성한다.
	//
	hr = Create ( pd3dDevice );
	if ( FAILED(hr) )	return hr;

	//	Note : 형제, 자식 읽기.
	//
	EFF_PROPERTY::LoadFile ( SFile, pd3dDevice );

	return S_OK;
}

HRESULT GROUND_PROPERTY::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	TextureManager::LoadTexture ( m_szTexture, pd3dDevice, m_pTexture, 0, 0, TRUE );

	//	Note : 형제, 자식.
	//
	EFF_PROPERTY::InitDeviceObjects ( pd3dDevice );

	return S_OK;
}

HRESULT GROUND_PROPERTY::DeleteDeviceObjects ()
{
	TextureManager::ReleaseTexture( m_szTexture, m_pTexture );

	//	Note : 형제, 자식.
	//
	EFF_PROPERTY::DeleteDeviceObjects ();

	return S_OK;
}

const DWORD	DxEffectGround::TYPEID		= EFFSINGLE_GROUND;
const DWORD	DxEffectGround::FLAG		= NULL;
const char	DxEffectGround::NAME[]		= "지면효과-캐릭";

CMemPool<DxEffectGround> *DxEffectGround::m_pPool = NULL;

LPDIRECT3DSTATEBLOCK9		DxEffectGround::m_pSavedRenderStats = NULL;
LPDIRECT3DSTATEBLOCK9		DxEffectGround::m_pDrawRenderStats = NULL;

BOOL DxEffectGround:: VAILEDDEVICE				= FALSE;

DxEffectGround::DxEffectGround(void) :
	m_vGLocation(0,0,0),
	m_nBlend(1),
	m_fTimeSum(0.f),
	m_iAllSprite(1),
	m_iNowSprite(0),

	m_bRotateUse(FALSE),
	m_fRotateAngel(0.f),
	m_fRotateAngelSum(0.f),

	m_fHeightTime1(0.f),
	m_fHeightTime2(0.f),
	m_fHeightDelta1(0.f),
	m_fHeightDelta2(0.f),
	m_fHeightDelta3(0.f),

	m_fSizeTime1(0.f),
	m_fSizeTime2(0.f),
	m_fSizeDelta1(0.f),
	m_fSizeDelta2(0.f),
	m_fSizeDelta3(0.f),

	m_fAlphaDelta1(0.f),
	m_fAlphaDelta2(0.f),
	m_fAlphaDelta3(0.f),
	m_fAlphaTime1(0.f),
	m_fAlphaTime2(0.f),

	m_cColorDelta(0.f,0.f,0.f,0.f),

	m_bSequenceRoop(FALSE),
	m_vPrevPicking(0.f,0.f,0.f),

	m_pd3dDevice(NULL)
{
	m_fAniTime = 0.f;
	m_iCol = 1;
	m_iRow = 1;

	m_fHeight	= 0.f;

	m_fSize		= 0.f;

	m_fAlpha	= 0.f;

	m_cColorStart.r		= 0.0f;
	m_cColorStart.g		= 0.0f;
	m_cColorStart.b		= 0.0f;
	m_cColorStart.a		= 0.4f;
			
	m_cColorVar.r		= 0.0f;
	m_cColorVar.g		= 0.0f;
	m_cColorVar.b		= 0.0f;
	m_cColorVar.a		= 0.0f;

	m_cColorEnd.r		= 0.0f;
	m_cColorEnd.g		= 0.0f;
	m_cColorEnd.b		= 0.0f;
	m_cColorEnd.a		= 0.0f;

	D3DXMatrixIdentity( &m_matWorld );
}

DxEffectGround::~DxEffectGround(void)
{

}

HRESULT DxEffectGround::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;

	VAILEDDEVICE = TRUE;

	D3DCAPSQ d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState( D3DRS_COLORVERTEX,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,			0x00000000 );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		FALSE );

		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_ALPHAREF,			0x01 );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2,	D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );

		pd3dDevice->SetSamplerState ( 0, D3DSAMP_BORDERCOLOR, 0x00000000 );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSU,	D3DTADDRESS_BORDER );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSV,	D3DTADDRESS_BORDER );

		if( which==0 )	pd3dDevice->EndStateBlock ( &m_pSavedRenderStats );
		else			pd3dDevice->EndStateBlock ( &m_pDrawRenderStats );
	}

	return S_OK;
}

HRESULT DxEffectGround::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	VAILEDDEVICE = FALSE;

	SAFE_RELEASE( m_pSavedRenderStats );
	SAFE_RELEASE( m_pDrawRenderStats );

	return S_OK;
}

void DxEffectGround::OnInitDevice_STATIC()
{
	m_pPool = new CMemPool<DxEffectGround>;
}

void DxEffectGround::OnDeleteDevice_STATIC()
{
	SAFE_DELETE(m_pPool);
}

void DxEffectGround::CheckAABBBox( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin )
{
	D3DXVECTOR3 vPos;
	for( DWORD i=0; i<4; ++i )
	{
		D3DXVec3TransformCoord( &vPos, &m_pGroundVertex[i].vPos, &m_matWorld );

		if( vMax.x < vPos.x )	vMax.x = vPos.x;
		if( vMax.y < vPos.y )	vMax.y = vPos.y;
		if( vMax.z < vPos.z )	vMax.z = vPos.z;

		if( vMin.x > vPos.x )	vMin.x = vPos.x;
		if( vMin.y > vPos.y )	vMin.y = vPos.y;
		if( vMin.z > vPos.z )	vMin.z = vPos.z;
	}

	if( m_pChild )		m_pChild->CheckAABBBox( vMax, vMin );
	if( m_pSibling )	m_pSibling->CheckAABBBox( vMax, vMin );
}

HRESULT DxEffectGround::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	//	Note : 초기 위치 생성
	//
	float TempXZ = m_fSize/2;
	m_pGroundVertex[0].vPos = D3DXVECTOR3 ( -TempXZ, m_fHeight, TempXZ );
	m_pGroundVertex[1].vPos = D3DXVECTOR3 ( TempXZ, m_fHeight, TempXZ );
	m_pGroundVertex[2].vPos = D3DXVECTOR3 ( -TempXZ, m_fHeight, -TempXZ );
	m_pGroundVertex[3].vPos = D3DXVECTOR3 ( TempXZ, m_fHeight, -TempXZ );

	//	Note : 컬러값 초기화
	//
	m_cColorDelta.a = 0.f;
	m_cColorDelta.r = ( m_cColorEnd.r - m_cColorStart.r ) / m_fGLifeTime;
	m_cColorDelta.g = ( m_cColorEnd.g - m_cColorStart.g ) / m_fGLifeTime;
	m_cColorDelta.b = ( m_cColorEnd.b - m_cColorStart.b ) / m_fGLifeTime;

	m_pGroundVertex[0].dwColor = m_pGroundVertex[1].dwColor = m_pGroundVertex[2].dwColor = 
	m_pGroundVertex[3].dwColor = m_cColorStart;
	m_dwColor = m_cColorStart;

	m_pGroundVertex[0].vTex01 = D3DXVECTOR2 ( 0.f, 0.f );
	m_pGroundVertex[1].vTex01 = D3DXVECTOR2 ( 1.f, 0.f );
	m_pGroundVertex[2].vTex01 = D3DXVECTOR2 ( 0.f, 1.f );
	m_pGroundVertex[3].vTex01 = D3DXVECTOR2 ( 1.f, 1.f );

	m_pGroundVertex[0].vTex02 = D3DXVECTOR2 ( 0.f, 0.f );
	m_pGroundVertex[1].vTex02 = D3DXVECTOR2 ( 1.f, 0.f );
	m_pGroundVertex[2].vTex02 = D3DXVECTOR2 ( 0.f, 1.f );
	m_pGroundVertex[3].vTex02 = D3DXVECTOR2 ( 1.f, 1.f );

	// 
	memset( &m_sMaterial, 0, sizeof(D3DMATERIAL9) );

	//	Note : 형제, 자식 호출
	DxEffSingle::RestoreDeviceObjects ( pd3dDevice );

	return S_OK;
}

HRESULT DxEffectGround::InvalidateDeviceObjects ()
{
	//	Note : 형제, 자식 호출
	//
	DxEffSingle::InvalidateDeviceObjects ();

	return S_OK;
}

void DxEffectGround::ReStartEff ()
{
	m_fGAge		= 0.f;
	m_fSize		= m_fSizeStart;
	m_fAlpha	= m_fAlphaStart;
	m_fHeight	= m_fHeightStart;
	m_dwRunFlag &= ~EFF_PLY_PLAY;
	m_dwRunFlag &= ~EFF_PLY_DONE;
	m_dwRunFlag &= ~EFF_PLY_END;

	if ( m_pChild )		m_pChild->ReStartEff ();
	if ( m_pSibling )	m_pSibling->ReStartEff ();
}

HRESULT DxEffectGround::FrameMove ( float fTime, float fElapsedTime )
{
	//	Note : Eff의 나이를 계산.
	//
	m_fGAge += fElapsedTime;
	m_fTimeSum += fElapsedTime;

	//	Note : 반복할 때 시간 및 여러 값을 초기화 한다.
	//
	if ( m_pThisGroup->m_dwFlag&EFF_CFG_NEVERDIE )
	{
		if ( m_fGAge>=(m_pThisGroup->m_fGBeginTime+m_pThisGroup->m_fGLifeTime) )
		{
			m_fGAge		= 0.f;
			m_fSize		= m_fSizeStart;
			m_fAlpha	= m_fAlphaStart;
			m_fHeight	= m_fHeightStart;
			m_dwRunFlag &= ~EFF_PLY_PLAY;
			m_dwRunFlag &= ~EFF_PLY_DONE;
			m_dwRunFlag &= ~EFF_PLY_END;
		}
	}

	if ( m_dwRunFlag & EFF_PLY_END )		goto _RETURN;

	//	Note : 시작되지 않았는지 검사, 시작 시간이면 활성화 시킴.
	//
	if ( !( m_dwRunFlag & EFF_PLY_PLAY ) )
	{
		if ( m_fGAge >= m_fGBeginTime )		m_dwRunFlag |= EFF_PLY_PLAY;
		else								goto _RETURN;
	}

	//	Note : 종료 시점이 되었는지 검사.
	//
	if ( IsReadyToDie() )					m_dwRunFlag |= EFF_PLY_END;

	//	Note : 회전 적용
	//
	if ( m_dwFlag & USEROTATE )	m_fRotateAngelSum += m_fRotateAngel * fElapsedTime;

	//	Note : 범위(사이즈) 적용
	//
	if ( m_fGAge >= m_fGBeginTime && m_fGAge < m_fSizeTime1)
		m_fSize += (m_fSizeDelta1*fElapsedTime)/2;
	else if ( m_fGAge >= m_fSizeTime1 && m_fGAge < m_fSizeTime2)	
		m_fSize += (m_fSizeDelta2*fElapsedTime)/2;
	else														
		m_fSize += (m_fSizeDelta3*fElapsedTime)/2;

	if ( m_fSize < 0.f )		m_fSize = 0.f;

	m_pGroundVertex[0].vPos = D3DXVECTOR3 ( -m_fSize, 0.f, m_fSize );
	m_pGroundVertex[1].vPos = D3DXVECTOR3 ( m_fSize, 0.f, m_fSize );
	m_pGroundVertex[2].vPos = D3DXVECTOR3 ( -m_fSize, 0.f, -m_fSize );
	m_pGroundVertex[3].vPos = D3DXVECTOR3 ( m_fSize, 0.f, -m_fSize );

	//	Note : 높이 조정
	//
	if ( m_dwFlag & USEHEIGHT )
	{
		if ( m_fGAge >= m_fGBeginTime && m_fGAge < m_fHeightTime1)
			m_fHeight += m_fHeightDelta1*fElapsedTime;
		else if ( m_fGAge >= m_fHeightTime1 && m_fGAge < m_fHeightTime2)	
			m_fHeight += m_fHeightDelta2*fElapsedTime;
		else																
			m_fHeight += m_fHeightDelta3*fElapsedTime;
	}

	//	Note : 컬러 조정 
	//
	if ( m_fGAge >= m_fGBeginTime && m_fGAge < m_fAlphaTime1)
		m_fAlpha += m_fAlphaDelta1*fElapsedTime;
	else if ( m_fGAge >= m_fAlphaTime1 && m_fGAge < m_fAlphaTime2)	
		m_fAlpha += m_fAlphaDelta2*fElapsedTime;
	else														
		m_fAlpha += m_fAlphaDelta3*fElapsedTime;

	if ( m_fAlpha < 0.f )		m_fAlpha = 0.f;
	if ( m_fAlpha > 1.f )		m_fAlpha = 1.f;

	m_cColorVar = m_cColorStart;
	m_cColorVar.a = m_fAlpha;	

	//m_pGroundVertex[0].Diffuse = m_pGroundVertex[1].Diffuse = m_pGroundVertex[2].Diffuse = 
	//m_pGroundVertex[3].Diffuse = m_cColorVar + m_cColorDelta*(m_fGAge-m_fGBeginTime);
	m_dwColor = m_cColorVar + m_cColorDelta*(m_fGAge-m_fGBeginTime);

	//	Note : 스프라이트
	//
	if ( m_dwFlag & USEANI )
	{
		if ( m_dwFlag & USESEQUENCELOOP )
		{
			if ( m_fTimeSum > m_fAniTime )
			{
				if ( m_bSequenceRoop )	m_iNowSprite += 1;
				else					m_iNowSprite -= 1;
				m_fTimeSum = 0.0f;
			}

			if ( m_iNowSprite >= m_iAllSprite )
			{
				m_bSequenceRoop = !m_bSequenceRoop;
				m_iNowSprite -= 2;
			}
			else if ( m_iNowSprite < 0 )
			{
				m_bSequenceRoop = !m_bSequenceRoop;
				m_iNowSprite += 2;
			}
		}
		else
		{
			if ( m_fTimeSum > m_fAniTime )
			{
				m_iNowSprite += 1;
				m_fTimeSum = 0.0f;
			}
			if ( m_iNowSprite >= m_iAllSprite )		m_iNowSprite = 0;
		}

		m_pGroundVertex[0].vTex01.x = m_iNowSprite / (float)m_iCol;
		m_pGroundVertex[0].vTex01.y = (m_iNowSprite / m_iCol) / (float)m_iRow;

		m_pGroundVertex[1].vTex01.x = (m_iNowSprite+1) / (float)m_iCol;
		m_pGroundVertex[1].vTex01.y = (m_iNowSprite / m_iCol) / (float)m_iRow;

		m_pGroundVertex[2].vTex01.x = m_iNowSprite / (float)m_iCol;
		m_pGroundVertex[2].vTex01.y = ((m_iNowSprite) / m_iCol) / (float)m_iRow + 1.f/(float)m_iRow;

		m_pGroundVertex[3].vTex01.x = (m_iNowSprite+1) / (float)m_iCol;
		m_pGroundVertex[3].vTex01.y = ((m_iNowSprite) / m_iCol) / (float)m_iRow + 1.f/(float)m_iRow;
	}

_RETURN:
	//	Note : 형제, 자식 노드.
	//
	DxEffSingle::FrameMove ( fTime, fElapsedTime );

	return S_OK;
}

HRESULT	DxEffectGround::Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matComb )
{
	HRESULT hr = S_OK;
	D3DXMATRIX matWorld, matRotate, matTrans, matChild;

	D3DXVECTOR3	vDelta(0.f,0.f,0.f);
	matWorld = m_matLocal;

	//	Note : 높이가 변한다
	if ( m_dwFlag & USEPICKING )	// 지면에 위치가 고정된다.
	{
		D3DXMatrixMultiply ( &matChild, &matWorld, &matComb );		// matTrans = matComb
		matChild._42 += 0.3f;			// 높이를 건들지 못하도록 한다.

		DxLandMan*	pLandMan = DxEffSingleMan::GetInstance().GetLandMan();

		if ( (m_vPrevPicking.x==matChild._41) && (m_vPrevPicking.z==matChild._43) )		// 예전하고 같은 위치이면 Picking 계산 안함
		{
			matChild._42 = m_vPrevPicking.y;
		}
		else if ( pLandMan )															// 처음보는 위치이면 Picking 계산함
		{
			BOOL		bCollision;
			D3DXVECTOR3	vCollision;
			D3DXVECTOR3	vPoint1(matChild._41,matChild._42+5.f,	matChild._43);		// 뿌려지는 중심 위치 구하기
			D3DXVECTOR3	vPoint2(matChild._41,matChild._42-100.f,matChild._43);		// 뿌려지는 중심 위치 구하기
			LPDXFRAME	pDxFrame = NULL;

			pLandMan->IsCollision ( vPoint1, vPoint2, vCollision, bCollision, pDxFrame, TRUE );

			matChild._42 = vCollision.y + 0.3f;
		}
	}
	else if ( m_dwFlag & USEHEIGHT )
	{
		if ( m_dwFlag & USEROTATE )			//	Note : 회전 적용
		{
			D3DXMatrixRotationY ( &matRotate, m_fRotateAngelSum*D3DX_PI/180.f );
			//D3DXMatrixMultiply ( &matWorld, &matRotate, &matWorld );
			D3DXMatrixMultiply_RM2( matRotate, matWorld );
		}

		D3DXMatrixMultiply ( &matChild, &matWorld, &matComb );		// matTrans = matComb
		matChild._42 += (m_fHeight+0.3f);
	}
	else	// 높이가 변하지 않는다.
	{
		if ( m_dwFlag & USEROTATE )			//	Note : 회전 적용
		{
			D3DXMatrixRotationY ( &matRotate, m_fRotateAngelSum*D3DX_PI/180.f );
			//D3DXMatrixMultiply ( &matWorld, &matRotate, &matWorld );
			D3DXMatrixMultiply_RM2( matRotate, matWorld );
		}

		D3DXMatrixMultiply ( &matChild, &matWorld, &matComb );		// matTrans = matComb
		matChild._42 += 0.3f;			// 높이를 건들지 못하도록 한다.
	}

	m_matWorld = matChild;


	if ( !m_pThisGroup->m_bOut_Particle )	goto _RETURN;
	if ( !(m_dwRunFlag&EFF_PLY_PLAY) )		goto _RETURN;
	if ( m_dwRunFlag&EFF_PLY_END )			goto _RETURN;
	if ( m_dwRunFlag&EFF_PLY_RENDPASS )		goto _RETURN;
	if ( m_fSize <= 0.f || m_fAlpha <= 0.f)	goto _RETURN;

	// Note : Get Texture
	if( !m_pTexture )	TextureManager::GetTexture( m_strTexture.c_str(), m_pTexture );
	if( !m_pTexture )	goto _RETURN;

	// Note : OptimizeSequence를 사용하기 위한 초기 작업.
	OPTMSingleGround::DATA* pData = OPTMManager::GetInstance().m_sSingleGround.GetData();

	pData->matWorld = matChild;

	if( m_dwFlag & USENORMAL )
	{
		pData->sLIGHTING.SetValue( TRUE );
		pData->sCOLOROP.SetValue( D3DTOP_MODULATE );
	}

	//	Note : 블렌딩 요소 셋팅
	switch ( m_nBlend )
	{
	case 1 :
		pData->sDESTBLEND.SetValue( D3DBLEND_ONE );
		break;
	case 2:
		pData->sDESTBLEND.SetValue( D3DBLEND_ONE );
		pData->sCOLOROP.SetValue( D3DTOP_MODULATE2X );
		break;
	case 3:
		pData->sDESTBLEND.SetValue( D3DBLEND_ONE );
		pData->sCOLOROP.SetValue( D3DTOP_MODULATE4X );
		break;
	case 5:
		pData->sZWRITEENABLE.SetValue( TRUE );
		pData->sALPHABLENDENABLE.SetValue( FALSE );
		pData->sALPHAREF.SetValue( 0x80 );

		break;
	}

	if ( m_dwFlag & USENORMAL )
	{
		for ( DWORD i=0; i<4; i++ )
		{
			m_pGroundVertNor[i].vPos	= m_pGroundVertex[i].vPos;
			m_pGroundVertNor[i].vNor	= D3DXVECTOR3 ( 0.f, 1.f, 0.f );
			m_pGroundVertNor[i].vTex	= m_pGroundVertex[i].vTex01;
		}

		m_sMaterial.Diffuse.a = (float)((m_dwColor&0xff000000)>>24)*DIV_1_255;
		m_sMaterial.Diffuse.r = (float)((m_dwColor&0x00ff0000)>>16)*DIV_1_255;
		m_sMaterial.Diffuse.g = (float)((m_dwColor&0x0000ff00)>>8)*DIV_1_255;
		m_sMaterial.Diffuse.b = (float)((m_dwColor&0x000000ff))*DIV_1_255;

		// Note : Lock을 하기 위한 D3DLOCK_DISCARD or D3DLOCK_NOOVERWRITE
		DWORD dwFlag = D3DLOCK_NOOVERWRITE; 
		DWORD dwVertexSizeFULL = 4*sizeof(VERTEX);
		if( DxDynamicVB::m_sVB.nOffsetToLock + dwVertexSizeFULL > DxDynamicVB::m_sVB.nFullByte )
		{
			//CDebugSet::ToLogFile( "DxDynamicVB - Limit - Over" );
			OPTMManager::GetInstance().Render( pd3dDevice );

			dwFlag = D3DLOCK_DISCARD; 
			DxDynamicVB::m_sVB.nVertexCount = 0;
			DxDynamicVB::m_sVB.nOffsetToLock = 0; 
		}

		// Note : 데이터를 넣는다.
		pData->pTex = m_pTexture;
		pData->nVertexCount = DxDynamicVB::m_sVB.nVertexCount;
		pData->nFaceCount = 2;
		pData->dwFVF = VERTEX::FVF;
		pData->pMaterial = &m_sMaterial;

		// Note : 데이터 복사
		VERTEX *pVertexB;
		hr = DxDynamicVB::m_sVB.pVB->Lock( DxDynamicVB::m_sVB.nOffsetToLock, dwVertexSizeFULL, (VOID**)&pVertexB, dwFlag );
		memcpy( pVertexB, m_pGroundVertNor, dwVertexSizeFULL );
		DxDynamicVB::m_sVB.pVB->Unlock();

		// Note : 값을 더해줌. 
		DxDynamicVB::m_sVB.nVertexCount += 4;
		DxDynamicVB::m_sVB.nOffsetToLock += dwVertexSizeFULL;

		// Note : 나중에 뿌리도록 셋팅 해 놓는다.
		OPTMManager::GetInstance().m_sSingleGround.InsertData( pData );
	}
	else
	{
		// 색 변경
		m_pGroundVertex[0].dwColor = m_pGroundVertex[1].dwColor = m_pGroundVertex[2].dwColor = 
		m_pGroundVertex[3].dwColor = m_dwColor;

		// Note : Lock을 하기 위한 D3DLOCK_DISCARD or D3DLOCK_NOOVERWRITE
		DWORD dwFlag = D3DLOCK_NOOVERWRITE; 
		DWORD dwVertexSizeFULL = 4*sizeof(VERTEXCOLORTEX2);
		if( DxDynamicVB::m_sVB.nOffsetToLock + dwVertexSizeFULL > DxDynamicVB::m_sVB.nFullByte )
		{
			//CDebugSet::ToLogFile( "DxDynamicVB - Limit - Over" );
			OPTMManager::GetInstance().Render( pd3dDevice );

			dwFlag = D3DLOCK_DISCARD; 
			DxDynamicVB::m_sVB.nVertexCount = 0;
			DxDynamicVB::m_sVB.nOffsetToLock = 0; 
		}

		// Note : 데이터를 넣는다.
		pData->pTex = m_pTexture;
		pData->nVertexCount = DxDynamicVB::m_sVB.nVertexCount;
		pData->nFaceCount = 2;
		pData->dwFVF = VERTEXCOLORTEX2::FVF;

		// Note : 데이터 복사
		VERTEXCOLORTEX2 *pVertexB;
		DxDynamicVB::m_sVB.pVB->Lock( DxDynamicVB::m_sVB.nOffsetToLock, dwVertexSizeFULL, (VOID**)&pVertexB, dwFlag );
		memcpy( pVertexB, m_pGroundVertex, dwVertexSizeFULL );
		DxDynamicVB::m_sVB.pVB->Unlock();

		// Note : 값을 더해줌. 
		DxDynamicVB::m_sVB.nVertexCount += 4;
		DxDynamicVB::m_sVB.nOffsetToLock += dwVertexSizeFULL;

		// Note : 나중에 뿌리도록 셋팅 해 놓는다.
		OPTMManager::GetInstance().m_sSingleGround.InsertData( pData );
	}

_RETURN:
	if ( FAILED(hr) )	return hr;

	//	Note : 형제, 자식 노드.
	//
	if ( m_pChild )		m_pChild->Render ( pd3dDevice, matChild );
	if ( m_pSibling )	m_pSibling->Render ( pd3dDevice, matComb );

	return S_OK;
}

HRESULT DxEffectGround::FinalCleanup()
{
	//	Note : 형제 자식 노드.
	//
	DxEffSingle::FinalCleanup ();

	return S_OK;
}

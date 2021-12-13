#include "pch.h"

#include "./DxViewPort.h"
#include "./TextureManager.h"
#include "./DxDynamicVB.h"

#include "./glperiod.h"

#include "./DxEffectMan.h"
#include "./DxEffSingleMan.h"
#include "./SerialFile.h"

#include "./DxEffectSequence.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//---------------------------------------------------------------------[SEQUENCE_PROPERTY]
const DWORD	SEQUENCE_PROPERTY::VERSION	= 0x0102;
const char	SEQUENCE_PROPERTY::NAME[]	= "시퀀스";

DxEffSingle* SEQUENCE_PROPERTY::NEWOBJ ()
{
	//DxEffectSequence *pEffect = new DxEffectSequence;
	DxEffectSequence *pEffect = DxEffectSequence::m_pPool->New();

	//	Note : TransPorm Set.		[공통설정]
	pEffect->m_matLocal = m_matLocal;

	pEffect->m_dwFlag = m_dwFlag;
	
	//	Note : 시간 설정.			[공통설정]
	pEffect->m_fGBeginTime = m_fGBeginTime;

	if( pEffect->m_dwFlag&USERANDOMLIFE )
	{
		pEffect->m_fGLifeTime = m_fGLifeTime*(RANDOM_POS*0.6f+0.7f);	// 0.7f~1.3f
	}
	else
	{
		pEffect->m_fGLifeTime = m_fGLifeTime;
	}

	//	Note : 날아가는 오브젝트	[공통설정]
	pEffect->m_bMoveObj = m_bMoveObj;

	//	Note : 속도 설정.
	pEffect->m_vGVelocity = m_vGVelocity;
	pEffect->m_vGVelocityStart = m_vGVelocity;

	//	Note : 가속도 설정.
	pEffect->m_vGGravity = m_vGGravityStart;
	pEffect->m_vGGravityStart = m_vGGravityStart;
	pEffect->m_vGGravityDelta = ( m_vGGravityEnd - m_vGGravityStart ) / m_fGLifeTime;

	//pEffect->m_fRotateTime1 = m_fGBeginTime + m_fGLifeTime*m_fRotateRate1/100.f;
	//pEffect->m_fRotateTime2 = m_fGBeginTime + m_fGLifeTime*m_fRotateRate2/100.f;

	pEffect->m_fFlarePos = m_fFlarePos;

	//	Note : 회전 반지름 초기화
	//	
	pEffect->m_fRotateAngel = m_fRotateAngel;

	pEffect->m_fLength	 = m_fLengthStart;
	pEffect->m_fLengthStart = m_fLengthStart;;

	pEffect->m_fRotateTime1 = m_fGBeginTime + m_fGLifeTime*m_fRotateRate1/100.f;
	pEffect->m_fRotateTime2 = m_fGBeginTime + m_fGLifeTime*m_fRotateRate2/100.f;

	if ( m_fRotateRate1 == 0.f )				pEffect->m_fRotateDelta1	= 0.f;
	else										pEffect->m_fRotateDelta1 = ( m_fLengthMid1 - m_fLengthStart ) / (pEffect->m_fRotateTime1-m_fGBeginTime);
	if ( m_fRotateRate2-m_fRotateRate1 == 0.f )	pEffect->m_fRotateDelta2	= 0.f;
	else										pEffect->m_fRotateDelta2 = ( m_fLengthMid2 - m_fLengthMid1 ) / (pEffect->m_fRotateTime2-pEffect->m_fRotateTime1);
	if ( m_fRotateRate2 == 100.f )				pEffect->m_fRotateDelta3	= 0.f;
	else										pEffect->m_fRotateDelta3 = ( m_fLengthEnd - m_fLengthMid2 ) / (m_fGBeginTime+m_fGLifeTime-pEffect->m_fRotateTime2);

	//	Note : 사이즈 초기화
	//
	pEffect->m_fSize	 = m_fSizeStart/2;
	pEffect->m_fSizeStart = m_fSizeStart/2;

	pEffect->m_fSizeTime1 = m_fGBeginTime + m_fGLifeTime*m_fSizeRate1/100.f;
	pEffect->m_fSizeTime2 = m_fGBeginTime + m_fGLifeTime*m_fSizeRate2/100.f;

	if ( m_fSizeRate1 == 0.f )				pEffect->m_fSizeDelta1	= 0.f;
	else									pEffect->m_fSizeDelta1 = ( m_fSizeMid1 - m_fSizeStart ) / (pEffect->m_fSizeTime1-m_fGBeginTime);
	if ( m_fSizeRate2-m_fSizeRate1 == 0.f )	pEffect->m_fSizeDelta2	= 0.f;
	else									pEffect->m_fSizeDelta2 = ( m_fSizeMid2 - m_fSizeMid1 ) / (pEffect->m_fSizeTime2-pEffect->m_fSizeTime1);
	if ( m_fSizeRate2 == 100.f )			pEffect->m_fSizeDelta3	= 0.f;
	else									pEffect->m_fSizeDelta3 = ( m_fSizeEnd - m_fSizeMid2 ) / (m_fGBeginTime+m_fGLifeTime-pEffect->m_fSizeTime2);

	pEffect->m_fWidthRate	= m_fWidthRate;
	pEffect->m_fHeightRate	= m_fHeightRate;

	//	Note : 알파..
	//
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

	//	Note : 시퀀스 설정
	//
	pEffect->m_iAllSprite = m_iCol * m_iRow;
	pEffect->m_fAniTime = m_fAniTime;
	pEffect->m_iCol = m_iCol;
	pEffect->m_iRow = m_iRow;

	//	Note : 텍스쳐 회전
	//
	pEffect->m_fTexRotateAngel = m_fTexRotateAngel;

	pEffect->m_nBlend = m_nBlend;

	//	Note : 텍스쳐 설정.
	StringCchCopy( pEffect->m_szTexture, 256, m_szTexture );
	//pEffect->m_strTexture = m_szTexture;
	pEffect->m_pTexture = m_pTexture;

	//	Note : 사운드 설정
	//
	pEffect->m_MovSound = m_MovSound;

	return pEffect;
}

HRESULT SEQUENCE_PROPERTY::SaveFile ( CSerialFile &SFile )
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

HRESULT SEQUENCE_PROPERTY::LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	//	Note : 버전이 일치할 경우. 
	//
	if ( dwVer == VERSION )
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
	else if ( dwVer == 0x0101 )
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
	else if ( dwVer == 0x0100 )
	{
		//	Note : 부모 클레스의 정보.
		DXAFFINEPARTS sAffineParts;
		SFile.ReadBuffer( m_matLocal, sizeof(D3DXMATRIX) );
		SFile.ReadBuffer( &sAffineParts, sizeof(DXAFFINEPARTS) );
		D3DXMatrixCompX( m_matLocal, sAffineParts );
		
		SFile >> m_bMoveObj;
		SFile >> m_fGBeginTime;
		SFile >> m_fGLifeTime;

		PROPERTY_100	pProp;

		//	Note : 이팩트의 Property 를 저장.
		//
		SFile.ReadBuffer ( &pProp, sizeof(PROPERTY_100) );

		m_Property.m_bGIsColliding	= pProp.m_bGIsColliding;
		m_Property.m_bTexRotateUse	= pProp.m_bTexRotateUse;
		m_Property.m_cColorEnd		= pProp.m_cColorEnd;
		m_Property.m_cColorStart	= pProp.m_cColorStart;
		m_Property.m_cColorVar		= pProp.m_cColorVar;
		m_Property.m_dwFlag			= pProp.m_dwFlag;
		m_Property.m_fAlphaEnd		= pProp.m_fAlphaEnd;
		m_Property.m_fAlphaMid1		= pProp.m_fAlphaMid1;
		m_Property.m_fAlphaMid2		= pProp.m_fAlphaMid2;
		m_Property.m_fAlphaRate1	= pProp.m_fAlphaRate1;
		m_Property.m_fAlphaRate2	= pProp.m_fAlphaRate2;
		m_Property.m_fAlphaStart	= pProp.m_fAlphaStart;
		m_Property.m_fAniTime		= pProp.m_fAniTime;
		m_Property.m_fFlarePos		= pProp.m_fFlarePos;
		m_Property.m_fLengthEnd		= pProp.m_fLengthEnd;
		m_Property.m_fLengthMid1	= pProp.m_fLengthMid1;
		m_Property.m_fLengthMid2	= pProp.m_fLengthMid2;
		m_Property.m_fLengthStart	= pProp.m_fLengthStart;
		m_Property.m_fRotateAngel	= pProp.m_fRotateAngel;
		m_Property.m_fRotateRate1	= pProp.m_fRotateRate1;
		m_Property.m_fRotateRate2	= pProp.m_fRotateRate2;
		m_Property.m_fSizeEnd		= pProp.m_fSizeEnd;
		m_Property.m_fSizeMid1		= pProp.m_fSizeMid1;
		m_Property.m_fSizeMid2		= pProp.m_fSizeMid2;
		m_Property.m_fSizeStart		= pProp.m_fSizeStart;
		m_Property.m_fTexRotateAngel = pProp.m_fTexRotateAngel;
		m_Property.m_iCol			= pProp.m_iCol;
		m_Property.m_iRow			= pProp.m_iRow;
		m_Property.m_nBlend			= pProp.m_nBlend;
		m_Property.m_vGGravityEnd	= pProp.m_vGGravityEnd;
		m_Property.m_vGGravityStart	= pProp.m_vGGravityStart;
		m_Property.m_vGVelocity		= pProp.m_vGVelocity;

		StringCchCopy( m_Property.m_szTexture, 256, pProp.m_szTexture );

		m_Property.m_dwFlag |= USEBILLBOARD;
		m_Property.m_fWidthRate = 1.f;
		m_Property.m_fHeightRate = 1.f;

		m_pTexture = NULL;
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

HRESULT SEQUENCE_PROPERTY::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	TextureManager::LoadTexture ( m_szTexture, pd3dDevice, m_pTexture, 0, 0, TRUE );

	//	Note : 형제, 자식.
	//
	EFF_PROPERTY::InitDeviceObjects ( pd3dDevice );

	return S_OK;
}

HRESULT SEQUENCE_PROPERTY::DeleteDeviceObjects ()
{
	TextureManager::ReleaseTexture( m_szTexture, m_pTexture );

	//	Note : 형제, 자식.
	//
	EFF_PROPERTY::DeleteDeviceObjects ();

	return S_OK;
}

//-------------------------------------------- [정적 맴버 정의 및 초기화] ----------------------------------------------
//
//
const DWORD	DxEffectSequence::TYPEID	= EFFSINGLE_SEQUENCE;
const DWORD	DxEffectSequence::FLAG		= NULL;
const char	DxEffectSequence::NAME[]	= "시퀀스";

CMemPool<DxEffectSequence> *DxEffectSequence::m_pPool = NULL;

LPDIRECT3DSTATEBLOCK9		DxEffectSequence::m_pSavedRenderStats = NULL;
LPDIRECT3DSTATEBLOCK9		DxEffectSequence::m_pDrawRenderStats = NULL;

HRESULT DxEffectSequence::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;	

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		// 변하지 않음 ( 효과 Default 값. )
		pd3dDevice->SetRenderState( D3DRS_COLORVERTEX,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,			0x00000000 );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_ALPHAREF,			0x01 );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,		D3DTA_TFACTOR );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2,		D3DTA_TFACTOR );

		// 계속 변하는 값 ( Default랑 틀림. 효과 Default 값. )
		pd3dDevice->SetRenderState( D3DRS_CULLMODE,			D3DCULL_CCW );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,		D3DTOP_MODULATE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,		D3DTOP_MODULATE );

		// 계속 변하는 값 ( Default로 설정해야 함. )
		pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xffffffff );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_INVSRCALPHA );

		pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU,		D3DTADDRESS_WRAP );
		pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV,		D3DTADDRESS_WRAP );
		pd3dDevice->SetSamplerState( 0, D3DSAMP_BORDERCOLOR,	0x00000000 );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedRenderStats );
		else			pd3dDevice->EndStateBlock( &m_pDrawRenderStats );
	}

	return S_OK;
}

HRESULT DxEffectSequence::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//SAFE_RELEASE(m_pVBuffer);

	SAFE_RELEASE( m_pSavedRenderStats );
	SAFE_RELEASE( m_pDrawRenderStats );

	return S_OK;
}

void DxEffectSequence::OnInitDevice_STATIC()
{
	m_pPool = new CMemPool<DxEffectSequence>;
}

void DxEffectSequence::OnDeleteDevice_STATIC()
{
	SAFE_DELETE(m_pPool);
}

void DxEffectSequence::CheckAABBBox( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin )
{
	D3DXVECTOR3 vPos;
	for( DWORD i=0; i<4; ++i )
	{
		D3DXVec3TransformCoord( &vPos, &m_sSequenceVB[i].vPos, &m_matWorld );

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

HRESULT DxEffectSequence::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	//	Note : 컬러값 초기화
	//
	m_cColorDelta.a = 0.f;
	m_cColorDelta.r = ( m_cColorEnd.r - m_cColorStart.r ) / m_fGLifeTime;
	m_cColorDelta.g = ( m_cColorEnd.g - m_cColorStart.g ) / m_fGLifeTime;
	m_cColorDelta.b = ( m_cColorEnd.b - m_cColorStart.b ) / m_fGLifeTime;

	//	Note : 초기 위치 생성
	float TempXZ = m_fSize/2;
	m_sSequenceVB[0].vPos = D3DXVECTOR3 ( -TempXZ, 0.f, TempXZ );
	m_sSequenceVB[1].vPos = D3DXVECTOR3 ( TempXZ, 0.f, TempXZ );
	m_sSequenceVB[2].vPos = D3DXVECTOR3 ( -TempXZ, 0.f, -TempXZ );
	m_sSequenceVB[3].vPos = D3DXVECTOR3 ( TempXZ, 0.f, -TempXZ );

	m_sSequenceVB[0].dwColor = 0x00000000;
	m_sSequenceVB[1].dwColor = 0x00000000;
	m_sSequenceVB[2].dwColor = 0x00000000;
	m_sSequenceVB[3].dwColor = 0x00000000;

	m_sSequenceVB[0].vTex01 = D3DXVECTOR2 ( 0.f, 0.f );
	m_sSequenceVB[1].vTex01 = D3DXVECTOR2 ( 1.f, 0.f );
	m_sSequenceVB[2].vTex01 = D3DXVECTOR2 ( 0.f, 1.f );
	m_sSequenceVB[3].vTex01 = D3DXVECTOR2 ( 1.f, 1.f );

	m_sSequenceVB[0].vTex02 = D3DXVECTOR2 ( 0.f, 0.f );
	m_sSequenceVB[1].vTex02 = D3DXVECTOR2 ( 1.f, 0.f );
	m_sSequenceVB[2].vTex02 = D3DXVECTOR2 ( 0.f, 1.f );
	m_sSequenceVB[3].vTex02 = D3DXVECTOR2 ( 1.f, 1.f );

	//	Note : 형제, 자식 호출
	//
	DxEffSingle::RestoreDeviceObjects ( pd3dDevice );

	return S_OK;
}

HRESULT DxEffectSequence::InvalidateDeviceObjects ()
{
	//	Note : 형제, 자식 호출
	//
	DxEffSingle::InvalidateDeviceObjects ();

	return S_OK;
}

//-------------------------------------------- [클레스 본체] ------------------------------------------------------------
//
//
DxEffectSequence::DxEffectSequence () :
	m_pTexture(NULL),

	m_fTimeSum(0.f),
	m_iAllSprite(1),
	m_iNowSprite(0),

	m_vGVelocity(D3DXVECTOR3 (0,0,0)),
	m_vGGravity(D3DXVECTOR3 (0,0,0)),
	m_vGGravityDelta(D3DXVECTOR3(0,0,0)),
	m_fRotateAngel(0.f),

	m_fRotateDelta1(0.f),		// 범위	회전
	m_fRotateDelta2(0.f),
	m_fRotateDelta3(0.f),
	m_fRotateTime1(0.f),
	m_fRotateTime2(0.f),

	m_vGLocation(0,0,0),
	m_vGPrevLocation(0,0,0),
	m_fRotateAngelSum(0.f),
	m_fTexRotateAngelSum(0.f),

	m_vPlayPos(0.f,0.f,0.f),

	m_fFlarePos(0.0f),

	m_bSequenceRoop(FALSE),

	m_pd3dDevice(NULL)
{
	m_fElapsedTime		= 0.f;

	m_fAlpha			= 0.f;

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

	m_nBlend			= 1;

	m_fGLifeTime		= 0.0f;

	D3DXMatrixIdentity( &m_matWorld );
}

DxEffectSequence::~DxEffectSequence ()
{
	CleanUp ();
}

HRESULT DxEffectSequence::FinalCleanup()
{
//	DxEffSingle::StopEffSound ();
	//	Note : 형제 자식 노드.
	//
	DxEffSingle::FinalCleanup ();

	return S_OK;
}

void DxEffectSequence::ReStartEff ()
{
	m_fGAge		= 0.f;
	m_fSize		= m_fSizeStart;
	m_fAlpha	= m_fAlphaStart;
	m_fLength	= m_fLengthStart;
	m_dwRunFlag &= ~EFF_PLY_PLAY;
	m_dwRunFlag &= ~EFF_PLY_DONE;
	m_dwRunFlag &= ~EFF_PLY_END;

	if ( m_pChild )		m_pChild->ReStartEff ();
	if ( m_pSibling )	m_pSibling->ReStartEff ();
}

HRESULT DxEffectSequence::FrameMove ( float fTime, float fElapsedTime )
{
	D3DXVECTOR3		vDeltaGVel;
	D3DXVECTOR3		vLocal;

	//	Note : Eff의 나이를 계산.
	//
	m_fGAge += fElapsedTime;
	m_fTimeSum += fElapsedTime;
	m_fElapsedTime = fElapsedTime;

	//	Note : 반복할 때 시간 및 여러 값을 초기화 한다.
	//
	if ( m_pThisGroup->m_dwFlag&EFF_CFG_NEVERDIE )
	{
		if ( m_fGAge>=(m_pThisGroup->m_fGBeginTime+m_pThisGroup->m_fGLifeTime) )
		{
			m_fGAge		= 0.f;
			m_fSize		= m_fSizeStart;
			m_fAlpha	= m_fAlphaStart;
			m_fLength	= m_fLengthStart;
			m_dwRunFlag &= ~EFF_PLY_PLAY;
			m_dwRunFlag &= ~EFF_PLY_DONE;
			m_dwRunFlag &= ~EFF_PLY_END;

			if( m_dwFlag&USEDEFAULTPOS )
			{
				m_vGLocation = D3DXVECTOR3( 0.f, 0.f, 0.f );
				m_vGGravity = m_vGGravityStart;
				m_vGVelocity = m_vGVelocityStart;
			}
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

	//	Note : 위치 이동.
	//
	m_vGPrevLocation = m_vGLocation;
	m_vGLocation += m_vGVelocity * fElapsedTime;


	//	Note : 지면과 충돌시
	//
	if ( m_dwFlag & USECOLLISION )
	{
		if ( m_vGLocation.y < 0.0f )
		{
			m_vGVelocity.y = -m_vGVelocity.y*0.7f;
			m_vGLocation.y += m_vGVelocity.y * fElapsedTime;
		}
	}

	//	Note : 속도 변화. [ GRAVITY 문제가 있는거 같음. ]  - [수정필요]
	//
	vDeltaGVel = m_vGGravity*fElapsedTime;

	m_vGVelocity += vDeltaGVel;

	//	Note : 중력 변화.
	//
	m_vGGravity += m_vGGravityDelta * fElapsedTime;

	//	Note : 회전 적용
	//
	if ( !( m_dwFlag & USEROTATE ) )	m_vPlayPos = m_vGLocation;	// 회전을 적용하지 않는다면...
	else
	{
		D3DXVECTOR3		vCenter ( 0.f, 0.f, 0.f );
		D3DXVECTOR3		vPoint ( 1.f, 0.f, 0.f );		
		D3DXVECTOR3		vTempPoint1 ( -0.01f, 10000.f, 0.f );
		D3DXVECTOR3		vTempPoint2 ( -0.01f, -10000.f, 0.f );
		D3DXPLANE		vPlane;										
		D3DXVECTOR3		tempVelocity;

		tempVelocity = ( m_vGLocation - m_vGPrevLocation ) / fElapsedTime;

		if ( tempVelocity.y <= 0.01f && tempVelocity.y >= -0.01f )		vPoint = D3DXVECTOR3 ( 0.f, 1.f, 0.f );
		else
		{
			D3DXPlaneFromPointNormal ( &vPlane, &vCenter, &tempVelocity );
			vPoint = DxSplit ( &vPlane, &vTempPoint1, &vTempPoint2 );
			D3DXVec3Normalize ( &vPoint, &vPoint );
		}
		
		//	Note : 회전 반지름 - FrameMove
		//
		if ( m_fGAge >= m_fGBeginTime && m_fGAge < m_fRotateTime1)
			m_fLength +=  m_fRotateDelta1*fElapsedTime;
		else if ( m_fGAge >= m_fRotateTime1 && m_fGAge < m_fRotateTime2)	
			m_fLength +=  m_fRotateDelta2*fElapsedTime;
		else														
			m_fLength +=  m_fRotateDelta3*fElapsedTime;

		D3DXMATRIX matRotate;
		vPoint *= m_fLength;
		m_fRotateAngelSum += m_fRotateAngel * fElapsedTime * 360.f;
		if ( tempVelocity.y > 0.f )	D3DXMatrixRotationAxis ( &matRotate ,&tempVelocity , m_fRotateAngelSum*D3DX_PI/180.f );	// 회전 매트릭스 구함
		else						D3DXMatrixRotationAxis ( &matRotate ,&tempVelocity , m_fRotateAngelSum*D3DX_PI/180.f + D3DX_PI );	// 회전 매트릭스 구함

		D3DXVec3TransformCoord ( &vCenter, &vPoint ,&matRotate );

		m_vPlayPos += m_vGLocation + vCenter;
	}

	m_vGPrevLocation = m_vGLocation;

	//	Note : 사이즈 적용
	//
	if ( m_fGAge >= m_fGBeginTime && m_fGAge < m_fSizeTime1)
		m_fSize += (m_fSizeDelta1*fElapsedTime)/2;
	else if ( m_fGAge >= m_fSizeTime1 && m_fGAge < m_fSizeTime2)	
		m_fSize += (m_fSizeDelta2*fElapsedTime)/2;
	else														
		m_fSize += (m_fSizeDelta3*fElapsedTime)/2;

	if ( m_fSize < 0.f )		m_fSize = 0.f;

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

		m_sSequenceVB[0].vTex01.x = m_iNowSprite / (float)m_iCol;
		m_sSequenceVB[0].vTex01.y = (m_iNowSprite / m_iCol) / (float)m_iRow;

		m_sSequenceVB[1].vTex01.x = (m_iNowSprite+1) / (float)m_iCol;
		m_sSequenceVB[1].vTex01.y = (m_iNowSprite / m_iCol) / (float)m_iRow;

		m_sSequenceVB[2].vTex01.x = m_iNowSprite / (float)m_iCol;
		m_sSequenceVB[2].vTex01.y = (m_iNowSprite / m_iCol) / (float)m_iRow + 1.f/(float)m_iRow;

		m_sSequenceVB[3].vTex01.x = (m_iNowSprite+1) / (float)m_iCol;
		m_sSequenceVB[3].vTex01.y = (m_iNowSprite / m_iCol) / (float)m_iRow + 1.f/(float)m_iRow;
	}

_RETURN:
	//	Note : 형제, 자식 노드.
	//
	DxEffSingle::FrameMove ( fTime, fElapsedTime );

    return S_OK;
}

HRESULT DxEffectSequence::Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matComb )
{
	HRESULT hr = S_OK;
	D3DXMATRIX matWorld, matRotate, matRotateL, matTrans, matIdentity;

	float		fTime;
	D3DXVECTOR3	vScale;
	D3DXVECTOR3	vDelta(0.f,0.f,0.f);
	matTrans = m_matLocal;

	matTrans._41 *= m_pThisGroup->m_fOut_Scale;	// 스케일 
	matTrans._42 *= m_pThisGroup->m_fOut_Scale;
	matTrans._43 *= m_pThisGroup->m_fOut_Scale;

	if( m_dwFlag&NOT_WORLD_RS )
	{
		matComb._11 = 1.f;	matComb._12 = 0.f;	matComb._13 = 0.f;	matComb._14 = 0.f;
		matComb._21 = 0.f;	matComb._22 = 1.f;	matComb._23 = 0.f;	matComb._24 = 0.f;
		matComb._31 = 0.f;	matComb._32 = 0.f;	matComb._33 = 1.f;	matComb._34 = 0.f;
																	matComb._44 = 1.f;
	}

	float			fLength;
	float			fSize;
	D3DXVECTOR3		Sub11( 0.f, 0.f, 0.f );	
	D3DXVECTOR3		vSubDis11;
	D3DXVECTOR3		vLocal;
	D3DXVECTOR3&	vFromPt		= DxViewPort::GetInstance().GetFromPt();
	D3DXMATRIX		matLocal;
	D3DXVECTOR3		vPlayPos;

	//	Note : 컬러 조정 
	//
	if ( m_fGAge >= m_fGBeginTime && m_fGAge < m_fAlphaTime1)
		m_fAlpha += m_fAlphaDelta1*m_fElapsedTime;
	else if ( m_fGAge >= m_fAlphaTime1 && m_fGAge < m_fAlphaTime2)	
		m_fAlpha += m_fAlphaDelta2*m_fElapsedTime;
	else														
		m_fAlpha += m_fAlphaDelta3*m_fElapsedTime;

	if ( m_fAlpha < 0.f )		m_fAlpha = 0.f;
	if ( m_fAlpha > 1.f )		m_fAlpha = 1.f;

	m_cColorVar = m_cColorStart;
	m_cColorVar.a = m_fAlpha;	
	D3DXCOLOR	cColor = m_cColorVar + m_cColorDelta*(m_fGAge-m_fGBeginTime);
	D3DCOLOR	dwColor = ((DWORD)(cColor.a*255.f)<<24) +
							((DWORD)(cColor.r*255.f)<<16) +
							((DWORD)(cColor.g*255.f)<<8) +
							(DWORD)(cColor.b*255.f);

	//	Note : 내가 이동 물체다.
	//
	if ( m_dwFlag & USEGOTOCENTER )
	{
		fTime = m_fGBeginTime+m_fGLifeTime-m_fGAge;

		vDelta.x = m_matLocal._41/m_fGLifeTime;
		vDelta.y = m_matLocal._42/m_fGLifeTime;
		vDelta.z = m_matLocal._43/m_fGLifeTime;

		matTrans._41 = vDelta.x*fTime;
		matTrans._42 = vDelta.y*fTime;
		matTrans._43 = vDelta.z*fTime;
	}

	D3DXMatrixMultiply ( &matWorld, &matTrans, &matComb );

	//	Note : 빌보드에 필요한 매트릭스를 구함
	//
	D3DXVec3TransformCoord ( &vPlayPos, &m_vPlayPos, &matWorld );

	if( !m_pThisGroup->m_bOut_Particle )	goto _RETURN;
	if( !(m_dwRunFlag&EFF_PLY_PLAY) )		goto _RETURN;
	if( m_dwRunFlag&EFF_PLY_END )			goto _RETURN;
	if( m_dwRunFlag&EFF_PLY_RENDPASS )		goto _RETURN;
	if( m_fSize <= 0.f || m_fAlpha <= 0.f)	goto _RETURN;

	if( m_dwFlag&USELIGHTING && 
		GLPeriod::GetInstance().IsOffLight() && 
		DxLightMan::GetInstance()->IsWhenDayPointOff() )	goto _RETURN;

	// Note : Get Texture
	if( !m_pTexture )	TextureManager::GetTexture( m_szTexture, m_pTexture );
	if( !m_pTexture )	goto _RETURN;

	// Note : OptimizeSequence를 사용하기 위한 초기 작업.
	OPTMSingleSequence::DATA* pData = OPTMManager::GetInstance().m_sSingleSequence.GetData();

	//	Note : 물체에서 카메라로
	//
	Sub11.x = vFromPt.x - vPlayPos.x;
	Sub11.y = vFromPt.y - vPlayPos.y;
	Sub11.z = vFromPt.z - vPlayPos.z;

	fLength = D3DXVec3Length ( &Sub11 );			// 거리에 따른

	fSize = m_fSize * m_pThisGroup->m_fOut_Scale;		//	추가 스케일

	//	Note : 동적으로 스케일이 바뀐다.
	//
	if ( m_dwFlag&USEDYNAMICSCALE )	
	{
		fSize = m_fSize * fLength * 0.02f;				// 거리에 따른
	}



	if ( m_dwFlag&USEBILLBOARD )
	{
		D3DXVec3Normalize ( &Sub11, &Sub11 );
		vSubDis11 = Sub11 * m_fFlarePos;
		vPlayPos += vSubDis11;

		matLocal	= DxBillboardLookAt ( &vPlayPos, DxViewPort::GetInstance().GetMatView() );


		//	Note : 텍스쳐 회전 적용 안 함 
		//
		if ( !( m_dwFlag & USETEXROTATE ) )	
		{
			vLocal = D3DXVECTOR3( -fSize/2*m_fWidthRate, fSize/2*m_fHeightRate, 0.f );
			D3DXVec3TransformCoord ( &m_sSequenceVB[0].vPos, &vLocal, &matLocal );
			vLocal = D3DXVECTOR3( fSize/2*m_fWidthRate, fSize/2*m_fHeightRate, 0.f );
			D3DXVec3TransformCoord ( &m_sSequenceVB[1].vPos, &vLocal, &matLocal );
			vLocal = D3DXVECTOR3( -fSize/2*m_fWidthRate, -fSize/2*m_fHeightRate, 0.f );
			D3DXVec3TransformCoord ( &m_sSequenceVB[2].vPos, &vLocal, &matLocal );
			vLocal = D3DXVECTOR3( fSize/2*m_fWidthRate, -fSize/2*m_fHeightRate, 0.f );
			D3DXVec3TransformCoord ( &m_sSequenceVB[3].vPos, &vLocal, &matLocal );
		}
		else
		{
			//	Note : 텍스쳐 회전 적용
			//
			m_fTexRotateAngelSum += m_fTexRotateAngel * m_fElapsedTime;

			vLocal = D3DXVECTOR3( fSize*sinf( m_fTexRotateAngelSum )*m_fWidthRate, 
									fSize*cosf( m_fTexRotateAngelSum )*m_fHeightRate, 
									0.f );
			D3DXVec3TransformCoord ( &m_sSequenceVB[0].vPos, &vLocal, &matLocal );

			vLocal = D3DXVECTOR3( fSize*sinf( m_fTexRotateAngelSum+(D3DX_PI/2.f) )*m_fWidthRate, 
									fSize*cosf( m_fTexRotateAngelSum+(D3DX_PI/2.f) )*m_fHeightRate, 
									0.f );
			D3DXVec3TransformCoord ( &m_sSequenceVB[1].vPos, &vLocal, &matLocal );

			vLocal = D3DXVECTOR3( fSize*sinf( m_fTexRotateAngelSum+(D3DX_PI*1.5f) )*m_fWidthRate, 
									fSize*cosf( m_fTexRotateAngelSum+(D3DX_PI*1.5f) )*m_fHeightRate, 
									0.f );
			D3DXVec3TransformCoord ( &m_sSequenceVB[2].vPos, &vLocal, &matLocal );

			vLocal = D3DXVECTOR3( fSize*sinf( m_fTexRotateAngelSum+D3DX_PI )*m_fWidthRate, 
									fSize*cosf( m_fTexRotateAngelSum+D3DX_PI )*m_fHeightRate, 
									0.f );
			D3DXVec3TransformCoord ( &m_sSequenceVB[3].vPos, &vLocal, &matLocal );
		}

		D3DXMatrixIdentity ( &matIdentity );	// 빌보드일 때는 Identity 를...
		pData->bChangeMatrix = FALSE;
		m_matWorld = matIdentity;
	}
	else if ( m_dwFlag&USEDIRECTION )
	{
		if ( m_pThisGroup->m_bOut_Direct )
		{
			D3DXMatrixIdentity ( &matIdentity );
			D3DXMatrixBillboard ( matIdentity, &m_pThisGroup->m_vDirect );
			//D3DXMatrixMultiply ( &matWorld, &matIdentity, &matWorld  );	// 회전
			D3DXMatrixMultiply_RM2( matIdentity, matWorld  );	// 회전
			
			pData->bChangeMatrix = TRUE;
			pData->matWorld = matWorld;

			m_matWorld = matWorld;
		}
		else
		{
			D3DXVECTOR3	vStartPos(m_matLocal._41,m_matLocal._42,m_matLocal._43);
			vStartPos = m_pThisGroup->m_TargetID.vPos - vStartPos;
			if ( vStartPos.x != 0.f || vStartPos.y != 0.f || vStartPos.z != 0.f )
			{
				matTrans = DxBillboardLookAtDirXZ ( &vStartPos );//m_pAffineParts->vTrans );
				//D3DXMatrixMultiply ( &matWorld, &matTrans, &matWorld  );	// 회전
				D3DXMatrixMultiply_RM2( matTrans, matWorld  );	// 회전
			}

			pData->bChangeMatrix = TRUE;
			pData->matWorld = matWorld;

			m_matWorld = matWorld;
		}

		//	Note : 물체에서 카메라로
		//
		Sub11.x = vFromPt.x - vPlayPos.x;
		Sub11.y = vFromPt.y - vPlayPos.y;
		Sub11.z = vFromPt.z - vPlayPos.z;
		D3DXVec3Normalize ( &Sub11, &Sub11 );
		vSubDis11 = Sub11 * m_fFlarePos;

		//	Note : 텍스쳐 회전 적용/적용안함.
		//
		if ( m_dwFlag & USETEXROTATE )	
		{
			//	Note : 텍스쳐 회전 적용
			m_fTexRotateAngelSum += m_fTexRotateAngel * m_fElapsedTime;

			m_sSequenceVB[0].vPos = D3DXVECTOR3( fSize*sinf( m_fTexRotateAngelSum )*m_fWidthRate, 
												fSize*cosf( m_fTexRotateAngelSum )*m_fHeightRate, 
												0.f );
			m_sSequenceVB[1].vPos = D3DXVECTOR3( fSize*sinf( m_fTexRotateAngelSum+(D3DX_PI/2.f) )*m_fWidthRate, 
												fSize*cosf( m_fTexRotateAngelSum+(D3DX_PI/2.f) )*m_fHeightRate, 
												0.f );
			m_sSequenceVB[2].vPos = D3DXVECTOR3( fSize*sinf( m_fTexRotateAngelSum+(D3DX_PI*1.5f) )*m_fWidthRate, 
												fSize*cosf( m_fTexRotateAngelSum+(D3DX_PI*1.5f) )*m_fHeightRate, 
												0.f );
			m_sSequenceVB[3].vPos = D3DXVECTOR3( fSize*sinf( m_fTexRotateAngelSum+D3DX_PI )*m_fWidthRate, 
												fSize*cosf( m_fTexRotateAngelSum+D3DX_PI )*m_fHeightRate, 
												0.f );
		}
		else
		{
			//	Note : 텍스쳐 회전 적용 안 함 
			m_sSequenceVB[0].vPos = D3DXVECTOR3( -fSize/2*m_fWidthRate, fSize/2*m_fHeightRate, 0.f );
			m_sSequenceVB[1].vPos = D3DXVECTOR3( fSize/2*m_fWidthRate, fSize/2*m_fHeightRate, 0.f );
			m_sSequenceVB[2].vPos = D3DXVECTOR3( -fSize/2*m_fWidthRate, -fSize/2*m_fHeightRate, 0.f );
			m_sSequenceVB[3].vPos = D3DXVECTOR3( fSize/2*m_fWidthRate, -fSize/2*m_fHeightRate, 0.f );			
		}

		m_sSequenceVB[0].vPos += vSubDis11;
		m_sSequenceVB[1].vPos += vSubDis11;
		m_sSequenceVB[2].vPos += vSubDis11;
		m_sSequenceVB[3].vPos += vSubDis11;
	}
	else if ( m_dwFlag&USEBILLBOARDUP )
	{
		matTrans = DxBillboardLookAtHeight ( &m_vPlayPos, DxViewPort::GetInstance().GetMatView() );

		D3DXMatrixMultiply ( &matWorld, &matTrans, &matWorld  );	// 내꺼에 부모꺼

		pData->bChangeMatrix = TRUE;
		pData->matWorld = matWorld;

		m_matWorld = matWorld;

		//	Note : 물체에서 카메라로
		//
		Sub11.x = vFromPt.x - vPlayPos.x;
		Sub11.y = vFromPt.y - vPlayPos.y;
		Sub11.z = vFromPt.z - vPlayPos.z;
		D3DXVec3Normalize ( &Sub11, &Sub11 );
		vSubDis11 = Sub11 * m_fFlarePos;

		//	Note : 텍스쳐 회전 적용 안 함 
		//
		if ( !( m_dwFlag & USETEXROTATE ) )	
		{
			m_sSequenceVB[0].vPos = D3DXVECTOR3( -fSize/2*m_fWidthRate, fSize/2*m_fHeightRate, 0.f );
			m_sSequenceVB[1].vPos = D3DXVECTOR3( fSize/2*m_fWidthRate, fSize/2*m_fHeightRate, 0.f );
			m_sSequenceVB[2].vPos = D3DXVECTOR3( -fSize/2*m_fWidthRate, -fSize/2*m_fHeightRate, 0.f );
			m_sSequenceVB[3].vPos = D3DXVECTOR3( fSize/2*m_fWidthRate, -fSize/2*m_fHeightRate, 0.f );
		}
		else
		{
			//	Note : 텍스쳐 회전 적용
			//
			m_fTexRotateAngelSum += m_fTexRotateAngel * m_fElapsedTime;

			m_sSequenceVB[0].vPos = D3DXVECTOR3( fSize*sinf( m_fTexRotateAngelSum )*m_fWidthRate, 
												fSize*cosf( m_fTexRotateAngelSum )*m_fHeightRate, 
												0.f );
			m_sSequenceVB[1].vPos = D3DXVECTOR3( fSize*sinf( m_fTexRotateAngelSum+(D3DX_PI/2.f) )*m_fWidthRate, 
												fSize*cosf( m_fTexRotateAngelSum+(D3DX_PI/2.f) )*m_fHeightRate, 
												0.f );
			m_sSequenceVB[2].vPos = D3DXVECTOR3( fSize*sinf( m_fTexRotateAngelSum+(D3DX_PI*1.5f) )*m_fWidthRate, 
												fSize*cosf( m_fTexRotateAngelSum+(D3DX_PI*1.5f) )*m_fHeightRate, 
												0.f );
			m_sSequenceVB[3].vPos = D3DXVECTOR3( fSize*sinf( m_fTexRotateAngelSum+D3DX_PI )*m_fWidthRate, 
												fSize*cosf( m_fTexRotateAngelSum+D3DX_PI )*m_fHeightRate, 
												0.f );
		}

		m_sSequenceVB[0].vPos += vSubDis11;
		m_sSequenceVB[1].vPos += vSubDis11;
		m_sSequenceVB[2].vPos += vSubDis11;
		m_sSequenceVB[3].vPos += vSubDis11;
	}

	m_sSequenceVB[0].dwColor = dwColor;
	m_sSequenceVB[1].dwColor = dwColor;
	m_sSequenceVB[2].dwColor = dwColor;
	m_sSequenceVB[3].dwColor = dwColor;

	if( (m_dwFlag&USEDIRECTION) || (m_dwFlag&USEBILLBOARDUP) )
	{
		pData->sCULLMODE.SetValue( D3DCULL_NONE );
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
		break;
	}

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

	// Note : 데이터 복사
	VERTEXCOLORTEX2 *pVertexB;
	hr = DxDynamicVB::m_sVB.pVB->Lock( DxDynamicVB::m_sVB.nOffsetToLock, dwVertexSizeFULL, (VOID**)&pVertexB, dwFlag );
	memcpy( pVertexB, m_sSequenceVB, dwVertexSizeFULL );
	DxDynamicVB::m_sVB.pVB->Unlock();

	// Note : 값을 더해줌.
	DxDynamicVB::m_sVB.nVertexCount += 4;
	DxDynamicVB::m_sVB.nOffsetToLock += dwVertexSizeFULL;
	
	// Note : 나중에 뿌리도록 셋팅 해 놓는다.
	OPTMManager::GetInstance().m_sSingleSequence.InsertData( pData );

_RETURN:	

	//	Note : 형제, 자식 노드.
	//
	D3DXMatrixTranslation ( &matIdentity, vPlayPos.x, vPlayPos.y, vPlayPos.z );
	//D3DXMatrixMultiply ( &matWorld, &matTrans, &matWorld );
	if ( m_pChild )		m_pChild->Render ( pd3dDevice, matIdentity );
	if ( m_pSibling )	m_pSibling->Render ( pd3dDevice, matComb );

	return hr;
}
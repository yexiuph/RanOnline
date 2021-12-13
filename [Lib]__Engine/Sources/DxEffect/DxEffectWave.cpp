#include "pch.h"

#include "./DxViewPort.h"
#include "./TextureManager.h"

#include "./glperiod.h"

#include "./DxEnvironment.h"

#include "./DxEffectMan.h"
#include "./DxEffSingleMan.h"
#include "./SerialFile.h"

#include "./DxEffectWave.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//---------------------------------------------------------------------[WAVE_PROPERTY]
const DWORD	WAVE_PROPERTY::VERSION	= 0x0104;
const char	WAVE_PROPERTY::NAME[]	= "Wave";

DxEffSingle* WAVE_PROPERTY::NEWOBJ ()
{
	DxEffectWave *pEffect = DxEffectWave::m_pPool->New();
	//DxEffectWave *pEffect = new DxEffectWave;

	//	Note : TransPorm Set.		[공통설정]
	pEffect->m_matLocal = m_matLocal;
	
	//	Note : 시간 설정.			[공통설정]
	//
	pEffect->m_fGBeginTime = m_fGBeginTime;
	pEffect->m_fGLifeTime = m_fGLifeTime;

	//	Note : 날아가는 오브젝트	[공통설정]
	//
	pEffect->m_bMoveObj = m_bMoveObj;

	pEffect->m_dwFlag = m_dwFlag;

	pEffect->m_fFlarePos = m_fFlarePos;

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

	//	Note : Bump 초기화
	//
	pEffect->m_fBumpWave	 = m_fBumpWaveStart/2;
	pEffect->m_fBumpWaveStart = m_fBumpWaveStart/2;

	pEffect->m_fBumpWaveTime1 = m_fGBeginTime + m_fGLifeTime*m_fBumpWaveRate1/100.f;
	pEffect->m_fBumpWaveTime2 = m_fGBeginTime + m_fGLifeTime*m_fBumpWaveRate2/100.f;

	if ( m_fBumpWaveRate1 == 0.f )				pEffect->m_fBumpWaveDelta1	= 0.f;
	else										pEffect->m_fBumpWaveDelta1 = ( m_fBumpWaveMid1 - m_fBumpWaveStart ) / (pEffect->m_fBumpWaveTime1-m_fGBeginTime);
	if ( m_fBumpWaveRate2-m_fSizeRate1 == 0.f )	pEffect->m_fBumpWaveDelta2	= 0.f;
	else										pEffect->m_fBumpWaveDelta2 = ( m_fBumpWaveMid2 - m_fBumpWaveMid1 ) / (pEffect->m_fBumpWaveTime2-pEffect->m_fBumpWaveTime1);
	if ( m_fBumpWaveRate2 == 100.f )			pEffect->m_fBumpWaveDelta3	= 0.f;
	else										pEffect->m_fBumpWaveDelta3 = ( m_fBumpWaveEnd - m_fBumpWaveMid2 ) / (m_fGBeginTime+m_fGLifeTime-pEffect->m_fBumpWaveTime2);

	//	Note : 속도 설정
	//
	pEffect->m_fSpeed = m_fSpeed;

	//	Note : 컬러 설정
	//
	pEffect->m_cColorStart = m_cColorStart;
	pEffect->m_cColorVar = m_cColorVar;
	pEffect->m_cColorEnd = m_cColorEnd;

	pEffect->m_emDrawMode = m_emDrawMode;

	//	Note : 사운드 설정
	//
	pEffect->m_MovSound = m_MovSound;

	return pEffect;
}

HRESULT WAVE_PROPERTY::SaveFile ( CSerialFile &SFile )
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

HRESULT WAVE_PROPERTY::LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
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
	}
	else if( dwVer == 0x0103 )
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
	}
	else if( dwVer == 0x0102 )
	{
		//	Note : 부모 클레스의 정보.
		DXAFFINEPARTS sAffineParts;
		SFile.ReadBuffer( m_matLocal, sizeof(D3DXMATRIX) );
		SFile.ReadBuffer( &sAffineParts, sizeof(DXAFFINEPARTS) );
		D3DXMatrixCompX( m_matLocal, sAffineParts );
		
		SFile >> m_bMoveObj;
		SFile >> m_fGBeginTime;
		SFile >> m_fGLifeTime;

		SFile.ReadBuffer ( &m_Property, sizeof(PROPERTY) );

		m_Property.m_fBumpWaveStart	*= 1.3f;	// 값을 수정 함.
		m_Property.m_fBumpWaveMid1	*= 1.3f;
		m_Property.m_fBumpWaveMid2	*= 1.3f;
		m_Property.m_fBumpWaveEnd	*= 1.3f;
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

		SFile.ReadBuffer ( &m_Property, sizeof(PROPERTY) );

		m_Property.m_fBumpWaveStart	*= 1.3f;	// 값을 수정 함.
		m_Property.m_fBumpWaveMid1	*= 1.3f;
		m_Property.m_fBumpWaveMid2	*= 1.3f;
		m_Property.m_fBumpWaveEnd	*= 1.3f;

		m_Property.m_fSizeStart	*= 0.8f;	// 값을 수정 함.
		m_Property.m_fSizeMid1	*= 0.8f;
		m_Property.m_fSizeMid2	*= 0.8f;
		m_Property.m_fAlphaEnd	*= 0.8f;
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

		PROPERTY_100	sProperty;
		SFile.ReadBuffer ( &sProperty, sizeof(PROPERTY_100) );

		m_dwFlag		= sProperty.m_dwFlag;
		m_fFlarePos		= sProperty.m_fFlarePos;

		m_fSizeRate1	= sProperty.m_fSizeRate1;
		m_fSizeRate2	= sProperty.m_fSizeRate2;
		m_fSizeStart	= sProperty.m_fSizeStart;
		m_fSizeMid1		= sProperty.m_fSizeMid1;
		m_fSizeMid2		= sProperty.m_fSizeMid2;
		m_fSizeEnd		= sProperty.m_fSizeEnd;

		m_fAlphaRate1	= sProperty.m_fAlphaRate1;
		m_fAlphaRate2	= sProperty.m_fAlphaRate2;
		m_fAlphaStart	= sProperty.m_fAlphaStart;
		m_fAlphaMid1	= sProperty.m_fAlphaMid1;
		m_fAlphaMid2	= sProperty.m_fAlphaMid2;
		m_fAlphaEnd		= sProperty.m_fAlphaEnd;

		m_fBumpWaveRate1	= sProperty.m_fBumpWaveRate1;
		m_fBumpWaveRate2	= sProperty.m_fBumpWaveRate2;
		m_fBumpWaveStart	= sProperty.m_fBumpWaveStart * 1.3f;
		m_fBumpWaveMid1		= sProperty.m_fBumpWaveMid1 * 1.3f;
		m_fBumpWaveMid2		= sProperty.m_fBumpWaveMid2 * 1.3f;
		m_fBumpWaveEnd		= sProperty.m_fBumpWaveEnd * 1.3f;

		m_cColorStart	= sProperty.m_cColorStart;
		m_cColorVar		= sProperty.m_cColorVar;
		m_cColorEnd		= sProperty.m_cColorEnd;

		m_emDrawMode	= sProperty.m_emDrawMode;

		m_fSpeed		= 7.f;
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

HRESULT WAVE_PROPERTY::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 형제, 자식.
	//
	EFF_PROPERTY::InitDeviceObjects ( pd3dDevice );

	return S_OK;
}

HRESULT WAVE_PROPERTY::DeleteDeviceObjects ()
{
	//	Note : 형제, 자식.
	//
	EFF_PROPERTY::DeleteDeviceObjects ();

	return S_OK;
}

//-------------------------------------------- [정적 맴버 정의 및 초기화] ----------------------------------------------
//
//
const DWORD	DxEffectWave::TYPEID	= EFFSINGLE_WAVE;
const DWORD	DxEffectWave::FLAG		= NULL;
const char	DxEffectWave::NAME[]	= "Wave";

CMemPool<DxEffectWave> *DxEffectWave::m_pPool = NULL;

//	Note : 
const DWORD DxEffectWave::D3DVERTEX::FVF = D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX2;

HRESULT DxEffectWave::CreateDevice( LPDIRECT3DDEVICEQ pd3dDevice )	{ return S_OK; }
HRESULT DxEffectWave::ReleaseDevice( LPDIRECT3DDEVICEQ pd3dDevice )	{ return S_OK; }

void DxEffectWave::OnInitDevice_STATIC()
{
	m_pPool = new CMemPool<DxEffectWave>;
}

void DxEffectWave::OnDeleteDevice_STATIC()
{
	SAFE_DELETE(m_pPool);
}

void DxEffectWave::CheckAABBBox( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin )
{
	D3DXVECTOR3 vPos = m_vLastPlayPos;
	vPos += D3DXVECTOR3( m_fSize, m_fSize, m_fSize );
	if( vMax.x < vPos.x )	vMax.x = vPos.x;
	if( vMax.y < vPos.y )	vMax.y = vPos.y;
	if( vMax.z < vPos.z )	vMax.z = vPos.z;

	vPos = m_vLastPlayPos;
	vPos -= D3DXVECTOR3( m_fSize, m_fSize, m_fSize );
	if( vMin.x > vPos.x )	vMin.x = vPos.x;
	if( vMin.y > vPos.y )	vMin.y = vPos.y;
	if( vMin.z > vPos.z )	vMin.z = vPos.z;

	if( m_pChild )		m_pChild->CheckAABBBox( vMax, vMin );
	if( m_pSibling )	m_pSibling->CheckAABBBox( vMax, vMin );
}

HRESULT DxEffectWave::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	//	Note : 초기 위치 생성 
	//
	float TempXZ = m_fSize/2;
	m_pSequenceVB[0].vPos = D3DXVECTOR3 ( -TempXZ, 0.f, TempXZ );
	m_pSequenceVB[1].vPos = D3DXVECTOR3 ( -TempXZ, 0.f, -TempXZ );
	m_pSequenceVB[2].vPos = D3DXVECTOR3 ( TempXZ, 0.f, TempXZ );
	m_pSequenceVB[3].vPos = D3DXVECTOR3 ( TempXZ, 0.f, -TempXZ );

	//	Note : 컬러값 초기화
	//
	m_cColorDelta.a = 0.f;
	m_cColorDelta.r = ( m_cColorEnd.r - m_cColorStart.r ) / m_fGLifeTime;
	m_cColorDelta.g = ( m_cColorEnd.g - m_cColorStart.g ) / m_fGLifeTime;
	m_cColorDelta.b = ( m_cColorEnd.b - m_cColorStart.b ) / m_fGLifeTime;

	m_pSequenceVB[0].Diffuse = m_pSequenceVB[1].Diffuse = m_pSequenceVB[2].Diffuse = 
	m_pSequenceVB[3].Diffuse = m_cColorStart;

	m_pSequenceVB[0].vTex1 = D3DXVECTOR2 ( 0.f, 0.f );
	m_pSequenceVB[1].vTex1 = D3DXVECTOR2 ( 0.f, 1.f );
	m_pSequenceVB[2].vTex1 = D3DXVECTOR2 ( 1.f, 0.f );
	m_pSequenceVB[3].vTex1 = D3DXVECTOR2 ( 1.f, 1.f );

	m_pSequenceVB[0].vTex2 = D3DXVECTOR2 ( 0.f, 0.f );
	m_pSequenceVB[1].vTex2 = D3DXVECTOR2 ( 0.f, 1.f );
	m_pSequenceVB[2].vTex2 = D3DXVECTOR2 ( 1.f, 0.f );
	m_pSequenceVB[3].vTex2 = D3DXVECTOR2 ( 1.f, 1.f );

	//	Note : 형제, 자식 호출
	//
	DxEffSingle::RestoreDeviceObjects ( pd3dDevice );

	return S_OK;
}

HRESULT DxEffectWave::InvalidateDeviceObjects ()
{
	//	Note : 형제, 자식 호출
	//
	DxEffSingle::InvalidateDeviceObjects ();

	return S_OK;
}

//-------------------------------------------- [클레스 본체] ------------------------------------------------------------
//
//
DxEffectWave::DxEffectWave () :
	m_vGLocation(0,0,0),
	m_vGPrevLocation(0,0,0),

	m_vPlayPos(0.f,0.f,0.f),

	m_fFlarePos(0.0f),

	m_pd3dDevice(NULL)
{
	m_fTime				= 0.f;
	m_fElapsedTime		= 0.f;

	m_fAlpha			= 0.f;
	m_fSize				= 0.f;
	m_fBumpWave			= 0.f;
	m_fSpeed			= 7.f;

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

	m_emDrawMode		= EM_RANDOM;

	m_fGLifeTime		= 0.0f;

	m_fWidthRate = 1.f;
	m_fHeightRate = 1.f;
}

DxEffectWave::~DxEffectWave ()
{
	CleanUp ();
}

void DxEffectWave::ReStartEff ()
{
	m_fGAge		= 0.f;
	m_fSize		= m_fSizeStart;
	m_fAlpha	= m_fAlphaStart;
	m_fBumpWave = m_fBumpWaveStart;
	m_dwRunFlag &= ~EFF_PLY_PLAY;
	m_dwRunFlag &= ~EFF_PLY_DONE;
	m_dwRunFlag &= ~EFF_PLY_END;

	if ( m_pChild )		m_pChild->ReStartEff ();
	if ( m_pSibling )	m_pSibling->ReStartEff ();
}

HRESULT DxEffectWave::FinalCleanup()
{
	//DxEffSingle::StopEffSound ();
	//	Note : 형제 자식 노드.
	//
	DxEffSingle::FinalCleanup ();

	return S_OK;
}

HRESULT DxEffectWave::FrameMove ( float fTime, float fElapsedTime )
{
	D3DXVECTOR3		vDeltaGVel;
	D3DXVECTOR3		vLocal;

	//	Note : Eff의 나이를 계산.
	//
	m_fGAge += fElapsedTime;
	m_fTime += fElapsedTime;
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
			m_fBumpWave = m_fBumpWaveStart;
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

	//	Note : 위치 이동.
	//
	m_vGPrevLocation = m_vGLocation;

	m_vPlayPos = m_vGLocation;	// 회전을 적용하지 않는다면...
	
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

	//	Note : 범프 적용
	//
	if ( m_fGAge >= m_fGBeginTime && m_fGAge < m_fSizeTime1)
		m_fBumpWave += (m_fBumpWaveDelta1*fElapsedTime)/2;
	else if ( m_fGAge >= m_fBumpWaveTime1 && m_fGAge < m_fBumpWaveTime2)	
		m_fBumpWave += (m_fBumpWaveDelta2*fElapsedTime)/2;
	else														
		m_fBumpWave += (m_fBumpWaveDelta3*fElapsedTime)/2;

	if ( m_fBumpWave < 0.f )		m_fBumpWave = 0.f;

	m_pSequenceVB[0].Diffuse = m_pSequenceVB[1].Diffuse = m_pSequenceVB[2].Diffuse = 
	m_pSequenceVB[3].Diffuse = m_cColorVar + m_cColorDelta*(m_fGAge-m_fGBeginTime);

_RETURN:
	//	Note : 형제, 자식 노드.
	//
	DxEffSingle::FrameMove ( fTime, fElapsedTime );

    return S_OK;
}

HRESULT DxEffectWave::Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matComb )
{
	HRESULT hr = S_OK;
	D3DXMATRIX matWorld, matRotate, matRotateL, matTrans, matIdentity;

	float		fLifeTime;
	float		fTime;
	D3DXVECTOR3	vDelta(0.f,0.f,0.f);
	matTrans = m_matLocal;

	float			fLength;
	float			fSize;
	D3DXVECTOR3		Sub11( 0.f, 0.f, 0.f );	
	D3DXVECTOR3		vSubDis11;
	D3DXVECTOR3		vLocal;
	D3DXVECTOR3&	vFromPt		= DxViewPort::GetInstance().GetFromPt();
	D3DXMATRIX		matLocal;
	D3DXVECTOR3		vPlayPos;

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
	D3DXVec3TransformCoord ( &vPlayPos, &m_vPlayPos, &matWorld );
	m_vLastPlayPos = vPlayPos;

	if ( !(m_dwRunFlag&EFF_PLY_PLAY) )		goto _RETURN;
	if ( m_dwRunFlag&EFF_PLY_END )			goto _RETURN;
	if ( m_dwRunFlag&EFF_PLY_RENDPASS )		goto _RETURN;
	if ( m_fSize <= 0.f || m_fAlpha <= 0.f)	goto _RETURN;

	//	Note : 빛에 영향을 받을 경우 ^^;
	//
	if ( m_dwFlag&USELIGHTING && GLPeriod::GetInstance().IsOffLight() )		goto _RETURN;


	//	Note : 물체에서 카메라로
	//
	Sub11.x = vFromPt.x - vPlayPos.x;
	Sub11.y = vFromPt.y - vPlayPos.y;
	Sub11.z = vFromPt.z - vPlayPos.z;

	fLength = D3DXVec3Length ( &Sub11 );			// 거리에 따른
	fLength = fLength/80.f;
	fLength = (fLength<1.f) ? 1.f : fLength;

	float fBumpWave = m_fBumpWave * fLength;

	fSize = m_fSize;

	fLifeTime = m_fGAge / (m_fGBeginTime+m_fGLifeTime);

	if ( m_dwFlag&USEBILLBOARD )
	{
		D3DXVec3Normalize ( &Sub11, &Sub11 );
		vSubDis11 = Sub11 * m_fFlarePos;
		vPlayPos += vSubDis11;

		DxEnvironment::GetInstance().SetWave ( pd3dDevice, TRUE, m_emDrawMode, vPlayPos, m_fSize, m_fAlpha, fBumpWave, fLifeTime, m_fSpeed );
	}
	else if ( m_dwFlag&USEGROUND )
	{
		DxEnvironment::GetInstance().SetWave ( pd3dDevice, FALSE, m_emDrawMode, vPlayPos, m_fSize, m_fAlpha, fBumpWave, fLifeTime, m_fSpeed );
	}

_RETURN:	

	//	Note : 형제, 자식 노드.
	//
	D3DXMatrixTranslation ( &matIdentity, vPlayPos.x, vPlayPos.y, vPlayPos.z );
	if ( m_pChild )		m_pChild->Render ( pd3dDevice, matIdentity );
	if ( m_pSibling )	m_pSibling->Render ( pd3dDevice, matComb );

	return hr;
}
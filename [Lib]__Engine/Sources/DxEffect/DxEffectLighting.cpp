#include "pch.h"

#include "./DxViewPort.h"
#include "./TextureManager.h"
#include "./DxDynamicVB.h"

#include "./DxEffectMan.h"
#include "./DxEffSingleMan.h"
#include "./SerialFile.h"

#include "./DxEffectLighting.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//---------------------------------------------------------------------[LIGHTNING_PROPERTY]
const DWORD	LIGHTNING_PROPERTY::VERSION	= 0x0102;
const char	LIGHTNING_PROPERTY::NAME[]	= "번개";

DxEffSingle* LIGHTNING_PROPERTY::NEWOBJ ()
{
	//DxEffectLightning *pEffect = new DxEffectLightning;
	DxEffectLightning *pEffect = DxEffectLightning::m_pPool->New();

	//	Note : TransPorm Set.		[공통설정]
	pEffect->m_matLocal = m_matLocal;
	
	//	Note : 시간 설정.			[공통설정]
	pEffect->m_fGBeginTime = m_fGBeginTime;
	pEffect->m_fGLifeTime = m_fGLifeTime;

	//	Note : 날아가는 오브젝트	[공통설정]
	pEffect->m_bMoveObj = m_bMoveObj;

	pEffect->m_dwFlag	= m_dwFlag;

	pEffect->m_dwLightning = m_dwLightning;

	pEffect->m_dwOriginNum = m_dwDivision+2;
	pEffect->m_nVertexNum = (m_dwDivision+2)*2;

	pEffect->m_fWidth_In	= m_fWidth_In;
	pEffect->m_fWidth_Out	= m_fWidth_Out;

	pEffect->m_fMaxLenth	= m_fMaxLenth;
	pEffect->m_fVelocity	= m_fVelocity;

	pEffect->m_fAlphaStart	= m_fAlphaStart;
	pEffect->m_cColorStart	= m_cColorStart;

	//	Note : 알파..
	pEffect->m_fAlpha = m_fAlphaStart;

	pEffect->m_fAlphaTime1 = m_fGLifeTime*m_fAlphaRate1/100.f;
	pEffect->m_fAlphaTime2 = m_fGLifeTime*m_fAlphaRate2/100.f;

	if ( m_fAlphaRate1 == 0.f )					pEffect->m_fAlphaDelta1	= 0.f;
	else										pEffect->m_fAlphaDelta1 = ( m_fAlphaMid1 - m_fAlphaStart ) / (pEffect->m_fAlphaTime1);
	if ( m_fAlphaRate2-m_fAlphaRate1 == 0.f )	pEffect->m_fAlphaDelta2	= 0.f;
	else										pEffect->m_fAlphaDelta2 = ( m_fAlphaMid2 - m_fAlphaMid1 ) / (pEffect->m_fAlphaTime2-pEffect->m_fAlphaTime1);
	if ( m_fAlphaRate2 == 100.f || m_fAlphaEnd == m_fAlphaMid2 )	pEffect->m_fAlphaDelta3	= 0.f;
	else															pEffect->m_fAlphaDelta3 = ( m_fAlphaEnd - m_fAlphaMid2 - 0.1f ) / (m_fGLifeTime-pEffect->m_fAlphaTime2);

	pEffect->m_cColor = m_cColorStart;
	pEffect->m_cColorDelta = (m_cColorEnd-m_cColorStart)/m_fGLifeTime;

	pEffect->m_nBlend	= m_nBlend;

	//	Note : 텍스쳐 설정.
	pEffect->m_strTexture_IN = m_szTexture_In;
	pEffect->m_strTexture_OUT = m_szTexture_Out;
	pEffect->m_pTexture_In = m_pTexture_In;
	pEffect->m_pTexture_Out = m_pTexture_Out;

	//	Note : 사운드 설정			[일부]
	pEffect->m_MovSound = m_MovSound;

	return pEffect;
}

HRESULT LIGHTNING_PROPERTY::SaveFile ( CSerialFile &SFile )
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

HRESULT LIGHTNING_PROPERTY::LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
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

		m_pTexture_In = NULL;
		m_pTexture_Out = NULL;
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

		m_pTexture_In = NULL;
		m_pTexture_Out = NULL;
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
		PROPERTY_100	sProp;
		SFile.ReadBuffer ( &sProp, sizeof(PROPERTY_100) );

		m_Property.m_dwFlag			= sProp.m_dwFlag;

		m_Property.m_dwLightning	= sProp.m_dwLightning;

		m_Property.m_dwDivision		= sProp.m_dwDivision;

		m_Property.m_fWidth_In		= sProp.m_fWidth_In;
		m_Property.m_fWidth_Out		= sProp.m_fWidth_Out;

		m_Property.m_fMaxLenth		= (float)sProp.m_dwMaxLenth;
		m_Property.m_fVelocity		= (float)sProp.m_dwVelocity;

		m_Property.m_fAlphaRate1	= sProp.m_fAlphaRate1;
		m_Property.m_fAlphaRate2	= sProp.m_fAlphaRate2;

		m_Property.m_fAlphaStart	= sProp.m_fAlphaStart;
		m_Property.m_fAlphaMid1		= sProp.m_fAlphaMid1;
		m_Property.m_fAlphaMid2		= sProp.m_fAlphaMid2;
		m_Property.m_fAlphaEnd		= sProp.m_fAlphaEnd;

		m_Property.m_cColorStart	= sProp.m_cColorStart;
		m_Property.m_cColorEnd		= sProp.m_cColorEnd;

		m_Property.m_nBlend			= sProp.m_nBlend;

		StringCchCopy( m_Property.m_szTexture_In,	256, sProp.m_szTexture_In );
		StringCchCopy( m_Property.m_szTexture_Out, 256, sProp.m_szTexture_Out );

		m_pTexture_In = NULL;
		m_pTexture_Out = NULL;
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

HRESULT LIGHTNING_PROPERTY::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	TextureManager::LoadTexture ( m_szTexture_In, pd3dDevice, m_pTexture_In, 0, 0, TRUE );

	TextureManager::LoadTexture ( m_szTexture_Out, pd3dDevice, m_pTexture_Out, 0, 0, TRUE );

	//	Note : 형제, 자식.
	//
	EFF_PROPERTY::InitDeviceObjects ( pd3dDevice );

	return S_OK;
}

HRESULT LIGHTNING_PROPERTY::DeleteDeviceObjects ()
{
	TextureManager::ReleaseTexture( m_szTexture_In, m_pTexture_In );
	TextureManager::ReleaseTexture( m_szTexture_Out, m_pTexture_Out );

	//	Note : 형제, 자식.
	//
	EFF_PROPERTY::DeleteDeviceObjects ();

	return S_OK;
}

//-------------------------------------------- [정적 맴버 정의 및 초기화] ----------------------------------------------
//
//
const DWORD	DxEffectLightning::TYPEID	= EFFSINGLE_LIGHTING;
const DWORD	DxEffectLightning::FLAG		= NULL;
const char	DxEffectLightning::NAME[]	= "번개";

CMemPool<DxEffectLightning> *DxEffectLightning::m_pPool = NULL;

LPDIRECT3DSTATEBLOCK9	DxEffectLightning::m_pSavedRenderStats = NULL;
LPDIRECT3DSTATEBLOCK9	DxEffectLightning::m_pDrawRenderStats = NULL;

//	Note : 
//
const DWORD DxEffectLightning::D3DVERTEX::FVF = D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1;

BOOL DxEffectLightning:: VAILEDDEVICE				= FALSE;

HRESULT DxEffectLightning::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;	

	VAILEDDEVICE = TRUE;

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,			0x00000000 );
		pd3dDevice->SetRenderState ( D3DRS_CULLMODE,		D3DCULL_NONE );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,	FALSE );

		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_ALPHAREF,			0x01 );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );

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

HRESULT DxEffectLightning::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	VAILEDDEVICE = FALSE;
	
	SAFE_RELEASE( m_pSavedRenderStats );
	SAFE_RELEASE( m_pDrawRenderStats );

	return S_OK;
}

void DxEffectLightning::OnInitDevice_STATIC()
{
	m_pPool = new CMemPool<DxEffectLightning>;
}

void DxEffectLightning::OnDeleteDevice_STATIC()
{
	SAFE_DELETE(m_pPool);
}

void DxEffectLightning::CheckAABBBox( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin )
{
	D3DXVECTOR3 vPos;
	for( int i=0; i<m_nVertexNum; ++i )
	{
		vPos = m_pVertex[i].vPos;

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

HRESULT DxEffectLightning::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	D3DXVECTOR3 pDelta;

	//	Note : 분할면이 있다면 점을 얻어 와라.
	//
	SAFE_DELETE_ARRAY ( m_pRootVertex );

	m_pRootVertex = new D3DROOT[m_dwOriginNum];

	float Temp = (float)(m_dwOriginNum-2);
	if ( Temp > 0.f )
	{
		D3DXVECTOR3 vPos( m_matLocal._41, m_matLocal._42, m_matLocal._43 );
		pDelta = ( m_pThisGroup->m_vGNowPos - vPos ) / (float)(m_dwOriginNum-1);
		for ( int i=0; i<m_dwOriginNum; i++ )
		{
			m_pRootVertex[i].vPos = vPos + pDelta*(float)i;
		}
	}


	D3DXVECTOR3		vCarDirec;
	D3DXVECTOR3		vDirection;
	D3DXVECTOR3&	vFromPt		= DxViewPort::GetInstance().GetFromPt();
	D3DXVECTOR3&	vLookatPt	= DxViewPort::GetInstance().GetLookatPt();
	D3DXVECTOR3		vParent(0.f,0.f,0.f);

	//	Note : 초기 위치 생성
	//
	SAFE_DELETE_ARRAY ( m_pVertex );
	m_pVertex = new D3DVERTEX[m_nVertexNum];

	SAFE_DELETE_ARRAY ( m_pVertex_Out );
	m_pVertex_Out = new D3DVERTEX[m_nVertexNum];

	float	fWidth_In	= m_fWidth_In*m_pThisGroup->m_fOut_Scale;
	float	fWidth_Out	= m_fWidth_Out*m_pThisGroup->m_fOut_Scale;

	for ( int i=0; i<m_dwOriginNum; i++ )
	{
		vCarDirec = DxViewPort::GetInstance().GetLookDir();

		vDirection = DirectSum( m_pRootVertex, i, m_dwOriginNum-1 );	// 방향을 얻을 수 있다.. !!

		D3DXVec3Cross ( &m_pRootVertex[i].vLookatCross, &vDirection, &vCarDirec );
		D3DXVec3Normalize ( &m_pRootVertex[i].vLookatCross, &m_pRootVertex[i].vLookatCross );		// 계속 값을 받아서 바꾸어줘야 하는 것.!!

		m_pVertex[i*2+0].vPos = m_pRootVertex[i].vPos+(m_pRootVertex[i].vLookatCross*fWidth_In);	//	m_dwAngel
		m_pVertex[i*2+1].vPos = m_pRootVertex[i].vPos-(m_pRootVertex[i].vLookatCross*fWidth_In);

		m_pVertex[i*2+0].Diffuse = m_cColor;
		m_pVertex[i*2+1].Diffuse = m_cColor;

		m_pVertex[i*2+0].vTex = D3DXVECTOR2 ( 0.f, (float)i/(float)(m_dwOriginNum-1) );
		m_pVertex[i*2+1].vTex = D3DXVECTOR2 ( 1.f, (float)i/(float)(m_dwOriginNum-1) );

		m_pVertex_Out[i*2+0].vPos = m_pRootVertex[i].vPos+(m_pRootVertex[i].vLookatCross*fWidth_Out);	//	m_dwAngel
		m_pVertex_Out[i*2+1].vPos = m_pRootVertex[i].vPos-(m_pRootVertex[i].vLookatCross*fWidth_Out);

		m_pVertex_Out[i*2+0].Diffuse = m_cColor;
		m_pVertex_Out[i*2+1].Diffuse = m_cColor;

		m_pVertex_Out[i*2+0].vTex = D3DXVECTOR2 ( 0.f, (float)i/(float)(m_dwOriginNum-1) );
		m_pVertex_Out[i*2+1].vTex = D3DXVECTOR2 ( 1.f, (float)i/(float)(m_dwOriginNum-1) );
	}

	//	Note : 빌보드 및 점들 업데이트..
	//
	UpdatePos ( m_pVertex[0].vPos );

	//	Note : 형제, 자식 호출
	//
	DxEffSingle::RestoreDeviceObjects ( pd3dDevice );

	return S_OK;
}

HRESULT DxEffectLightning::InvalidateDeviceObjects ()
{
	SAFE_DELETE_ARRAY ( m_pVertex );
	SAFE_DELETE_ARRAY ( m_pVertex_Out );
	SAFE_DELETE_ARRAY ( m_pRootVertex );

	//	Note : 형제, 자식 호출
	//
	DxEffSingle::InvalidateDeviceObjects ();

	return S_OK;
}

//-------------------------------------------- [클레스 본체] ------------------------------------------------------------
//
//
DxEffectLightning::DxEffectLightning () :
	m_pTexture_In(NULL),
	m_pTexture_Out(NULL),
	m_pVertex(NULL),
	m_pVertex_Out(NULL),
	m_pRootVertex(NULL),

	m_pd3dDevice(NULL)
{
	m_fAlpha			= 0.f;

	m_nBlend			= 1;

	m_fGLifeTime		= 0.0f;
	m_fElapsedTime		= 0.f;

	m_bFirstPlay		= TRUE;
}

DxEffectLightning::~DxEffectLightning ()
{
	CleanUp ();
}

HRESULT DxEffectLightning::FinalCleanup()
{
	//DxEffSingle::StopEffSound ();
	//	Note : 형제 자식 노드.
	//
	DxEffSingle::FinalCleanup ();

	return S_OK;
}

void DxEffectLightning::ReStartEff ()
{
	m_fGAge		= 0.f;
	m_fAlpha	= m_fAlphaStart;
	m_cColor	= m_cColorStart;
	m_dwRunFlag &= ~EFF_PLY_PLAY;
	m_dwRunFlag &= ~EFF_PLY_DONE;
	m_dwRunFlag &= ~EFF_PLY_END;

	if ( m_pChild )		m_pChild->ReStartEff ();
	if ( m_pSibling )	m_pSibling->ReStartEff ();
}

HRESULT DxEffectLightning::FrameMove ( float fTime, float fElapsedTime )
{
	D3DXVECTOR3		vDeltaGVel;
	D3DXVECTOR3		vLocal;

	//	Note : Eff의 나이를 계산.
	//
	m_fGAge += fElapsedTime;
	m_fElapsedTime += fElapsedTime;

	//	Note : 반복할 때 시간 및 여러 값을 초기화 한다.
	//
	if ( m_pThisGroup->m_dwFlag&EFF_CFG_NEVERDIE )
	{
		if ( m_fGAge>=(m_pThisGroup->m_fGBeginTime+m_pThisGroup->m_fGLifeTime) )
		{
			m_fGAge		= 0.f;
			m_fAlpha	= m_fAlphaStart;
			m_cColor	= m_cColorStart;
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

	//	Note : 컬러 조정 
	//
	if ( m_fGAge >= m_fGBeginTime && m_fGAge < m_fAlphaTime1)
		m_fAlpha += m_fAlphaDelta1*fElapsedTime;
	else if ( m_fGAge >= m_fAlphaTime1 && m_fGAge < m_fAlphaTime2)	
		m_fAlpha += m_fAlphaDelta2*fElapsedTime;
	else		
		m_fAlpha += m_fAlphaDelta3*fElapsedTime;

	m_cColor += m_cColorDelta*fElapsedTime;
	m_cColor.a = m_fAlpha;		// 알파값 적용

_RETURN:
	//	Note : 형제, 자식 노드.
	//
	DxEffSingle::FrameMove ( fTime, fElapsedTime );

    return S_OK;
}

HRESULT DxEffectLightning::Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matComb )
{
	HRESULT			hr = S_OK;
	D3DXVECTOR3		vTrans;
	D3DXMATRIX		matWorld, matRotate, matRotateL, matTrans, matIdentity;

	matTrans = m_matLocal;

	D3DXMatrixMultiply ( &matWorld, &matTrans, &matComb );
	vTrans = D3DXVECTOR3 ( matWorld._41, matWorld._42, matWorld._43 );

	if ( !m_pThisGroup->m_bOut_Particle )	goto _RETURN;
	if ( !(m_dwRunFlag&EFF_PLY_PLAY) )		goto _RETURN;
	if ( m_dwRunFlag&EFF_PLY_END )			goto _RETURN;
	if ( m_dwRunFlag&EFF_PLY_RENDPASS )		goto _RETURN;

	//	Note : 빌보드 및 점들 업데이트..
	//
	//	Note : 뿌려질 위치 설정 
	//
	if ( m_bFirstPlay )		// 처음 뿌릴때 해준다.
	{
		m_fElapsedTime = 0.00f;
		UpdatePos ( vTrans );
		m_bFirstPlay = FALSE;
	}
	if ( m_fElapsedTime > 0.04f )
	{
		m_fElapsedTime = 0.00f;
		UpdatePos ( vTrans );
	}


	D3DXMatrixIdentity ( &matIdentity );
	pd3dDevice->SetTransform ( D3DTS_WORLD, &matIdentity );


	m_pSavedRenderStats->Capture();
	m_pDrawRenderStats->Apply();

	D3DCAPSQ d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );
	DWORD	dwSrcBlend, dwDestBlend, dwColorOP, dwZWriteEnable, dwAlphaBlendEnable, dwAlphaRef, dwAlphaFunc;

	pd3dDevice->GetRenderState ( D3DRS_SRCBLEND,			&dwSrcBlend );
	pd3dDevice->GetRenderState ( D3DRS_DESTBLEND,			&dwDestBlend );
	pd3dDevice->GetRenderState ( D3DRS_ZWRITEENABLE,		&dwZWriteEnable );
	pd3dDevice->GetRenderState ( D3DRS_ALPHABLENDENABLE,	&dwAlphaBlendEnable );
	pd3dDevice->GetRenderState ( D3DRS_ALPHAREF,			&dwAlphaRef );
	pd3dDevice->GetRenderState ( D3DRS_ALPHAFUNC,			&dwAlphaFunc );
	pd3dDevice->GetTextureStageState ( 0, D3DTSS_COLOROP,	&dwColorOP );

	//	Note : 블렌딩 요소 셋팅
	//
	switch ( m_nBlend )
	{
	case 1 :
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );

		break;
	case 2:
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE2X );

		break;
	case 3:
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE4X );

		break;			
	case 4:
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );

		break;
	case 5:
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		TRUE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	FALSE );

		if( d3dCaps.AlphaCmpCaps & D3DPCMPCAPS_GREATEREQUAL )
		{
			pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0x80 );
			pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
		}
		break;
	}


	// Note : Get Texture
	if( !m_pTexture_In )	TextureManager::GetTexture( m_strTexture_IN.c_str(), m_pTexture_In );
	if( !m_pTexture_Out )	TextureManager::GetTexture( m_strTexture_OUT.c_str(), m_pTexture_Out );

	if( m_pTexture_In && m_pTexture_Out )
	{
		//	Note : 텍스쳐 설정.
		pd3dDevice->SetStreamSource ( 0, DxDynamicVB::m_sVB_PDT.pVB, 0, sizeof(D3DVERTEX) );
		pd3dDevice->SetFVF ( D3DVERTEX::FVF );

		if ( m_dwFlag & USEIN )
		{
			// Note : Lock을 하기 위한 D3DLOCK_DISCARD or D3DLOCK_NOOVERWRITE
			DWORD dwFlag = D3DLOCK_NOOVERWRITE; 
			DWORD dwVertexSizeFULL = m_nVertexNum*sizeof(D3DVERTEX);
			if( DxDynamicVB::m_sVB_PDT.nOffsetToLock + dwVertexSizeFULL > DxDynamicVB::m_sVB_PDT.nFullByte )
			{
				dwFlag = D3DLOCK_DISCARD; 
				DxDynamicVB::m_sVB_PDT.nVertexCount = 0;
				DxDynamicVB::m_sVB_PDT.nOffsetToLock = 0; 
			}

			D3DVERTEX *pVertices;
			hr = DxDynamicVB::m_sVB_PDT.pVB->Lock( DxDynamicVB::m_sVB_PDT.nOffsetToLock, dwVertexSizeFULL, (VOID**)&pVertices, dwFlag );
			if ( FAILED(hr) )	goto _RETURN;
			memcpy( pVertices, m_pVertex, dwVertexSizeFULL );
			DxDynamicVB::m_sVB_PDT.pVB->Unlock ();

			pd3dDevice->SetTexture ( 0, m_pTexture_In );
			pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, DxDynamicVB::m_sVB_PDT.nVertexCount, m_nVertexNum-2 );

			// Note : 값을 더해줌. 
			DxDynamicVB::m_sVB_PDT.nVertexCount += m_nVertexNum;
			DxDynamicVB::m_sVB_PDT.nOffsetToLock += dwVertexSizeFULL;
		}
		if ( m_dwFlag & USEOUT )
		{
			for ( DWORD i=0; i<m_dwLightning; i++ )
			{
				// Note : 계산.. 랜덤
				//
				UpdatePos_Out ();

				// Note : Lock을 하기 위한 D3DLOCK_DISCARD or D3DLOCK_NOOVERWRITE
				DWORD dwFlag = D3DLOCK_NOOVERWRITE; 
				DWORD dwVertexSizeFULL = m_nVertexNum*sizeof(D3DVERTEX);
				if( DxDynamicVB::m_sVB_PDT.nOffsetToLock + dwVertexSizeFULL > DxDynamicVB::m_sVB_PDT.nFullByte )
				{
					dwFlag = D3DLOCK_DISCARD; 
					DxDynamicVB::m_sVB_PDT.nVertexCount = 0;
					DxDynamicVB::m_sVB_PDT.nOffsetToLock = 0; 
				}

				D3DVERTEX *pVertices;
				hr = DxDynamicVB::m_sVB_PDT.pVB->Lock( DxDynamicVB::m_sVB_PDT.nOffsetToLock, dwVertexSizeFULL, (VOID**)&pVertices, dwFlag );
				if ( FAILED(hr) )	goto _RETURN;
				memcpy( pVertices, m_pVertex_Out, dwVertexSizeFULL );
				DxDynamicVB::m_sVB_PDT.pVB->Unlock ();

				pd3dDevice->SetTexture ( 0, m_pTexture_Out );
				pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, DxDynamicVB::m_sVB_PDT.nVertexCount, m_nVertexNum-2 );

				// Note : 값을 더해줌. 
				DxDynamicVB::m_sVB_PDT.nVertexCount += m_nVertexNum;
				DxDynamicVB::m_sVB_PDT.nOffsetToLock += dwVertexSizeFULL;
			}
		}
	}


	pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			dwSrcBlend );
	pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			dwDestBlend );
	pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		dwZWriteEnable );
	pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	dwAlphaBlendEnable );
	pd3dDevice->SetRenderState ( D3DRS_ALPHAREF,			dwAlphaRef );
	pd3dDevice->SetRenderState ( D3DRS_ALPHAFUNC,			dwAlphaFunc );
	pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	dwColorOP );

	m_pSavedRenderStats->Apply();

_RETURN:	

	//	Note : 형제, 자식 노드.
	//
	if ( m_pChild )		m_pChild->Render ( pd3dDevice, matIdentity );
	if ( m_pSibling )	m_pSibling->Render ( pd3dDevice, matComb );

	return hr;
}

D3DXVECTOR3		DxEffectLightning::DirectSum ( D3DROOT* pPos, int i, int End )
{
	D3DXVECTOR3 pTempPos1;
	D3DXVECTOR3 pTempPos2;

	if ( i==0 )		pTempPos1 = D3DXVECTOR3 ( 0.f,0.f,0.f );
	else
	{
		pTempPos1 = pPos[i].vPos - pPos[i-1].vPos;
	}

	if ( i>=End )	pTempPos2 = D3DXVECTOR3 ( 0.f,0.f,0.f );
	else
	{
		pTempPos2 = pPos[i+1].vPos - pPos[i].vPos;
	}

	pTempPos1 = pTempPos1 + pTempPos2;
	D3DXVec3Normalize ( &pTempPos1, &pTempPos1 );

	return pTempPos1;
}

void	DxEffectLightning::UpdatePos ( D3DXVECTOR3 vPos )
{
	D3DXVECTOR3		vCarDirec;
	D3DXVECTOR3		vDirection;
	D3DXVECTOR3		vTarget;
	D3DXVECTOR3&	vFromPt		= DxViewPort::GetInstance().GetFromPt();
	D3DXVECTOR3&	vLookatPt	= DxViewPort::GetInstance().GetLookatPt();

	if ( m_pThisGroup->m_dwFlag & EFF_CFG_TARGETID )
	{
		vTarget = m_pThisGroup->m_vGNowPos;
	}
	else if ( m_pThisGroup->m_dwFlag & EFF_CFG_TARGETLINE )
	{
		vTarget = m_pThisGroup->m_vGNowPos;
	}
	else if ( m_pThisGroup->m_bAutoMove )
	{
		vPos	= m_pThisGroup->m_vStartPos;
		vTarget = m_pThisGroup->m_vTargetPos;
	}
	else
	{
		vTarget = D3DXVECTOR3 ( m_pThisGroup->m_matWorld._41, m_pThisGroup->m_matWorld._42, m_pThisGroup->m_matWorld._43 );
	}
	
	//	Note : 뿌려질 위치 설정 
	//
	m_pRootVertex[0].vPos					= vPos;			//m_pThisGroup->m_pAffineParts->vTrans + m_pAffineParts->vTrans;
	m_pRootVertex[m_dwOriginNum-1].vPos		= vTarget;		//m_pThisGroup->m_vGNowPos;
	m_pRootVertex[m_dwOriginNum-1].vPos.y	+= m_pThisGroup->m_vLocal.y;

	float	fVelocity	= (m_fVelocity*m_pThisGroup->m_fOut_Scale);

	//	Note : 회전 
	//
	if ( m_dwFlag & USERANROTATE )
	{
		D3DXVECTOR3		vVel(0.f,0.f,0.f);

		vVel = D3DXVECTOR3 ( (RANDOM_POS-0.5f)*fVelocity,(RANDOM_POS-0.5f)*fVelocity, (RANDOM_POS-0.5f)*fVelocity );
		
		for ( int i=1; i<m_dwOriginNum-1; i++ )
		{
			m_pRootVertex[i].vPos = m_pRootVertex[i-1].vPos + (vVel / (float)(i));
			m_pRootVertex[i].vPos += ((m_pRootVertex[m_dwOriginNum-1].vPos - m_pRootVertex[i].vPos) / (float)(m_dwOriginNum-i));
		}
	}
	else
	{
		float Temp = (float)(m_dwOriginNum-2);
		if ( Temp > 0.f )
		{
			D3DXVECTOR3 pDelta = (vTarget-vPos) / (float)(m_dwOriginNum-1);
			for ( int i=1; i<m_dwOriginNum-1; i++ )
			{
				m_pRootVertex[i].vPos = vPos + pDelta*(float)i;
			}
		}
	}

	float	fWidth_In	= m_fWidth_In*m_pThisGroup->m_fOut_Scale;

	for ( int i=0; i<m_dwOriginNum; i++ )
	{
		vCarDirec = DxViewPort::GetInstance().GetLookDir();

		vDirection = DirectSum( m_pRootVertex, i, m_dwOriginNum-1 );	// 방향을 얻을 수 있다.. !!

		D3DXVec3Cross ( &m_pRootVertex[i].vLookatCross, &vDirection, &vCarDirec );
		D3DXVec3Normalize ( &m_pRootVertex[i].vLookatCross, &m_pRootVertex[i].vLookatCross );		// 계속 값을 받아서 바꾸어줘야 하는 것.!!

		m_pVertex[i*2+0].vPos = m_pRootVertex[i].vPos+(m_pRootVertex[i].vLookatCross*fWidth_In);	//	m_dwAngel
		m_pVertex[i*2+1].vPos = m_pRootVertex[i].vPos-(m_pRootVertex[i].vLookatCross*fWidth_In);

		m_pVertex[i*2+0].Diffuse = m_cColor;
		m_pVertex[i*2+1].Diffuse = m_cColor;

		m_pVertex_Out[i*2+0].Diffuse = m_cColor;
		m_pVertex_Out[i*2+1].Diffuse = m_cColor;
	}
}

void	DxEffectLightning::UpdatePos_Out ()
{
	D3DXVECTOR3		vDirection;

	float	fMaxLenth	= (m_fMaxLenth*m_pThisGroup->m_fOut_Scale);
	float	fWidth_Out	= m_fWidth_Out*m_pThisGroup->m_fOut_Scale;

	for ( int i=0; i<m_dwOriginNum; i++ )
	{
		if ( (m_dwFlag&USESTART) && i==0 )						
		{ 
			m_pVertex_Out[i*2+0].vPos = m_pRootVertex[i].vPos+(m_pRootVertex[i].vLookatCross*fWidth_Out);
			m_pVertex_Out[i*2+1].vPos = m_pRootVertex[i].vPos-(m_pRootVertex[i].vLookatCross*fWidth_Out);
		}
		else if ( (m_dwFlag&USEEND) && i >= m_dwOriginNum-1 )
		{
			m_pVertex_Out[i*2+0].vPos = m_pRootVertex[i].vPos+(m_pRootVertex[i].vLookatCross*fWidth_Out);
			m_pVertex_Out[i*2+1].vPos = m_pRootVertex[i].vPos-(m_pRootVertex[i].vLookatCross*fWidth_Out);
		}
		else
		{
			vDirection = D3DXVECTOR3 ( (RANDOM_POS-0.5f)*fMaxLenth, (RANDOM_POS-0.5f)*fMaxLenth, (RANDOM_POS-0.5f)*fMaxLenth );
			m_pVertex_Out[i*2+0].vPos = m_pRootVertex[i].vPos+(m_pRootVertex[i].vLookatCross*fWidth_Out) + vDirection;
			m_pVertex_Out[i*2+1].vPos = m_pRootVertex[i].vPos-(m_pRootVertex[i].vLookatCross*fWidth_Out) + vDirection;
		}
	}

}
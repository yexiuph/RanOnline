#include "pch.h"

#include "./DxViewPort.h"
#include "./TextureManager.h"
#include "./ShaderConstant.h"
#include "./DxDecal.h"
#include "./DxDynamicVB.h"

#include "./DxEffectMan.h"
#include "./DxEffSingleMan.h"
#include "./SerialFile.h"

#include "./DxEffectDecal.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//	Note	:	디버그용,
extern	BOOL	g_bDebugParticleRange;

//---------------------------------------------------------------------[PARTICLESYS_PROPERTY]
const DWORD	DECAL_PROPERTY::VERSION	= 0x0101;
const char	DECAL_PROPERTY::NAME[]	= "지면효과-데칼";

DxEffSingle* DECAL_PROPERTY::NEWOBJ ()
{
	//DxEffectDecal *pEffect = new DxEffectDecal;
	DxEffectDecal *pEffect = DxEffectDecal::m_pPool->New();

	//	Note : TransPorm Set.		[공통설정]
	pEffect->m_matLocal = m_matLocal;
	//pEffect->m_AffineParts = *m_pAffineParts;
	//D3DXMatrixCompX( pEffect->m_matLocal, pEffect->m_AffineParts );
	////pEffect->SetAffineValue ( m_pAffineParts );

	//	Note : 시간 설정.			[공통설정]
	pEffect->m_fGBeginTime = m_fGBeginTime;
	pEffect->m_fGLifeTime = m_fGLifeTime;

	//	Note : 날아가는 오브젝트	[공통설정]
	pEffect->m_bMoveObj = m_bMoveObj;

	pEffect->m_dwFlag = m_dwFlag;

	pEffect->m_vArea = m_vArea;

	pEffect->m_fAniTime = m_fAniTime;
	pEffect->m_nCol = m_iCol;
	pEffect->m_nRow = m_iRow;

	pEffect->m_fRotateAngel = m_fRotateAngel;

	//	Note : 범위(사이즈) 초기화
	pEffect->m_fSize = m_fSizeStart;
	pEffect->m_fSizeStart = m_fSizeStart;

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
	pEffect->m_fAlphaStart = m_fSizeStart;

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

	return pEffect;
}

HRESULT DECAL_PROPERTY::SaveFile ( CSerialFile &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	//	Note : DATA의 사이즈를 기록한다. Load 시에 버전이 틀릴 경우 사용됨.
	//
	SFile << (DWORD) ( GetSizeBase() + sizeof(m_Property) );

	//	Note : 부모 클레스의 정보.
	SFile.WriteBuffer( m_matLocal, sizeof(D3DXMATRIX) );
	
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

HRESULT DECAL_PROPERTY::LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
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
		
		SFile >> m_fGBeginTime;
		SFile >> m_fGLifeTime;

		//	Note : 이팩트의 Property 를 저장.
		//
		SFile.ReadBuffer ( &m_Property, sizeof(PROPERTY) );

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

HRESULT DECAL_PROPERTY::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	TextureManager::LoadTexture ( m_szTexture, pd3dDevice, m_pTexture, 0, 0, TRUE );

	//	Note : 형제, 자식.
	//
	EFF_PROPERTY::InitDeviceObjects ( pd3dDevice );

	return S_OK;
}

HRESULT DECAL_PROPERTY::DeleteDeviceObjects ()
{
	TextureManager::ReleaseTexture( m_szTexture, m_pTexture );

	//	Note : 형제, 자식.
	//
	EFF_PROPERTY::DeleteDeviceObjects ();

	return S_OK;
}

const DWORD	DxEffectDecal::TYPEID		= EFFSINGLE_DECAL;
const DWORD	DxEffectDecal::FLAG			= NULL;
const char	DxEffectDecal::NAME[]		= "지면효과-데칼";

//	Note :
//
const DWORD DxEffectDecal::D3DDECALVERTEX::FVF = D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1;
const DWORD DxEffectDecal::MAX_VERTICES = 512;

CMemPool<DxEffectDecal> *DxEffectDecal::m_pPool = NULL;

LPDIRECT3DSTATEBLOCK9		DxEffectDecal::m_pSavedRenderStats = NULL;
LPDIRECT3DSTATEBLOCK9		DxEffectDecal::m_pDrawRenderStats = NULL;

BOOL DxEffectDecal:: VAILEDDEVICE				= FALSE;

DxEffectDecal::DxEffectDecal(void) :
	m_vGLocation(0,0,0),
	m_nBlend(1),
	m_fTimeSum(0.f),
	m_iAllSprite(1),
	m_iNowSprite(0),

	m_fSizeStart(0.f),
	m_fAlphaStart(0.f),

	m_fRotateAngel(0.f),
	m_fRotateAngelSum(0.f),

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

	m_pd3dDevice(NULL),

	m_pDecalIB(NULL),
	m_dwVerts(0),
	m_dwFaces(0),

	m_dwRepeat(0),

	m_vPrevPos(0.f,0.f,0.f)
{
	m_fAniTime = 0.f;
	m_nCol = 1;
	m_nRow = 1;

	m_fSize		= 1.f;

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
}

DxEffectDecal::~DxEffectDecal(void)
{

}

HRESULT DxEffectDecal::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;

	VAILEDDEVICE = TRUE;

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		float fBias = -0.0004f;
		pd3dDevice->SetRenderState( D3DRS_DEPTHBIAS,		*((DWORD*)&fBias) );
		pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,			0x00000000 );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,	FALSE );

		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_ALPHAREF,			0x01 );

		pd3dDevice->SetSamplerState ( 0, D3DSAMP_BORDERCOLOR, 0x00000000 );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSU,	D3DTADDRESS_CLAMP );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSV,	D3DTADDRESS_CLAMP );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );

		if( which==0 )	pd3dDevice->EndStateBlock ( &m_pSavedRenderStats );
		else			pd3dDevice->EndStateBlock ( &m_pDrawRenderStats );
	}

	return S_OK;
}

HRESULT DxEffectDecal::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	VAILEDDEVICE = FALSE;

	SAFE_RELEASE( m_pSavedRenderStats );
	SAFE_RELEASE( m_pDrawRenderStats );

	return S_OK;
}

void DxEffectDecal::OnInitDevice_STATIC()
{
	m_pPool = new CMemPool<DxEffectDecal>;
}

void DxEffectDecal::OnDeleteDevice_STATIC()
{
	SAFE_DELETE(m_pPool);
}

void DxEffectDecal::CheckAABBBox( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin )
{
	for( DWORD i=0; i<m_dwVerts; ++i )
	{
		if( vMax.x < m_VertexArray[i].vPos.x )	vMax.x = m_VertexArray[i].vPos.x;
		if( vMax.y < m_VertexArray[i].vPos.y )	vMax.y = m_VertexArray[i].vPos.y;
		if( vMax.z < m_VertexArray[i].vPos.z )	vMax.z = m_VertexArray[i].vPos.z;

		if( vMin.x > m_VertexArray[i].vPos.x )	vMin.x = m_VertexArray[i].vPos.x;
		if( vMin.y > m_VertexArray[i].vPos.y )	vMin.y = m_VertexArray[i].vPos.y;
		if( vMin.z > m_VertexArray[i].vPos.z )	vMin.z = m_VertexArray[i].vPos.z;
	}

	if( m_pChild )		m_pChild->CheckAABBBox( vMax, vMin );
	if( m_pSibling )	m_pSibling->CheckAABBBox( vMax, vMin );
}

HRESULT DxEffectDecal::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	//	Note : 컬러값 초기화
	//
	m_cColorDelta.a = 0.f;
	m_cColorDelta.r = ( m_cColorEnd.r - m_cColorStart.r ) / m_fGLifeTime;
	m_cColorDelta.g = ( m_cColorEnd.g - m_cColorStart.g ) / m_fGLifeTime;
	m_cColorDelta.b = ( m_cColorEnd.b - m_cColorStart.b ) / m_fGLifeTime;

	SAFE_RELEASE ( m_pDecalIB );
	pd3dDevice->CreateIndexBuffer ( 3 * MAX_VERTICES * sizeof(WORD), 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pDecalIB, NULL );

	//	Note : 형제, 자식 호출
	//
	DxEffSingle::RestoreDeviceObjects ( pd3dDevice );

	return S_OK;
}

HRESULT DxEffectDecal::InvalidateDeviceObjects ()
{
	SAFE_RELEASE ( m_pDecalIB );

	//	Note : 형제, 자식 호출
	//
	DxEffSingle::InvalidateDeviceObjects ();

	return S_OK;
}

void DxEffectDecal::ReStartEff ()
{
	m_fGAge		= 0.f;
	m_fSize		= m_fSizeStart;
	m_fAlpha	= m_fAlphaStart;
	m_dwRunFlag &= ~EFF_PLY_PLAY;
	m_dwRunFlag &= ~EFF_PLY_DONE;
	m_dwRunFlag &= ~EFF_PLY_END;

	if ( m_pChild )		m_pChild->ReStartEff ();
	if ( m_pSibling )	m_pSibling->ReStartEff ();
}

HRESULT DxEffectDecal::FrameMove ( float fTime, float fElapsedTime )
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

	//	Note : 텍스쳐 사이즈 적용
	//
	if ( m_dwFlag & USETEXSCALE )
	{
		if ( m_fGAge >= m_fGBeginTime && m_fGAge < m_fSizeTime1)
			m_fSize += (m_fSizeDelta1*fElapsedTime)/2;
		else if ( m_fGAge >= m_fSizeTime1 && m_fGAge < m_fSizeTime2)
			m_fSize += (m_fSizeDelta2*fElapsedTime)/2;
		else
			m_fSize += (m_fSizeDelta3*fElapsedTime)/2;
	}

	if ( m_fSize > 1.f )		m_fSize = 1.f;
	if ( m_fSize < 0.f )		m_fSize = 0.001f;

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

_RETURN:
	//	Note : 형제, 자식 노드.
	//
	DxEffSingle::FrameMove ( fTime, fElapsedTime );

	return S_OK;
}

HRESULT	DxEffectDecal::Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matComb )
{
	HRESULT hr = S_OK;
	D3DXVECTOR3		v;
	D3DXMATRIX		matView, matProj, matWVP;
	D3DXMATRIX		matIdentity, matWorld, matRotate, matRotateL, matTrans, matTempComb;
	D3DXMatrixIdentity ( &matIdentity );
	matTrans = m_matLocal;

	pd3dDevice->SetTransform ( D3DTS_WORLD, &matIdentity );

	D3DXMatrixMultiply ( &matWorld, &matTrans, &matComb );				// 마지막 World 매트릭스 구하기


	D3DXVECTOR3	vCenter(matWorld._41,matWorld._42,matWorld._43);		// 뿌려지는 중심 위치 구하기

	DxLandMan*	pLandMan = DxEffSingleMan::GetInstance().GetLandMan();
	if ( pLandMan )
	{
		BOOL		bCollision;
		D3DXVECTOR3	vCollision;
		D3DXVECTOR3	vPoint1(matWorld._41,matWorld._42+1.f,matWorld._43);		// 뿌려지는 중심 위치 구하기
		D3DXVECTOR3	vPoint2(matWorld._41,matWorld._42-100.f,matWorld._43);		// 뿌려지는 중심 위치 구하기
		LPDXFRAME	pDxFrame = NULL;

		pLandMan->IsCollision( vPoint1, vPoint2, vCollision, bCollision, pDxFrame, TRUE );

		vCenter = vCollision;
	}
	
	float fHeight = matWorld._42 - vCenter.y;
	fHeight = fabsf ( fHeight );

	float fWidth;
	if ( fHeight >= m_vArea.x )	fWidth = 0.f;
	else						fWidth = 1.f - (fHeight/m_vArea.x);

	fWidth = m_fSize*fWidth;

	D3DXVECTOR3	vAreaXYZ;
	vAreaXYZ.x = m_vArea.x*fWidth;
	vAreaXYZ.y = m_vArea.y*fWidth;
	vAreaXYZ.z = m_vArea.z*fWidth;

	D3DXMatrixIdentity ( &matWorld );									// 제 정상 

	D3DXVECTOR3	vNormal(0.f,1.f,0.f);
	D3DXVECTOR3	vTangent(0.f,0.f,1.f);

	//	Note : 회전 시키자 
	//
	if ( m_dwFlag & USEROTATE )
	{
		D3DXMatrixRotationAxis ( &matRotate, &vNormal, m_fRotateAngelSum );
		//D3DXMatrixMultiply ( &matWorld, &matRotate, &matWorld );
		D3DXMatrixMultiply_RM2( matRotate, matWorld );
	}
	
	D3DXVec3TransformNormal ( &vNormal, &vNormal, &matWorld );		// 회전시 방향
	D3DXVec3TransformNormal ( &vTangent, &vTangent, &matWorld );	// 회전시 방향

	D3DXVECTOR3 vBiNormal;
	D3DXVec3Cross ( &vBiNormal, &vNormal, &vTangent );

	if ( m_dwFaces == 0 || m_vPrevPos!=vCenter )
	{
		DxOctree *pOctree = NULL;
		if ( DxEffSingleMan::GetInstance().GetLandMan() )
		{
			pOctree = DxEffSingleMan::GetInstance().GetLandMan()->GetOctree();
			DxDecal::GetInstance().CreateDecal ( vCenter, vNormal, vTangent, vAreaXYZ, pOctree );

		//	DxDecal::GetInstance().CloneVertexBuffer ( pd3dDevice, m_pDecalVB );
		//	DxDecal::GetInstance().CloneIndexBuffer ( pd3dDevice, m_pDecalIB );

			DxDecal::GetInstance().CloneVertexArray ( m_VertexArray );
			DxDecal::GetInstance().CloneIndexBuffer ( pd3dDevice, m_pDecalIB );

			m_dwVerts = DxDecal::GetInstance().GetVertexCount();
			m_dwFaces = DxDecal::GetInstance().GetFaceCount();
		}
		m_vPrevPos = vCenter;				// 현재 위치를 넣어 준다.
	}

	if ( m_dwFaces == 0 )					goto _RETURN;

	if ( !(m_dwRunFlag&EFF_PLY_PLAY) )		goto _RETURN;
	if ( m_dwRunFlag&EFF_PLY_END )			goto _RETURN;
	if ( m_dwRunFlag&EFF_PLY_RENDPASS )		goto _RETURN;
	if ( m_fSize <= 0.f || m_fAlpha <= 0.f)	goto _RETURN;


	float fOneOverX = 1.0f / vAreaXYZ.x;
	float fOneOverZ = 1.0f / vAreaXYZ.z;

	//	Note : 색 변화
	//			초기 값 ( vTangent, vBiNormal 이 값에 의해 회전이 가능하다. )
	//
	for ( DWORD i=0; i<m_dwVerts; i++ )
	{
		v.x = m_VertexArray[i].vPos.x - vCenter.x;
		v.y = m_VertexArray[i].vPos.y - vCenter.y;//0.f;
		v.z = m_VertexArray[i].vPos.z - vCenter.z;

		// Diffuse
		m_cColorVar.a = m_fAlpha;
		m_VertexArray[i].Diffuse = m_cColorVar + m_cColorDelta*(m_fGAge-m_fGBeginTime);

		// UV
		v /= m_fSize;
		float s = D3DXVec3Dot ( &v, &vTangent ) * fOneOverX + 0.5f;
		float t = D3DXVec3Dot ( &v, &vBiNormal ) * fOneOverZ + 0.5f;
		m_VertexArray[i].vTex.x = s;
		m_VertexArray[i].vTex.y = t;
	}

	//	Note : 스프라이트
	//
	if ( m_dwFlag & USESEQUENCE )
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

		for ( DWORD i=0; i<m_dwVerts; i++ )
		{
			m_VertexArray[i].vTex.x = (m_VertexArray[i].vTex.x/(float)m_nCol) + (m_iNowSprite/(float)m_nCol) - (m_iNowSprite/m_nCol);
			m_VertexArray[i].vTex.y = (m_VertexArray[i].vTex.y/(float)m_nRow) + ((m_iNowSprite/m_nCol)/(float)m_nRow);
		}
	}

	// Note : Lock을 하기 위한 D3DLOCK_DISCARD or D3DLOCK_NOOVERWRITE
	DWORD dwVertexSizeFULL = m_dwVerts*sizeof(DECALVERTEX);
	DWORD dwFlag = D3DLOCK_NOOVERWRITE; 
	if( DxDynamicVB::m_sVB_PDT.nOffsetToLock + dwVertexSizeFULL > DxDynamicVB::m_sVB_PDT.nFullByte )
	{
		dwFlag = D3DLOCK_DISCARD; 
		DxDynamicVB::m_sVB_PDT.nVertexCount = 0;
		DxDynamicVB::m_sVB_PDT.nOffsetToLock = 0; 
	}

	// Note : Lock 함.
	DECALVERTEX* pVertices;
	hr = DxDynamicVB::m_sVB_PDT.pVB->Lock( DxDynamicVB::m_sVB_PDT.nOffsetToLock, dwVertexSizeFULL, (VOID**)&pVertices, dwFlag );
	if ( FAILED(hr) )	return S_OK;

	memcpy( pVertices, m_VertexArray, dwVertexSizeFULL );

	DxDynamicVB::m_sVB_PDT.pVB->Unlock();


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
	if( !m_pTexture )	TextureManager::GetTexture( m_strTexture.c_str(), m_pTexture );

	//	Note : 텍스쳐 설정.
	//
	pd3dDevice->SetTexture ( 0, m_pTexture );

	pd3dDevice->SetIndices( m_pDecalIB );
	pd3dDevice->SetStreamSource ( 0, DxDynamicVB::m_sVB_PDT.pVB, 0, sizeof (D3DDECALVERTEX) );
	pd3dDevice->SetFVF ( D3DDECALVERTEX::FVF );

	if( m_pTexture )
	{
		pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, DxDynamicVB::m_sVB_PDT.nVertexCount, 0, m_dwVerts, 0, m_dwFaces );
	}

	pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			dwSrcBlend );
	pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			dwDestBlend );
	pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		dwZWriteEnable );
	pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	dwAlphaBlendEnable );
	pd3dDevice->SetRenderState ( D3DRS_ALPHAREF,			dwAlphaRef );
	pd3dDevice->SetRenderState ( D3DRS_ALPHAFUNC,			dwAlphaFunc );
	pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	dwColorOP );

	if ( g_bDebugParticleRange )
	{
		DWORD	dwFillMode;
		pd3dDevice->GetRenderState ( D3DRS_FILLMODE,			&dwFillMode );
		pd3dDevice->SetRenderState ( D3DRS_FILLMODE,			D3DFILL_WIREFRAME );
		pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, DxDynamicVB::m_sVB_PDT.nVertexCount, 0, m_dwVerts, 0, m_dwFaces );
		pd3dDevice->SetRenderState ( D3DRS_FILLMODE,			dwFillMode );
	}

	// Note : 값을 더해줌. 
	DxDynamicVB::m_sVB_PDT.nVertexCount += m_dwVerts;
	DxDynamicVB::m_sVB_PDT.nOffsetToLock += dwVertexSizeFULL;

	m_pSavedRenderStats->Apply();

_RETURN:
	if ( FAILED(hr) )	return hr;

	//	Note : 형제, 자식 노드.
	//
	if ( m_pChild )		m_pChild->Render ( pd3dDevice, matWorld );
	if ( m_pSibling )	m_pSibling->Render ( pd3dDevice, matComb );

	return S_OK;
}

HRESULT DxEffectDecal::FinalCleanup()
{
	//	Note : 형제 자식 노드.
	//
	DxEffSingle::FinalCleanup ();

	return S_OK;
}

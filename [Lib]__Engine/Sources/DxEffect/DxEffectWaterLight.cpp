// DxEffectWaterLight.cpp: implementation of the DxEffectShadowHW class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "./DxFrameMesh.h"
#include "./DxEffectMan.h"
#include "./SerialFile.h"

#include "./dxeffectwaterlight.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const DWORD DxEffectWaterLight::VERTEX::FVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1;
const DWORD DxEffectWaterLight::WATERVERTEX::FVF = D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX2;
const DWORD DxEffectWaterLight::TYPEID = DEF_EFFECT_WATERLIGHT;
const DWORD	DxEffectWaterLight::VERSION = 0x00000101;
const char DxEffectWaterLight::NAME[] = "[ 물 효과 (벽에 물빛) ]";
const DWORD DxEffectWaterLight::FLAG = _EFF_REPLACE;

void DxEffectWaterLight::GetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD &dwVer )
{
	pProp = (PBYTE) &m_Property;
	dwSize = sizeof(WATERLIGHT_PROPERTY);
	dwVer = VERSION;
}

void DxEffectWaterLight::SetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD dwVer )
{
	if ( dwVer==0x100 && dwSize==sizeof(WATERLIGHT_PROPERTY_100) )
	{
		WATERLIGHT_PROPERTY_100 Property100 = *((WATERLIGHT_PROPERTY_100*)pProp);

		m_Property.m_bBaseRender	= Property100.m_bBaseRender;
		m_Property.m_fScale			= Property100.m_fScale;
		m_Property.m_fVel			= Property100.m_fVel;
		m_Property.m_iAlphaDown		= Property100.m_iAlphaDown;
		m_Property.m_iAlphaMiddle	= Property100.m_iAlphaMiddle;
		m_Property.m_iAlphaUp		= Property100.m_iAlphaUp;
		m_Property.m_vColor			= Property100.m_vColor;
		m_Property.m_vDir			= Property100.m_vDir;

		StringCchCopy( m_Property.m_szTexMove,		MAX_PATH, Property100.m_szTexMove );
		StringCchCopy( m_Property.m_szTexRotate,	MAX_PATH, Property100.m_szTexRotate );

		m_Property.m_fMaxX			= 0.f;
		m_Property.m_fMinX			= 0.f;
		m_Property.m_fMaxY			= 0.f;
		m_Property.m_fMinY			= 0.f;
		m_Property.m_fMaxZ			= 0.f;
		m_Property.m_fMinZ			= 0.f;
	}	

	if ( dwVer==VERSION && dwSize==sizeof(WATERLIGHT_PROPERTY) )
	{
		memcpy ( &m_Property, pProp, dwSize );
	}
}

LPDIRECT3DSTATEBLOCK9	DxEffectWaterLight::m_pSavedStateBlock = NULL;
LPDIRECT3DSTATEBLOCK9	DxEffectWaterLight::m_pEffectStateBlock = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DxEffectWaterLight::DxEffectWaterLight() : 
	m_bBaseRender(false),
	m_fVel(1.f),
	m_fScale(0.7f),
	m_vDir(D3DXVECTOR3(1.f,0.f,1.f)),
	m_vColor(255,255,255),	// 컬러
	m_iAlphaUp(0),				// 알파 값	UP
	m_iAlphaMiddle(100),		// 알파 값	Middle
	m_iAlphaDown(0),			// 알파 값	DOWN

	m_fMaxX(0.f),
	m_fMinX(0.f),
	m_fMaxY(0.f),
	m_fMinY(0.f),
	m_fMaxZ(0.f),
	m_fMinZ(0.f),

	m_pVB(NULL),
	m_pIB(NULL),
	m_dwVertices(0),
	m_dwFaces(0),
	m_pDiffVertex(NULL),	// 저장	-	디퓨즈..~?
	m_pDiffNum(0),			// 저장	-	디퓨즈..~?
	m_pddsTexMove(NULL),
	m_pddsTexRotate(NULL),
	m_fElapsedTime(0.f),
	m_fTime(0.f),
	m_pTexUV(NULL),
	m_vAddTex1(D3DXVECTOR2(0.f,0.f)),
	m_vAddTex2(D3DXVECTOR2(0.f,0.f)),
	m_fHighHeight(0.0f),
	m_fLowerHeight(0.0f)
{
	StringCchCopy( m_szTexMove,		MAX_PATH, "_Wa_water1.bmp" );
	StringCchCopy( m_szTexRotate,	MAX_PATH, "_Wa_water1.bmp" );
}

DxEffectWaterLight::~DxEffectWaterLight()
{
	SAFE_DELETE_ARRAY(m_szAdaptFrame);

	SAFE_DELETE_ARRAY ( m_pTexUV );
	SAFE_DELETE_ARRAY ( m_pDiffVertex );
	SAFE_RELEASE ( m_pVB );
	SAFE_RELEASE ( m_pIB );
}


HRESULT DxEffectWaterLight::AdaptToDxFrameChild ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	DxMeshes *pmsMeshs;

	if ( pframeCur->pmsMeshs != NULL )
	{
		pmsMeshs = pframeCur->pmsMeshs;

		while ( pmsMeshs != NULL )
		{
			if ( pmsMeshs->m_pLocalMesh || pmsMeshs->m_pDxOctreeMesh )
			{
				CloneWaterLight( pd3dDevice, pmsMeshs );
			}
			if ( pmsMeshs->m_pLocalMesh )
			{
				SetBoundBox ( pmsMeshs->m_pLocalMesh );
			}

			pmsMeshs = pmsMeshs->pMeshNext;
		}
	}

	return S_OK;
}

HRESULT DxEffectWaterLight::AdaptToDxFrame ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !pFrame ) return S_OK;

	this->pLocalFrameEffNext = pFrame->pEffect;
	pFrame->pEffect = this;

	if ( pFrame->szName )
	{
		SAFE_DELETE_ARRAY(m_szAdaptFrame);
		size_t nStrLen = strlen(pFrame->szName)+1;
		m_szAdaptFrame = new char[nStrLen];

		StringCchCopy( m_szAdaptFrame, nStrLen, pFrame->szName );
	}
	
	return AdaptToDxFrameChild ( pFrame, pd3dDevice );												//	H/W
}

HRESULT DxEffectWaterLight::AdaptToEffList ( const DxFrame *const pFrame, LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT ( pFrame );

	if ( pFrame->szName )
	{
		SAFE_DELETE_ARRAY(m_szAdaptFrame);
		size_t nStrLen = strlen(pFrame->szName)+1;
		m_szAdaptFrame = new char[strlen(pFrame->szName)+1];

		StringCchCopy( m_szAdaptFrame, nStrLen, pFrame->szName );
	}

	return S_OK;
}

HRESULT DxEffectWaterLight::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,	FALSE );

		pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR,			0x00000000 );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,	D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,	D3DBLEND_ONE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );	// 디퓨즈 사용
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,	D3DTOP_MODULATE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG2 );		

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedStateBlock );
		else			pd3dDevice->EndStateBlock( &m_pEffectStateBlock );
	}

	return S_OK;
}

HRESULT DxEffectWaterLight::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSavedStateBlock );
	SAFE_RELEASE( m_pEffectStateBlock );
	
	return S_OK;
}

HRESULT DxEffectWaterLight::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 텍스쳐의 읽기 오류는 무시한다.
	//
	TextureManager::LoadTexture ( m_szTexMove, pd3dDevice, m_pddsTexMove, 0, 0 );

	TextureManager::LoadTexture ( m_szTexRotate, pd3dDevice, m_pddsTexRotate, 0, 0 );

	return S_OK;
}

HRESULT DxEffectWaterLight::DeleteDeviceObjects ()
{
	TextureManager::ReleaseTexture( m_szTexMove, m_pddsTexMove );
	TextureManager::ReleaseTexture( m_szTexRotate, m_pddsTexRotate );

	return S_OK;
}

HRESULT DxEffectWaterLight::FrameMove ( float fTime, float fElapsedTime )
{
	m_fElapsedTime += fElapsedTime;
	m_fTime			= fTime;

	return S_OK;
}

HRESULT DxEffectWaterLight::Render ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan )
{
	PROFILE_BEGIN("DxEffectWaterLight");

	if ( m_pVB )
	{
		// 0.05초 단위로 업데이트.. UV 좌표를 움직여준다. 캬캬캬
		if ( m_fElapsedTime > 0.05f )
		{
			UpdateWater ( pd3dDevice );

			m_fElapsedTime = 0.0f;
		}

		DWORD dwFVFSize;
		dwFVFSize = D3DXGetFVFVertexSize ( WATERVERTEX::FVF );

		D3DXMATRIX matIdentity;
		D3DXMatrixIdentity( &matIdentity );
		pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );


		pd3dDevice->SetTexture( 0, m_pddsTexMove );
		pd3dDevice->SetTexture( 1, m_pddsTexRotate );


		//	Note : 이전 상태 백업.
		m_pSavedStateBlock->Capture();

		//	Note : 랜더링 상태 조정.
		m_pEffectStateBlock->Apply();

		pd3dDevice->SetIndices ( m_pIB );
		pd3dDevice->SetStreamSource ( 0, m_pVB, 0, dwFVFSize );
		pd3dDevice->SetFVF ( WATERVERTEX::FVF );

		pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0, 0, m_dwVertices, 0, m_dwFaces );

		//	Note : 이전상태로 복원.
		m_pSavedStateBlock->Apply();

		pd3dDevice->SetTexture ( 1, NULL );
	}

	PROFILE_END("DxEffectWaterLight");

	return S_OK;
}

HRESULT DxEffectWaterLight::CloneWaterLight ( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes *pmsMeshs )
{
	//	Note : LocalMesh, OctreeMesh 둘 중 하나를 고른다.
	//
	if ( pmsMeshs->m_pLocalMesh )
	{	
		m_dwVertices	= pmsMeshs->m_pLocalMesh->GetNumVertices ( );
		m_dwFaces		= pmsMeshs->m_pLocalMesh->GetNumFaces ( );
	}
	else
	{
		m_dwVertices	= pmsMeshs->m_pDxOctreeMesh->m_dwNumVertices;
		m_dwFaces		= pmsMeshs->m_pDxOctreeMesh->m_dwNumFaces;
	}

	WATERVERTEX*	pDestVertices;
	WORD*			pDestIndices;
	VERTEX*			pSrcVertices;
	WORD*			pSrcIndices;

	SAFE_RELEASE ( m_pVB );
	pd3dDevice->CreateVertexBuffer ( m_dwVertices*sizeof(WATERVERTEX), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, 
										WATERVERTEX::FVF, D3DPOOL_SYSTEMMEM, &m_pVB, NULL  );

	SAFE_RELEASE ( m_pIB );
	pd3dDevice->CreateIndexBuffer(	m_dwFaces*3* sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, 
									D3DPOOL_MANAGED, &m_pIB, NULL );

	
	if ( pmsMeshs->m_pLocalMesh )
	{	
		pmsMeshs->m_pLocalMesh->LockVertexBuffer ( 0L, (VOID**)&pSrcVertices );
		pmsMeshs->m_pLocalMesh->LockIndexBuffer ( 0L, (VOID**)&pSrcIndices );
	}
	else
	{
		pmsMeshs->m_pDxOctreeMesh->LockVertexBuffer ( (VOID**)&pSrcVertices );
		pmsMeshs->m_pDxOctreeMesh->LockIndexBuffer( (VOID**)&pSrcIndices );
	}





	// 처음 디폴트 값을 넣어준다.
	m_fHighHeight = m_fLowerHeight = pSrcVertices[0].Pos.y;

	// 최고점과 최저점을 찾는다.
	for ( DWORD i=0; i<m_dwVertices; i++ )
	{
		if ( m_fHighHeight < pSrcVertices[i].Pos.y )
			m_fHighHeight = pSrcVertices[i].Pos.y;
		if ( m_fLowerHeight > pSrcVertices[i].Pos.y )
			m_fLowerHeight = pSrcVertices[i].Pos.y;
	}
	float fDis = m_fHighHeight - m_fLowerHeight;
	m_fMiddleHeight = m_fLowerHeight + fDis/2.f;

	SAFE_DELETE_ARRAY ( m_pTexUV );
	m_pTexUV = new D3DXVECTOR2[m_dwVertices];

	// 0 을 넣지 못하도록
	if ( m_fScale  <= 0.001f )		m_fScale = 0.001f;

	//	Note : 버텍스 버퍼 카피
	//
	m_pVB->Lock( 0, 0, (VOID**)&pDestVertices, D3DLOCK_NOOVERWRITE );
	for ( DWORD i=0; i<m_dwVertices; i++ )
	{
		float fTempAlpha;
		if ( pSrcVertices->Pos.y-m_fMiddleHeight >= 0.f )
			fTempAlpha = ( (m_fHighHeight-pSrcVertices->Pos.y) / (fDis/2.f)) * (m_iAlphaMiddle - m_iAlphaUp) + m_iAlphaUp;
		else
			fTempAlpha = ( (m_fLowerHeight-pSrcVertices->Pos.y) / (fDis/2.f)) * (m_iAlphaMiddle - m_iAlphaDown) + m_iAlphaDown;

		fTempAlpha = fabsf(fTempAlpha);

		pDestVertices->Pos = pSrcVertices->Pos;
		pDestVertices->Diff = D3DCOLOR_ARGB ( (int)fTempAlpha, (int)m_vColor.x, (int)m_vColor.y, (int)m_vColor.z );
		pDestVertices->Tex1 = D3DXVECTOR2((pSrcVertices->Pos.x+pSrcVertices->Pos.y)*0.01f/m_fScale,(pSrcVertices->Pos.z+pSrcVertices->Pos.y)*0.01f)/m_fScale;	// 스케일
		pDestVertices->Tex2 = pDestVertices->Tex1;
		m_pTexUV[i]			= pDestVertices->Tex1;		// 초기의 UV 좌표를 가지고 있는다.
		pDestVertices++;
		pSrcVertices++;
	}
	m_pVB->Unlock ();



	//	Note : 인덱스 버퍼 카피
	//
	WORD* TempIndex = pSrcIndices;
	m_pIB->Lock ( 0, 0, (VOID**)&pDestIndices, 0 );
	memcpy ( pDestIndices, pSrcIndices, m_dwFaces*3*sizeof(WORD) );
	m_pIB->Unlock ();


	if ( pmsMeshs->m_pLocalMesh )
	{	
		pmsMeshs->m_pLocalMesh->UnlockVertexBuffer ();
		pmsMeshs->m_pLocalMesh->UnlockIndexBuffer ();
	}
	else
	{
		pmsMeshs->m_pDxOctreeMesh->UnlockVertexBuffer();
		pmsMeshs->m_pDxOctreeMesh->UnlockIndexBuffer();
	}

	return S_OK;
}

HRESULT DxEffectWaterLight::UpdateWater	( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//m_vAddTex1 += D3DXVECTOR2 ( m_fElapsedTime*0.1f*m_vDir.x*m_fVel, m_fElapsedTime*0.1f*m_vDir.z*m_fVel);
	m_vAddTex1 += D3DXVECTOR2 ( -sinf(m_fTime)*m_fElapsedTime*0.1f*m_fVel, -cosf(m_fTime)*m_fElapsedTime*0.1f*m_fVel );
	m_vAddTex2 += D3DXVECTOR2 ( sinf(m_fTime)*m_fElapsedTime*0.1f*m_fVel, cosf(m_fTime)*m_fElapsedTime*0.1f*m_fVel );

	if ( m_vAddTex1.x > 1.f )		m_vAddTex1.x -= 1.f;
	else if ( m_vAddTex1.x < -1.f ) m_vAddTex1.x += 1.f;
	if ( m_vAddTex1.y > 1.f )		m_vAddTex1.y -= 1.f;
	else if ( m_vAddTex1.y < -1.f ) m_vAddTex1.y += 1.f;
	if ( m_vAddTex2.x > 1.f )		m_vAddTex2.x -= 1.f;
	else if ( m_vAddTex2.x < -1.f ) m_vAddTex2.x += 1.f;
	if ( m_vAddTex2.y > 1.f )		m_vAddTex2.y -= 1.f;
	else if ( m_vAddTex2.y < -1.f ) m_vAddTex2.y += 1.f;



	WATERVERTEX*	pVertices;

	m_pVB->Lock( 0, 0, (VOID**)&pVertices, 0L );
	for ( DWORD i=0; i<m_dwVertices; i++ )
	{
		pVertices[i].Tex1 = m_pTexUV[i] + m_vAddTex1;	// 반대 방향 회전 --- XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX// 한쪽 방향으로 이동만 시킴
		pVertices[i].Tex2 = m_pTexUV[i] + m_vAddTex2;	// 회전 시킴
	}
	m_pVB->Unlock ();

	return S_OK;
}

HRESULT DxEffectWaterLight::ReMakeWaterLight	( LPDIRECT3DDEVICEQ pd3dDevice )
{
	WATERVERTEX*	pVertices;

	// 0 을 넣지 못하도록
	if ( m_fScale  <= 0.001f )		m_fScale = 0.001f;

	//	Note : 버텍스 버퍼 카피
	//
	float fDis = m_fHighHeight - m_fLowerHeight;
	m_pVB->Lock( 0, 0, (VOID**)&pVertices, 0L );
	for ( DWORD i=0; i<m_dwVertices; i++ )
	{
		float fTempAlpha;
		if ( pVertices->Pos.y-m_fMiddleHeight >= 0.f )
			fTempAlpha = ( (m_fHighHeight-pVertices->Pos.y) / (fDis/2.f)) * (m_iAlphaMiddle - m_iAlphaUp) + m_iAlphaUp;
		else
			fTempAlpha = ( (m_fLowerHeight-pVertices->Pos.y) / (fDis/2.f)) * (m_iAlphaMiddle - m_iAlphaDown) + m_iAlphaDown;

		fTempAlpha = fabsf(fTempAlpha);

		pVertices->Diff = D3DCOLOR_ARGB ( (int)fTempAlpha, (int)m_vColor.x, (int)m_vColor.y, (int)m_vColor.z );
		pVertices->Tex1 = D3DXVECTOR2((pVertices->Pos.x+pVertices->Pos.y)*0.01f/m_fScale,(pVertices->Pos.z+pVertices->Pos.y)*0.01f)/m_fScale;	// 스케일
		pVertices->Tex2 = pVertices->Tex1;
		m_pTexUV[i]		= pVertices->Tex1;		// 초기의 UV 좌표를 가지고 있는다.
		pVertices++;
	}
	m_pVB->Unlock ();

	return S_OK;
}

void DxEffectWaterLight::SaveBuffer ( CSerialFile &SFile )
{
	//	읽지 않고 스킵용으로 사용됨.
	SFile << sizeof(DWORD)+sizeof(DWORD)+sizeof(DWORD)
			+sizeof(DWORD)+sizeof(DIFFVERTEX)*m_pDiffNum
			+sizeof(DWORD)+sizeof(WATERVERTEX)*m_dwVertices
			+sizeof(DWORD)+sizeof(WORD)*m_dwFaces*3
			+sizeof(D3DXVECTOR2)*m_dwVertices;


	//	높이 저장
	SFile << m_fHighHeight;
	SFile << m_fMiddleHeight;
	SFile << m_fLowerHeight;

	//	점 알파값 바꾼 것들...
	SFile << m_pDiffNum;
	if ( m_pDiffVertex )
		SFile.WriteBuffer ( m_pDiffVertex, sizeof(DIFFVERTEX)*m_pDiffNum );

	//	버텍스 버퍼
	SFile << m_dwVertices;
	PBYTE pbPoints;
	m_pVB->Lock( 0, 0, (VOID**)&pbPoints, D3DLOCK_NOOVERWRITE );
	SFile.WriteBuffer ( pbPoints, sizeof(WATERVERTEX)*m_dwVertices );
	m_pVB->Unlock ();

	//	인덱스 버퍼
	SFile << m_dwFaces;
	m_pIB->Lock( 0, 0, (VOID**)&pbPoints, NULL );
	SFile.WriteBuffer ( pbPoints, sizeof(WORD)*m_dwFaces*3 );
	m_pIB->Unlock ();

	//	UV 좌표 저장 스킬. ㅋㅋ
	SFile.WriteBuffer ( m_pTexUV, sizeof(D3DXVECTOR2)*m_dwVertices );

}

void DxEffectWaterLight::LoadBufferSet ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwBuffSize;
	WATERVERTEX* pTemp;
	WORD*		 pIndices;
	WATERVERTEX* pDestTemp;
	WORD*		 pDestIndices;

	SFile >> dwBuffSize; //	읽지 않고 스킵용으로 사용되므로 실제 로드에서는 무의미.

	//DWORD dwCur = SFile.GetfTell();
	//SFile.SetOffSet ( dwCur+dwBuffSize );
	//return;

	//	높이
	SFile >> m_fHighHeight;
	SFile >> m_fMiddleHeight;
	SFile >> m_fLowerHeight;

	//	점 알파값 바꾼 것들...
	SFile >> m_pDiffNum;
	SAFE_DELETE_ARRAY ( m_pDiffVertex );
	if ( m_pDiffNum >= 1 )
	{
		m_pDiffVertex = new DIFFVERTEX[m_pDiffNum];
		SFile.ReadBuffer ( m_pDiffVertex, sizeof(DIFFVERTEX)*m_pDiffNum );
	}


	// 버텍스 버퍼
	SFile >> dwBuffSize;
	if ( dwBuffSize >= 1 )
	{
		pTemp = new WATERVERTEX[dwBuffSize];
		SFile.ReadBuffer ( pTemp, sizeof(WATERVERTEX)*dwBuffSize );
	}

	//	Note : VB Size
	//
	m_dwVertices = dwBuffSize;

	//	Note : Base Vertex Buffer 복제.
	//
	SAFE_RELEASE ( m_pVB );
	pd3dDevice->CreateVertexBuffer ( m_dwVertices*sizeof(WATERVERTEX), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, WATERVERTEX::FVF, D3DPOOL_SYSTEMMEM, &m_pVB, NULL );

	m_pVB->Lock( 0, 0, (VOID**)&pDestTemp, D3DLOCK_NOOVERWRITE );
	memcpy ( pDestTemp, pTemp, sizeof(WATERVERTEX)*dwBuffSize );
	m_pVB->Unlock ();
	
	SAFE_DELETE_ARRAY(pTemp);



	// 인덱스 버퍼
	SFile >> dwBuffSize;
	if ( dwBuffSize >= 1 )
	{
		pIndices = new WORD[dwBuffSize*3];
		SFile.ReadBuffer ( pIndices, sizeof(WORD)*dwBuffSize*3 );
	}

	//	Note : IB Size
	//
	m_dwFaces = dwBuffSize;

	//	Note : Base Index Buffer 복제.
	//
	SAFE_RELEASE ( m_pIB );
	pd3dDevice->CreateIndexBuffer ( m_dwFaces*3*sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL );

	m_pIB->Lock( 0, 0, (VOID**)&pDestIndices, NULL );
	memcpy ( pDestIndices, pIndices, sizeof(WORD)*m_dwFaces*3 );
	m_pIB->Unlock ();
	
	SAFE_DELETE_ARRAY(pIndices);



	//	UV 좌표 저장 스킬. ㅋㅋ
	SAFE_DELETE_ARRAY ( m_pTexUV );
	m_pTexUV = new D3DXVECTOR2[m_dwVertices];
	SFile.ReadBuffer ( m_pTexUV, sizeof(D3DXVECTOR2)*m_dwVertices );
}

void DxEffectWaterLight::LoadBuffer ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwBuffSize;
	PBYTE pbBuff;
	PBYTE pbPoints;

	SFile >> dwBuffSize; //	읽지 않고 스킵용으로 사용되므로 실제 로드에서는 무의미.

	//	높이
	SFile >> m_fHighHeight;
	SFile >> m_fMiddleHeight;
	SFile >> m_fLowerHeight;

	//	점 알파값 바꾼 것들...
	SFile >> m_pDiffNum;
	if ( m_pDiffNum )
	{
		SAFE_DELETE_ARRAY ( m_pDiffVertex );
		m_pDiffVertex = new DIFFVERTEX[m_pDiffNum];
		SFile.ReadBuffer ( m_pDiffVertex, sizeof(DIFFVERTEX)*m_pDiffNum );
	}


	// 버텍스 버퍼
	SFile >> dwBuffSize;
	pbBuff = new BYTE[sizeof(WATERVERTEX)*dwBuffSize];
	SFile.ReadBuffer ( pbBuff, sizeof(WATERVERTEX)*dwBuffSize );

	//	Note : VB Size
	//
	m_dwVertices = dwBuffSize;

	//	Note : Base Vertex Buffer 복제.
	//
	SAFE_RELEASE ( m_pVB );
	pd3dDevice->CreateVertexBuffer ( m_dwVertices*sizeof(WATERVERTEX), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, WATERVERTEX::FVF, D3DPOOL_SYSTEMMEM, &m_pVB, NULL );

	m_pVB->Lock( 0, 0, (VOID**)&pbPoints, D3DLOCK_NOOVERWRITE );
	memcpy ( pbPoints, pbBuff, sizeof(WATERVERTEX)*dwBuffSize );
	m_pVB->Unlock ();
	
	SAFE_DELETE(pbBuff);



	// 인덱스 버퍼
	SFile >> dwBuffSize;
	pbBuff = new BYTE[sizeof(WORD)*dwBuffSize*3];
	SFile.ReadBuffer ( pbBuff, sizeof(WORD)*dwBuffSize*3 );

	//	Note : IB Size
	//
	m_dwFaces = dwBuffSize;

	//	Note : Base Index Buffer 복제.
	//
	SAFE_RELEASE ( m_pIB );
	pd3dDevice->CreateIndexBuffer ( m_dwFaces*3*sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL );

	m_pIB->Lock( 0, 0, (VOID**)&pbPoints, NULL );
	memcpy ( pbPoints, pbBuff, sizeof(WORD)*m_dwFaces*3 );
	m_pIB->Unlock ();
	
	SAFE_DELETE(pbBuff);



	//	UV 좌표 저장 스킬. ㅋㅋ
	SAFE_DELETE_ARRAY ( m_pTexUV );
	m_pTexUV = new D3DXVECTOR2[m_dwVertices];
	SFile.ReadBuffer ( m_pTexUV, sizeof(D3DXVECTOR2)*m_dwVertices );
}

HRESULT DxEffectWaterLight::CloneData ( DxEffectBase* pSrcEffect, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;

	DxEffectWaterLight* pSrcWater = (DxEffectWaterLight*) pSrcEffect;
	GASSERT(pSrcWater->m_pVB);
	GASSERT(pSrcWater->m_pIB);
	GASSERT(pd3dDevice);

	m_fHighHeight	= pSrcWater->m_fHighHeight;
	m_fMiddleHeight = pSrcWater->m_fMiddleHeight;
	m_fLowerHeight	= pSrcWater->m_fLowerHeight;

	m_pDiffNum	= pSrcWater->m_pDiffNum;

	if ( pSrcWater->m_pDiffVertex )
	{
		SAFE_DELETE_ARRAY(m_pDiffVertex);
		m_pDiffVertex = new DIFFVERTEX[m_pDiffNum];
		memcpy ( m_pDiffVertex, pSrcWater->m_pDiffVertex, sizeof(DIFFVERTEX)*m_pDiffNum );
	}

	m_dwVertices = pSrcWater->m_dwVertices;
	m_dwFaces	= pSrcWater->m_dwFaces;

	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pIB);



	//	Note : Vertex Buffer 복제.
	//
	pd3dDevice->CreateVertexBuffer ( m_dwVertices*sizeof(WATERVERTEX), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, WATERVERTEX::FVF, D3DPOOL_SYSTEMMEM, &m_pVB, NULL );

	PBYTE pbSrcPoints, pbDesPoints;
	hr = pSrcWater->m_pVB->Lock( 0, 0, (VOID**)&pbSrcPoints, D3DLOCK_NOOVERWRITE );
	if (FAILED(hr))	goto E_ERROR;

	hr = m_pVB->Lock( 0, 0, (VOID**)&pbDesPoints, D3DLOCK_NOOVERWRITE );
	if (FAILED(hr))	goto E_ERROR;

	memcpy ( pbDesPoints, pbSrcPoints, sizeof(WATERVERTEX)*m_dwVertices );

	pSrcWater->m_pVB->Unlock ();
	m_pVB->Unlock ();



	//	Note : Index Buffer 복제.
	//
	pd3dDevice->CreateIndexBuffer ( m_dwFaces*3*sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL );

	hr = pSrcWater->m_pIB->Lock ( 0, 0, (VOID**)&pbSrcPoints, NULL );
	if (FAILED(hr))	goto E_ERROR;

	hr = m_pIB->Lock ( 0, 0, (VOID**)&pbDesPoints, NULL );
	if (FAILED(hr))	goto E_ERROR;

	memcpy ( pbDesPoints, pbSrcPoints, sizeof(WORD)*m_dwFaces*3 );

	pSrcWater->m_pIB->Unlock ();
	m_pIB->Unlock ();



	//	Note : 초기 UV 좌표 저장한 것..
	//
	if ( pSrcWater->m_pTexUV )
	{
		SAFE_DELETE_ARRAY(m_pTexUV);
		m_pTexUV = new D3DXVECTOR2[m_dwVertices];
		memcpy ( m_pTexUV, pSrcWater->m_pTexUV, sizeof(D3DXVECTOR2)*m_dwVertices );
	}

	return S_OK;


E_ERROR:
	pSrcWater->m_pVB->Unlock ();
	pSrcWater->m_pIB->Unlock ();

	m_pVB->Unlock ();
	m_pIB->Unlock ();

	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pIB);


	return E_FAIL;
}

HRESULT DxEffectWaterLight::SetBoundBox ( LPD3DXMESH pMesh )
{
	DWORD dwVertices = pMesh->GetNumVertices();

	VERTEX*	pVertices;
	pMesh->LockVertexBuffer ( 0, (VOID**)&pVertices );

	m_fMaxX = m_fMinX = pVertices[0].Pos.x;
	m_fMaxY = m_fMinY = pVertices[0].Pos.y;
	m_fMaxZ = m_fMinZ = pVertices[0].Pos.z;

	for ( DWORD i=0; i<dwVertices; i++ )
	{
		m_fMaxX = (m_fMaxX > pVertices[i].Pos.x) ? m_fMaxX : pVertices[i].Pos.x;
		m_fMinX = (m_fMinX > pVertices[i].Pos.x) ? pVertices[i].Pos.x : m_fMinX;

		m_fMaxY = (m_fMaxY > pVertices[i].Pos.y) ? m_fMaxY : pVertices[i].Pos.y;
		m_fMinY = (m_fMinY > pVertices[i].Pos.y) ? pVertices[i].Pos.y : m_fMinY;

		m_fMaxZ = (m_fMaxZ > pVertices[i].Pos.z) ? m_fMaxZ : pVertices[i].Pos.z;
		m_fMinZ = (m_fMinZ > pVertices[i].Pos.z) ? pVertices[i].Pos.z : m_fMinZ;
	}
	pMesh->UnlockVertexBuffer ();

	return S_OK;
}

void DxEffectWaterLight::GetAABBSize( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin )
{
	vMax.x = m_fMaxX;
	vMax.y = m_fMaxY;
	vMax.z = m_fMaxZ;

	vMin.x = m_fMinX;
	vMin.y = m_fMinY;
	vMin.z = m_fMinZ;

	D3DXMATRIX		matIdentity;
	D3DXMatrixIdentity ( &matIdentity );

	COLLISION::TransformAABB( vMax, vMin, matIdentity );
}


BOOL DxEffectWaterLight::IsDetectDivision( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin )
{
	D3DXVECTOR3  vCenter;
	vCenter.x = 0.f;
	vCenter.y = 0.f;
	vCenter.z = 0.f;

	return COLLISION::IsWithInPoint( vDivMax, vDivMin, vCenter );
}

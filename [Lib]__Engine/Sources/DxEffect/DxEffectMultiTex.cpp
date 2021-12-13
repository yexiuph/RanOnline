// .cpp: implementation of the DxEffectMultiTex class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "./TextureManager.h"
#include "./DxFrameMesh.h"
#include "./DxEffectDefine.h"
#include "./Collision.h"
#include "./SerialFile.h"

#include "./DxEffectMan.h"
#include "./DxShadowMap.h"

#include "./DxViewPort.h"
#include "./DxRenderStates.h"

#include "./DxEffectMultiTex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const DWORD DxEffectMultiTex::TYPEID = DEF_EFFECT_MULTITEX;
const DWORD	DxEffectMultiTex::VERSION = 0x00000101;
const char	DxEffectMultiTex::NAME[] = "[ 3 Tex 사용 ]";
const DWORD DxEffectMultiTex::FLAG = _EFF_REPLACE;

LPDIRECT3DSTATEBLOCK9	DxEffectMultiTex::m_pSB_Tex2 = NULL;
LPDIRECT3DSTATEBLOCK9	DxEffectMultiTex::m_pSB_Tex2_SAVE = NULL;
LPDIRECT3DSTATEBLOCK9	DxEffectMultiTex::m_pSB_Tex3 = NULL;
LPDIRECT3DSTATEBLOCK9	DxEffectMultiTex::m_pSB_Tex3_SAVE = NULL;
LPDIRECT3DSTATEBLOCK9	DxEffectMultiTex::m_pSB_Tex23 = NULL;
LPDIRECT3DSTATEBLOCK9	DxEffectMultiTex::m_pSB_Tex23_SAVE = NULL;

void DxEffectMultiTex::GetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD &dwVer )
{
	pProp = (PBYTE) &m_Property;
	dwSize = sizeof(MULTITEX_PROPERTY);
	dwVer = VERSION;
}

void DxEffectMultiTex::SetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD dwVer )
{
	if ( dwVer==VERSION && dwSize==sizeof(MULTITEX_PROPERTY) )
	{
		memcpy ( &m_Property, pProp, dwSize );
	}
	else if ( dwVer==100 && dwSize==sizeof(MULTITEX_PROPERTY_100) )
	{
		MULTITEX_PROPERTY_100	pProp;

		m_bTex1				= pProp.m_bTex1;
		m_bTex2				= pProp.m_bTex2;
		m_bTex3				= pProp.m_bTex3;
		m_bSequence2		= pProp.m_bSequence2;
		m_bSequence3		= pProp.m_bSequence3;
		m_bSequenceLoop2	= pProp.m_bSequenceLoop2;
		m_bSequenceLoop3	= pProp.m_bSequenceLoop3;

		m_fAlphaAVG			= pProp.m_fAlphaAVG;
		m_fAlphaDelta		= pProp.m_fAlphaDelta;
		m_fAlphaSpeed		= pProp.m_fAlphaSpeed;

		m_fAniTime01		= pProp.m_fAniTime01;
		m_nCol01			= pProp.m_nCol01;
		m_nRow01			= pProp.m_nRow01;
		m_fAniTime02		= pProp.m_fAniTime02;
		m_nCol02			= pProp.m_nCol02;
		m_nRow02			= pProp.m_nRow02;

		m_SrcBlend			= pProp.m_SrcBlend; 
		m_DestBlend			= pProp.m_DestBlend;

		m_vMoveTex1			= pProp.m_vMoveTex1;
		m_vMoveTex2			= pProp.m_vMoveTex2;
		m_vMoveTex3			= pProp.m_vMoveTex3;

		m_vColor			= pProp.m_vColor;
		m_matFrameComb		= pProp.m_matFrameComb;

		m_vMax				= pProp.m_vMax;
		m_vMin				= pProp.m_vMin;

		StringCchCopy( m_szTexture2, MAX_PATH, pProp.m_szTexture2 );
		StringCchCopy( m_szTexture3, MAX_PATH, pProp.m_szTexture3 );

		m_vScaleTex2 = D3DXVECTOR2 ( 1.f, 1.f );
		m_vScaleTex3 = D3DXVECTOR2 ( 1.f, 1.f );
	}

}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
DxEffectMultiTex::DxEffectMultiTex ()	:
	m_bTex1(TRUE),
	m_bTex2(FALSE),
	m_bTex3(FALSE),
	m_bSequence2(FALSE),
	m_bSequence3(FALSE),
	m_bSequenceLoop2(FALSE),
	m_bSequenceLoop3(FALSE),

	m_fAlphaAVG(0.5f),
	m_fAlphaDelta(0.2f),
	m_fAlphaSpeed(10.f),

	m_fAniTime01(0.03f),
	m_nCol01(4),
	m_nRow01(4),
	m_fAniTime02(0.03f),
	m_nCol02(4),
	m_nRow02(4),
	m_SrcBlend(D3DBLEND_SRCALPHA), 
	m_DestBlend(D3DBLEND_INVSRCALPHA),

	m_vMoveTex1(0.f,0.f),
	m_vMoveTex2(0.1f,0.f),
	m_vMoveTex3(0.1f,0.f),
	m_vScaleTex2( 1.f, 1.f ),
	m_vScaleTex3( 1.f, 1.f ),
	m_vColor(0xffffffff),
	m_vMax(0.f,0.f,0.f),
	m_vMin(0.f,0.f,0.f),

	m_pddsTexture2(NULL),
	m_pddsTexture3(NULL),

	m_fTime(0.f),

	m_pSrcVB(NULL),
	m_pColorVB(NULL),
	m_pIB(NULL),
	m_vAddTex1(0,0),
	m_vAddTex2(0,0),
	m_vAddTex3(0,0),
	m_dwVertices(0),

	m_dwAttribTableSize(0),
	m_pAttribTable(NULL),
	m_cMaterials(0),
	m_pMaterials(NULL),

	m_vCenter(0.f,0.f,0.f),

	m_pTexUV(NULL)
{
	StringCchCopy( m_szTexture2, MAX_PATH, "_Wa_water1.bmp" );
	StringCchCopy( m_szTexture3, MAX_PATH, "_Wa_water1.bmp" );
}

DxEffectMultiTex::~DxEffectMultiTex ()
{
	SAFE_DELETE_ARRAY ( m_pTexUV );
	SAFE_DELETE_ARRAY ( m_pAttribTable );
	SAFE_DELETE_ARRAY ( m_pMaterials );
	SAFE_RELEASE ( m_pSrcVB );
	SAFE_RELEASE ( m_pColorVB );
	SAFE_RELEASE ( m_pIB );
}

DXMATERIAL_MULTITEX::~DXMATERIAL_MULTITEX()
{
	TextureManager::ReleaseTexture( szTexture, pTexture );
}

HRESULT DxEffectMultiTex::AdaptToDxFrameChild ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	DxMeshes *pmsMeshs;

	if ( pframeCur->pmsMeshs != NULL )
	{
		pmsMeshs = pframeCur->pmsMeshs;

		while ( pmsMeshs != NULL )
		{
			if ( pmsMeshs->m_pLocalMesh )
			{
				CreateObject( pd3dDevice, pmsMeshs );
				SetBoundBox ();
			}
			pmsMeshs = pmsMeshs->pMeshNext;
		}
	}

	return hr;
}

//	Note : DxFrame 에 효과를 붙일 경우에 사용된다.
//
HRESULT DxEffectMultiTex::AdaptToDxFrame ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !pFrame )		return S_OK;

	this->pLocalFrameEffNext = pFrame->pEffect;
	pFrame->pEffect = this;

	GASSERT(pFrame->szName);
	if ( pFrame->szName )
	{
		SAFE_DELETE_ARRAY(m_szAdaptFrame);
		size_t nStrLen = strlen(pFrame->szName)+1;
		m_szAdaptFrame = new char[nStrLen];

		StringCchCopy( m_szAdaptFrame, nStrLen, pFrame->szName );
	}

	return AdaptToDxFrameChild ( pFrame, pd3dDevice );
}

HRESULT DxEffectMultiTex::AdaptToEffList ( const DxFrame *const pFrame, LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT ( pFrame );

	if ( pFrame->szName )
	{
		SAFE_DELETE_ARRAY(m_szAdaptFrame);
		size_t nStrLen = strlen(pFrame->szName)+1;
		m_szAdaptFrame = new char[nStrLen];

		StringCchCopy( m_szAdaptFrame, nStrLen, pFrame->szName );
	}

	return S_OK;
}

HRESULT DxEffectMultiTex::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		float fBias = -0.0001f;
		pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,			*((DWORD*)&fBias) );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );

		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLORARG1,	D3DTA_CURRENT );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,	D3DTOP_SELECTARG1 );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG1 );

		pd3dDevice->SetTextureStageState ( 1, D3DTSS_ALPHAARG1,	D3DTA_CURRENT );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG1 );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSB_Tex2_SAVE );
		else			pd3dDevice->EndStateBlock( &m_pSB_Tex2 );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		float fBias = -0.0001f;
		pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,			*((DWORD*)&fBias) );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_DISABLE );

		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,	D3DTOP_MODULATE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG1 );

		pd3dDevice->SetTextureStageState ( 1, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG1 );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSB_Tex3_SAVE );
		else			pd3dDevice->EndStateBlock( &m_pSB_Tex3 );
	}

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		float fBias = -0.0001f;
		pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,			*((DWORD*)&fBias) );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );

		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLORARG2,	D3DTA_CURRENT );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,	D3DTOP_MODULATE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG1 );

		pd3dDevice->SetTextureStageState ( 1, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_ALPHAARG2,	D3DTA_CURRENT );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSB_Tex23_SAVE );
		else			pd3dDevice->EndStateBlock( &m_pSB_Tex23 );
	}

	return S_OK;
}

HRESULT DxEffectMultiTex::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	
	SAFE_RELEASE( m_pSB_Tex2 );
	SAFE_RELEASE( m_pSB_Tex3 );
	SAFE_RELEASE( m_pSB_Tex23 );

	SAFE_RELEASE( m_pSB_Tex2_SAVE );
	SAFE_RELEASE( m_pSB_Tex3_SAVE );
	SAFE_RELEASE( m_pSB_Tex23_SAVE );

	return S_OK;
}

HRESULT DxEffectMultiTex::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_vAddTex1 = D3DXVECTOR2( 0.f, 0.f );
	m_vAddTex2 = D3DXVECTOR2( 0.f, 0.f );
	m_vAddTex3 = D3DXVECTOR2( 0.f, 0.f );

	return S_OK;
}

HRESULT DxEffectMultiTex::InvalidateDeviceObjects ()
{
	return S_OK;
}

HRESULT DxEffectMultiTex::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 텍스쳐의 읽기 오류는 무시한다.
	//
	TextureManager::LoadTexture( m_szTexture2, pd3dDevice, m_pddsTexture2, 0, 0, TRUE );

	TextureManager::LoadTexture( m_szTexture3, pd3dDevice, m_pddsTexture3, 0, 0, TRUE );

	return S_OK;
}

HRESULT DxEffectMultiTex::DeleteDeviceObjects ()
{
	//	Note : 이전 텍스쳐가 로드되어 있을 경우 제거.
	TextureManager::ReleaseTexture( m_szTexture2, m_pddsTexture2 );
	TextureManager::ReleaseTexture( m_szTexture3, m_pddsTexture3 );

	return S_OK;
}

void DxEffectMultiTex::ResetTexture ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	VOID* pTex = NULL;
	for ( DWORD i=0; i<m_cMaterials; i++ )
	{
		TextureManager::ReleaseTexture( m_pMaterials[i].szTexture, m_pMaterials[i].pTexture );
	}

	for ( DWORD i=0; i<m_cMaterials; i++ )
	{
		TextureManager::LoadTexture( m_pMaterials[i].szTexture, pd3dDevice, m_pMaterials[i].pTexture, 0, 0, TRUE );
	}
}

HRESULT DxEffectMultiTex::FrameMove ( float fTime, float fElapsedTime )
{
	m_fTime		= fTime;

	m_vAddTex1 += m_vMoveTex1*fElapsedTime;
	m_vAddTex2 += m_vMoveTex2*fElapsedTime;
	m_vAddTex3 += m_vMoveTex3*fElapsedTime;

	if( !m_pSrcVB )
	{
		CDebugSet::ToLogFile( "DxEffectMultiTex::FrameMove() -- m_pSrcVB -- Failed" );
		return S_OK;
	}
	if( !m_pColorVB )
	{
		CDebugSet::ToLogFile( "DxEffectMultiTex::FrameMove() -- m_pColorVB -- Failed" );
		return S_OK;
	}

	//	Note : 텍스쳐 움직이기
	UpdateDiffuse();
	UpdateUV();

	return S_OK;
}

HRESULT DxEffectMultiTex::Render ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan )
{
	PROFILE_BEGIN("DxEffectMultiTex");

	HRESULT hr = S_OK;

	//	Note : 트렌스폼 설정.
	//
	D3DXMATRIX		matWorld, matTemp;
	D3DXMATRIX		matIdentity;
	if ( pframeCur )	m_matFrameComb = pframeCur->matCombined;
	D3DXMatrixIdentity ( &matIdentity );

	hr = pd3dDevice->SetTransform ( D3DTS_WORLD, &m_matFrameComb );

	if ( m_bTex1 )
	{
		pd3dDevice->SetStreamSource ( 0, m_pSrcVB, 0, sizeof(VERTEX) );
		pd3dDevice->SetIndices ( m_pIB );
		pd3dDevice->SetFVF ( VERTEX::FVF );

		for ( DWORD i=0; i<m_dwAttribTableSize ; i++ )
		{
			pd3dDevice->SetMaterial ( &m_pMaterials[m_pAttribTable[i].AttribId].rgMaterial );

			// Get Texture 
			if( !m_pMaterials[m_pAttribTable[i].AttribId].pTexture )
			{
				TextureManager::GetTexture( m_pMaterials[m_pAttribTable[i].AttribId].szTexture, m_pMaterials[m_pAttribTable[i].AttribId].pTexture );
			}
			pd3dDevice->SetTexture( 0, m_pMaterials[m_pAttribTable[i].AttribId].pTexture );			

			pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 
												0,
												m_pAttribTable[i].VertexStart, 
												m_pAttribTable[i].VertexCount, 
												m_pAttribTable[i].FaceStart*3, 
												m_pAttribTable[i].FaceCount );
		}
	}

	if ( m_bTex2 || m_bTex3 )
	{
		// Note : StateBlock Capture
		if ( m_bTex2 && !m_bTex3 )
		{
			m_pSB_Tex2_SAVE->Capture();
			m_pSB_Tex2->Apply();
		}
		else if ( !m_bTex2 && m_bTex3 )	
		{
			m_pSB_Tex3_SAVE->Capture();
			m_pSB_Tex3->Apply();
		}
		else if ( m_bTex2 && m_bTex3 )	
		{
			m_pSB_Tex23_SAVE->Capture();
			m_pSB_Tex23->Apply();
		}

		DWORD	dwSrcBlend, dwDestBlend;
		pd3dDevice->GetRenderState ( D3DRS_SRCBLEND,			&dwSrcBlend );
		pd3dDevice->GetRenderState ( D3DRS_DESTBLEND,			&dwDestBlend );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			m_SrcBlend );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			m_DestBlend );


		pd3dDevice->SetStreamSource( 0, m_pColorVB, 0, sizeof(VERTEXCOLORTEX2) );
		pd3dDevice->SetIndices( m_pIB );
		pd3dDevice->SetFVF( VERTEXCOLORTEX2::FVF );

		// Get Texture 
		if( !m_pddsTexture2 )	TextureManager::GetTexture( m_szTexture2, m_pddsTexture2 );
		if( !m_pddsTexture3 )	TextureManager::GetTexture( m_szTexture3, m_pddsTexture3 );

		pd3dDevice->SetTexture( 0, m_pddsTexture2 );
		pd3dDevice->SetTexture( 1, m_pddsTexture3 );

		pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0, 0, m_dwVertices, 0, m_dwFaces );

		pd3dDevice->SetTexture( 1, NULL );

		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			dwSrcBlend );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			dwDestBlend );

		// Note : StateBlock Apply
		if ( m_bTex2 && !m_bTex3 )		m_pSB_Tex2_SAVE->Apply();
		else if ( !m_bTex2 && m_bTex3 )	m_pSB_Tex3_SAVE->Apply();
		else if ( m_bTex2 && m_bTex3 )	m_pSB_Tex23_SAVE->Apply();
	}

	PROFILE_END("DxEffectMultiTex");

	return S_OK;
}

VOID DxEffectMultiTex::UpdateDiffuse ()
{
	HRESULT hr(S_OK);

	float fAlpha = m_fAlphaAVG + sinf(m_fTime*m_fAlphaSpeed)*m_fAlphaDelta;
	if ( fAlpha < 0.f )		fAlpha = 0.f;
	if ( fAlpha > 1.f )		fAlpha = 1.f;
	m_vColor = ((DWORD)(fAlpha*255)<<24) + (m_vColor&0xff0000) + (m_vColor&0xff00) + (m_vColor&0xff);

	VERTEXCOLORTEX2*	pColorVert;
	hr = m_pColorVB->Lock( 0, 0, (VOID**)&pColorVert, 0 );
	if( FAILED(hr) )	
	{
		CDebugSet::ToLogFile( "DxEffectMultiTex::UpdateDiffuse() -- m_pColorVB->Lock() -- Failed" );
		return;
	}

	for ( DWORD i=0; i<m_dwVertices; i++ )
	{
		pColorVert[i].dwColor = m_vColor;
	}
	m_pColorVB->Unlock ();
}

VOID DxEffectMultiTex::UpdateUV()
{
	HRESULT hr(S_OK);

	if ( m_bTex1 && (m_vMoveTex1.x != 0.f || m_vMoveTex1.y != 0.f) )
	{
		VERTEX*	pVert;
		hr = m_pSrcVB->Lock ( 0, 0, (VOID**)&pVert, 0 );
		if( FAILED(hr) )	
		{
			CDebugSet::ToLogFile( "DxEffectMultiTex::UpdateUV() -- m_pSrcVB->Lock() -- Failed" );
			return;
		}

		for ( DWORD i=0; i<m_dwVertices; i++ )
		{
		//	pVert[i].vTex = m_pTexUV[i] + m_vAddTex1;
			pVert[i].vTex = m_pTexUV[i] - m_vAddTex1;
		}
		m_pSrcVB->Unlock ();
	}

	if ( m_bTex2 || m_bTex3 )
	{
		VERTEXCOLORTEX2*	pColorVert;
		hr = m_pColorVB->Lock ( 0, 0, (VOID**)&pColorVert, 0 );
		if( FAILED(hr) )	
		{
			CDebugSet::ToLogFile( "DxEffectMultiTex::UpdateUV() -- m_pColorVB->Lock() -- Failed" );
			return;
		}

		for ( DWORD i=0; i<m_dwVertices; i++ )
		{
		//	pColorVert[i].vTex1 = m_pTexUV[i] + m_vAddTex2;
			pColorVert[i].vTex01.x = m_pTexUV[i].x*m_vScaleTex2.x - m_vAddTex2.x;
			pColorVert[i].vTex01.y = m_pTexUV[i].y*m_vScaleTex2.y - m_vAddTex2.y;

		//	pColorVert[i].vTex2 = m_pTexUV[i] + m_vAddTex3;
			pColorVert[i].vTex02.x = m_pTexUV[i].x*m_vScaleTex3.x - m_vAddTex3.x;
			pColorVert[i].vTex02.y = m_pTexUV[i].y*m_vScaleTex3.y - m_vAddTex3.y;
		}
		m_pColorVB->Unlock ();
	}
}







HRESULT DxEffectMultiTex::CreateObject( LPDIRECT3DDEVICEQ pd3dDevice, DxMeshes *pmsMeshs )
{
	HRESULT	 hr = S_OK;

	VERTEX*	pVertices;
	WORD*	pIndices;

	VERTEX*	pDestVert;
	WORD*		pDestIndices;

	//	Note : LocalMesh, OctreeMesh 둘 중 하나를 고른다.
	if ( pmsMeshs->m_pLocalMesh )
	{	
		m_dwVertices	= pmsMeshs->m_pLocalMesh->GetNumVertices ( );
		m_dwFaces		= pmsMeshs->m_pLocalMesh->GetNumFaces ( );

		pmsMeshs->m_pLocalMesh->LockVertexBuffer ( 0L, (VOID**)&pVertices );
		pmsMeshs->m_pLocalMesh->LockIndexBuffer ( 0L, (VOID**)&pIndices );
	}
	else
	{
		m_dwVertices	= pmsMeshs->m_pDxOctreeMesh->m_dwNumVertices;
		m_dwFaces		= pmsMeshs->m_pDxOctreeMesh->m_dwNumFaces;

		pmsMeshs->m_pDxOctreeMesh->LockVertexBuffer( (VOID**)&pVertices );
		pmsMeshs->m_pDxOctreeMesh->LockIndexBuffer( (VOID**)&pIndices );
	}

	//	
	//
	SAFE_DELETE_ARRAY ( m_pTexUV );
	m_pTexUV = new D3DXVECTOR2[m_dwVertices];


	//	Note : 원본 복제 하기
	SAFE_RELEASE ( m_pSrcVB );
	hr = pd3dDevice->CreateVertexBuffer ( m_dwVertices*sizeof(VERTEX), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, VERTEX::FVF, D3DPOOL_SYSTEMMEM, &m_pSrcVB, NULL );
	if( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "DxEffectMultiTex::CreateObject() -- CreateVertexBuffer() -- Failed" );
		return S_OK;
	}

	hr = m_pSrcVB->Lock ( 0, 0, (VOID**)&pDestVert, 0 );
	if( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "DxEffectMultiTex::CreateObject() -- m_pSrcVB->Lock() -- Failed" );
		return S_OK;
	}

	m_vMax = m_vMin = pVertices[0].vPos;	// 초기값 셋팅

	for ( DWORD i=0; i<m_dwVertices; i++ )
	{
		pDestVert[i].vPos		= pVertices[i].vPos;
		pDestVert[i].vNor		= pVertices[i].vNor;
		pDestVert[i].vTex		= pVertices[i].vTex;
		m_pTexUV[i]				= pVertices[i].vTex;

		m_vMax.x = (pVertices[i].vPos.x > m_vMax.x) ? pVertices[i].vPos.x : m_vMax.x ;
		m_vMax.y = (pVertices[i].vPos.y > m_vMax.y) ? pVertices[i].vPos.y : m_vMax.y ;
		m_vMax.z = (pVertices[i].vPos.z > m_vMax.z) ? pVertices[i].vPos.z : m_vMax.z ;

		m_vMin.x = (pVertices[i].vPos.x < m_vMin.x) ? pVertices[i].vPos.x : m_vMin.x ;
		m_vMin.y = (pVertices[i].vPos.y < m_vMin.y) ? pVertices[i].vPos.y : m_vMin.y ;
		m_vMin.z = (pVertices[i].vPos.z < m_vMin.z) ? pVertices[i].vPos.z : m_vMin.z ;
	}
	m_pSrcVB->Unlock ();

	//	Note : Diffuse 만들기
	//
	VERTEXCOLORTEX2*	pColorVert;
	SAFE_RELEASE ( m_pColorVB );
	hr = pd3dDevice->CreateVertexBuffer ( m_dwVertices*sizeof(VERTEXCOLORTEX2), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, VERTEXCOLORTEX2::FVF, D3DPOOL_SYSTEMMEM, &m_pColorVB, NULL );
	if( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "DxEffectMultiTex::CreateObject() -- CreateVertexBuffer(m_pColorVB) -- Failed" );
		return S_OK;
	}

	hr = m_pColorVB->Lock ( 0, 0, (VOID**)&pColorVert, 0 );
	if( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "DxEffectMultiTex::CreateObject() -- m_pColorVB->Lock() -- Failed" );
		return S_OK;
	}

	m_vMax = m_vMin = pVertices[0].vPos;	// 초기값 셋팅

	for ( DWORD i=0; i<m_dwVertices; i++ )
	{
		pColorVert[i].vPos		= pVertices[i].vPos;
		pColorVert[i].dwColor	= m_vColor;
		pColorVert[i].vTex01	= pVertices[i].vTex;
		pColorVert[i].vTex02	= pVertices[i].vTex;
	}
	m_pColorVB->Unlock ();

	//	Note : IB
	//
	SAFE_RELEASE ( m_pIB );
	hr = pd3dDevice->CreateIndexBuffer ( 3*m_dwFaces*sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL );
	if( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "DxEffectMultiTex::CreateObject() -- CreateIndexBuffer -- Failed" );
		return S_OK;
	}

	hr = m_pIB->Lock ( 0, 0, (VOID**)&pDestIndices, 0 );
	if( FAILED(hr) )
	{
		CDebugSet::ToLogFile( "DxEffectMultiTex::CreateObject() -- m_pIB->Lock() -- Failed" );
		return S_OK;
	}

	for ( DWORD i=0; i<m_dwFaces*3; i++ )
	{
		pDestIndices[i] = pIndices[i];
	}
	m_pIB->Unlock ();


	//	Note : LocalMesh, OctreeMesh 둘 중 하나를 고른다.
	//
	if ( pmsMeshs->m_pLocalMesh )
	{	
		pmsMeshs->m_pLocalMesh->UnlockVertexBuffer();
		pmsMeshs->m_pLocalMesh->UnlockIndexBuffer();
	}
	else
	{
		pmsMeshs->m_pDxOctreeMesh->UnlockVertexBuffer ( );
		pmsMeshs->m_pDxOctreeMesh->UnlockIndexBuffer ( );
	}

	//	Note : AttributeTable 얻어오기	( 기존 매트리얼과 텍스쳐를 사용 가능하도록 하기위해.. )
	//	
	if ( pmsMeshs->m_pLocalMesh )
	{	
		pmsMeshs->m_pLocalMesh->GetAttributeTable ( NULL, &m_dwAttribTableSize );
	}
	else
	{
		m_dwAttribTableSize = pmsMeshs->m_pDxOctreeMesh->m_dwAttribTableSize;
	}
	
	SAFE_DELETE_ARRAY ( m_pAttribTable );
	m_pAttribTable = new D3DXATTRIBUTERANGE [ m_dwAttribTableSize ];

	if ( pmsMeshs->m_pLocalMesh )
	{
		pmsMeshs->m_pLocalMesh->GetAttributeTable( m_pAttribTable, &m_dwAttribTableSize );
	}
	else
	{
		memcpy ( m_pAttribTable, pmsMeshs->m_pDxOctreeMesh->m_pAttribTable, sizeof ( D3DXATTRIBUTERANGE ) * m_dwAttribTableSize );
	}

	m_cMaterials = pmsMeshs->cMaterials;

	SAFE_DELETE_ARRAY ( m_pMaterials );
	m_pMaterials = new DXMATERIAL_MULTITEX[pmsMeshs->cMaterials];

	for ( DWORD i=0; i<m_cMaterials; i++ )
	{
		StringCchCopy( m_pMaterials[i].szTexture, MAX_PATH, pmsMeshs->strTextureFiles[i].GetString() );

		hr = TextureManager::LoadTexture( m_pMaterials[i].szTexture, pd3dDevice, m_pMaterials[i].pTexture, 0, 0, TRUE );
		if (FAILED(hr))
		{
			hr = S_OK;
			m_pMaterials[i].pTexture = NULL;
		}

		m_pMaterials[i].rgMaterial = pmsMeshs->rgMaterials[i];
	}

	//	Note : 스케일 조정하기
	{
		VERTEX*				pSrcVert;
		VERTEXCOLORTEX2*	pDestVert;
		WORD*				pIndices;

		hr = m_pSrcVB->Lock ( 0, 0, (VOID**)&pSrcVert, NULL );
		if( FAILED(hr) )
		{
			CDebugSet::ToLogFile( "DxEffectMultiTex::CreateObject() -- m_pSrcVB->Lock(2) -- Failed" );
			return S_OK;
		}

		hr = m_pColorVB->Lock ( 0, 0, (VOID**)&pDestVert, NULL );
		if( FAILED(hr) )
		{
			m_pSrcVB->Unlock ();
			CDebugSet::ToLogFile( "DxEffectMultiTex::CreateObject() -- m_pColorVB->Lock(2) -- Failed" );
			return S_OK;
		}

		hr = m_pIB->Lock ( 0, 0, (VOID**)&pIndices, NULL );
		if( FAILED(hr) )
		{
			m_pSrcVB->Unlock ();
			m_pColorVB->Unlock ();
			CDebugSet::ToLogFile( "DxEffectMultiTex::CreateObject() -- m_pIB->Lock(2) -- Failed" );
			return S_OK;
		}

		ChangeObject ( pSrcVert, pDestVert, pIndices, m_dwVertices, m_dwFaces, 0.02f );

		m_pSrcVB->Unlock ();
		m_pColorVB->Unlock ();
		m_pIB->Unlock ();

	}

	return hr;
}

HRESULT	DxEffectMultiTex::ChangeObject ( VERTEX* pSrcVert, VERTEXCOLORTEX2* pDestVert, WORD* pIndices, DWORD dwVert, DWORD dwFaces, float fScale )
{
	WORD			wVertex;
	D3DXVECTOR3*	pTempNormal;
	D3DXVECTOR3		vTempNormal;

	//	Note : 임시 저장소 생성 및 초기화
	//
	pTempNormal = new D3DXVECTOR3[dwVert];
	for ( DWORD i=0; i<dwVert; i++ )
	{
		pTempNormal[i] = D3DXVECTOR3 ( 0.f, 0.f, 0.f );
	}

	//	임시 저장소에 값 넣기
	//
	for ( DWORD i=0; i<dwFaces; i++ )
	{
		wVertex = pIndices[i*3+0];
		pTempNormal[wVertex] += pSrcVert[wVertex].vNor;

		wVertex = pIndices[i*3+1];
		pTempNormal[wVertex] += pSrcVert[wVertex].vNor;

		wVertex = pIndices[i*3+2];
		pTempNormal[wVertex] += pSrcVert[wVertex].vNor;
	}

	//	임시 저장소에 있는 값 정리하기
	//
	for ( DWORD i=0; i<dwVert; i++ )
	{
		vTempNormal = pTempNormal[i];
		D3DXVec3Normalize ( &vTempNormal, &vTempNormal );
		pTempNormal[i] = D3DXVECTOR3 ( vTempNormal.x*fScale, vTempNormal.y*fScale, vTempNormal.z*fScale );
	}

	//	Note : 변환값 넣기
	//	
	for ( DWORD i=0; i<dwVert; i++ )
	{
		pDestVert[i].vPos = pSrcVert[i].vPos + pTempNormal[i];
	}

	//	Note : 정리
	//
	SAFE_DELETE_ARRAY ( pTempNormal );

	return S_OK;
}





void DxEffectMultiTex::SaveBuffer ( CSerialFile &SFile )
{
	//	읽지 않고 스킵용으로 사용됨.
	SFile << sizeof(DWORD)+sizeof(VERTEX)*m_dwVertices
			+sizeof(VERTEXCOLORTEX2)*m_dwVertices
			+sizeof(DWORD)+sizeof(WORD)*m_dwFaces*3
			+sizeof(D3DXVECTOR2)*m_dwVertices
			+sizeof(DWORD)+sizeof(D3DXATTRIBUTERANGE)*m_dwAttribTableSize
			+sizeof(DWORD)+sizeof(DXMATERIAL_MULTITEX)*m_cMaterials;

	//	버텍스 버퍼
	SFile << m_dwVertices;
	PBYTE pbPoints;
	m_pSrcVB->Lock( 0, 0, (VOID**)&pbPoints, NULL );
	SFile.WriteBuffer ( pbPoints, sizeof(VERTEX)*m_dwVertices );
	m_pSrcVB->Unlock ();

	//	버텍스 버퍼 Color
	m_pColorVB->Lock( 0, 0, (VOID**)&pbPoints, NULL );
	SFile.WriteBuffer ( pbPoints, sizeof(VERTEXCOLORTEX2)*m_dwVertices );
	m_pColorVB->Unlock ();

	//	인덱스 버퍼
	SFile << m_dwFaces;
	m_pIB->Lock( 0, 0, (VOID**)&pbPoints, NULL );
	SFile.WriteBuffer ( pbPoints, sizeof(WORD)*m_dwFaces*3 );
	m_pIB->Unlock ();

	//	UV 좌표 저장 스킬. ㅋㅋ
	SFile.WriteBuffer ( m_pTexUV, sizeof(D3DXVECTOR2)*m_dwVertices );

	// AttributeTable 저장
	SFile << m_dwAttribTableSize;
	SFile.WriteBuffer ( m_pAttribTable, sizeof(D3DXATTRIBUTERANGE)*m_dwAttribTableSize );

	// Material 저장
	SFile << m_cMaterials;
	SFile.WriteBuffer ( m_pMaterials, sizeof(DXMATERIAL_MULTITEX)*m_cMaterials );
}

void DxEffectMultiTex::LoadBufferSet ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr(S_OK);
	VERTEX*				pDestTemp;
	VERTEXCOLORTEX2*	pColorVert;
	WORD*				pDestIndices;

	DWORD dwSAVESIZE;
	SFile >> dwSAVESIZE; //	읽지 않고 스킵용으로 사용되므로 실제 로드에서는 무의미.

	int nfTell_a = SFile.GetfTell ();


	SFile >> m_dwVertices;

	// 버텍스 버퍼
	SAFE_RELEASE ( m_pSrcVB );
	hr = pd3dDevice->CreateVertexBuffer ( m_dwVertices*sizeof(VERTEX), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, VERTEX::FVF, D3DPOOL_SYSTEMMEM, &m_pSrcVB, NULL );
	if( FAILED(hr) )	CDebugSet::ToLogFile( "DxEffectMultiTex::LoadBufferSet() -- CreateVertexBuffer(m_pSrcVB) -- Failed" );

	hr = m_pSrcVB->Lock( 0, 0, (VOID**)&pDestTemp, NULL );
	if( FAILED(hr) )	CDebugSet::ToLogFile( "DxEffectMultiTex::LoadBufferSet() -- m_pSrcVB->Lock() -- Failed" );
	else
	{
		SFile.ReadBuffer( pDestTemp, sizeof(VERTEX)*m_dwVertices );
		m_pSrcVB->Unlock ();
	}


	// 버텍스 버퍼		Color
	SAFE_RELEASE ( m_pColorVB );
	hr = pd3dDevice->CreateVertexBuffer ( m_dwVertices*sizeof(VERTEXCOLORTEX2), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, VERTEXCOLORTEX2::FVF, D3DPOOL_SYSTEMMEM, &m_pColorVB, NULL );
	if( FAILED(hr) )	CDebugSet::ToLogFile( "DxEffectMultiTex::LoadBufferSet() -- CreateVertexBuffer(m_pColorVB) -- Failed" );

	hr = m_pColorVB->Lock( 0, 0, (VOID**)&pColorVert, NULL );
	if( FAILED(hr) )	CDebugSet::ToLogFile( "DxEffectMultiTex::LoadBufferSet() -- m_pColorVB->Lock() -- Failed" );
	else
	{
		SFile.ReadBuffer( pColorVert, sizeof(VERTEXCOLORTEX2)*m_dwVertices );
		m_pColorVB->Unlock ();
	}


	// 인덱스 버퍼
	SFile >> m_dwFaces;

	SAFE_RELEASE ( m_pIB );
	hr = pd3dDevice->CreateIndexBuffer ( m_dwFaces*3*sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL );
	if( FAILED(hr) )	CDebugSet::ToLogFile( "DxEffectMultiTex::LoadBufferSet() -- CreateIndexBuffer() -- Failed" );

	hr = m_pIB->Lock( 0, 0, (VOID**)&pDestIndices, NULL );
	if( FAILED(hr) )	CDebugSet::ToLogFile( "DxEffectMultiTex::LoadBufferSet() -- m_pIB->Lock() -- Failed" );
	else
	{
		SFile.ReadBuffer( pDestIndices, sizeof(WORD)*m_dwFaces*3 );
		m_pIB->Unlock ();
	}


	//	UV 좌표 저장 스킬. ㅋㅋ
	SAFE_DELETE_ARRAY ( m_pTexUV );
	m_pTexUV = new D3DXVECTOR2[m_dwVertices];
	SFile.ReadBuffer ( m_pTexUV, sizeof(D3DXVECTOR2)*m_dwVertices );


	// AttributeTable 저장
	SFile >> m_dwAttribTableSize;
	SAFE_DELETE_ARRAY ( m_pAttribTable );
	m_pAttribTable = new D3DXATTRIBUTERANGE[m_dwAttribTableSize];
	SFile.ReadBuffer ( m_pAttribTable, sizeof(D3DXATTRIBUTERANGE)*m_dwAttribTableSize );


	// Material 저장
	SFile >> m_cMaterials;
	SAFE_DELETE_ARRAY ( m_pMaterials );
	m_pMaterials = new DXMATERIAL_MULTITEX[m_cMaterials];
	SFile.ReadBuffer ( m_pMaterials, sizeof(DXMATERIAL_MULTITEX)*m_cMaterials );

	for ( DWORD i=0; i<m_cMaterials; i++ )
	{
		m_pMaterials[i].pTexture = NULL;
		TextureManager::LoadTexture( m_pMaterials[i].szTexture, pd3dDevice, m_pMaterials[i].pTexture, 0, 0, TRUE );
	}

	int nfTell_b = SFile.GetfTell ();
	GASSERT ( DWORD(nfTell_b-nfTell_a)==dwSAVESIZE);
}

void DxEffectMultiTex::LoadBuffer ( CSerialFile &SFile, DWORD dwVer, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr(S_OK);

	DWORD dwBuffSize;

	SFile >> dwBuffSize; //	읽지 않고 스킵용으로 사용되므로 실제 로드에서는 무의미.

	VERTEX*				pDestTemp;
	VERTEXCOLORTEX2*	pColorVert;
	WORD*				pDestIndices;

	SFile >> m_dwVertices;

	// 버텍스 버퍼
	SAFE_RELEASE ( m_pSrcVB );
	hr = pd3dDevice->CreateVertexBuffer ( m_dwVertices*sizeof(VERTEX), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, VERTEX::FVF, D3DPOOL_SYSTEMMEM, &m_pSrcVB, NULL );
	if( FAILED(hr) )	CDebugSet::ToLogFile( "DxEffectMultiTex::LoadBufferSet() -- CreateVertexBuffer(m_pSrcVB) -- Failed" );

	hr = m_pSrcVB->Lock( 0, 0, (VOID**)&pDestTemp, NULL );
	if( FAILED(hr) )	CDebugSet::ToLogFile( "DxEffectMultiTex::LoadBufferSet() -- m_pSrcVB->Lock() -- Failed" );
	else
	{
		SFile.ReadBuffer( pDestTemp, sizeof(VERTEX)*m_dwVertices );
		m_pSrcVB->Unlock ();
	}


	// 버텍스 버퍼		Color
	SAFE_RELEASE ( m_pColorVB );
	hr = pd3dDevice->CreateVertexBuffer ( m_dwVertices*sizeof(VERTEXCOLORTEX2), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, VERTEXCOLORTEX2::FVF, D3DPOOL_SYSTEMMEM, &m_pColorVB, NULL );
	if( FAILED(hr) )	CDebugSet::ToLogFile( "DxEffectMultiTex::LoadBufferSet() -- CreateVertexBuffer(m_pColorVB) -- Failed" );

	hr = m_pColorVB->Lock( 0, 0, (VOID**)&pColorVert, NULL );
	if( FAILED(hr) )	CDebugSet::ToLogFile( "DxEffectMultiTex::LoadBufferSet() -- m_pColorVB->Lock() -- Failed" );
	else
	{
		SFile.ReadBuffer( pColorVert, sizeof(VERTEXCOLORTEX2)*m_dwVertices );
		m_pColorVB->Unlock ();
	}


	// 인덱스 버퍼
	SFile >> m_dwFaces;

	SAFE_RELEASE ( m_pIB );
	hr = pd3dDevice->CreateIndexBuffer ( m_dwFaces*3*sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL );
	if( FAILED(hr) )	CDebugSet::ToLogFile( "DxEffectMultiTex::LoadBufferSet() -- CreateIndexBuffer() -- Failed" );

	hr = m_pIB->Lock( 0, 0, (VOID**)&pDestIndices, NULL );
	if( FAILED(hr) )	CDebugSet::ToLogFile( "DxEffectMultiTex::LoadBufferSet() -- m_pIB->Lock() -- Failed" );
	else
	{
		SFile.ReadBuffer( pDestIndices, sizeof(WORD)*m_dwFaces*3 );
		m_pIB->Unlock ();
	}

	//	UV 좌표 저장 스킬. ㅋㅋ
	SAFE_DELETE_ARRAY ( m_pTexUV );
	m_pTexUV = new D3DXVECTOR2[m_dwVertices];
	SFile.ReadBuffer ( m_pTexUV, sizeof(D3DXVECTOR2)*m_dwVertices );


	// AttributeTable 저장
	SFile >> m_dwAttribTableSize;
	SAFE_DELETE_ARRAY ( m_pAttribTable );
	m_pAttribTable = new D3DXATTRIBUTERANGE[m_dwAttribTableSize];
	SFile.ReadBuffer ( m_pAttribTable, sizeof(D3DXATTRIBUTERANGE)*m_dwAttribTableSize );

	// Material 저장
	SFile >> m_cMaterials;
	SAFE_DELETE_ARRAY ( m_pMaterials );
	m_pMaterials = new DXMATERIAL_MULTITEX[m_cMaterials];
	SFile.ReadBuffer ( m_pMaterials, sizeof(DXMATERIAL_MULTITEX)*m_cMaterials );

	for ( DWORD i=0; i<m_cMaterials; i++ )
	{
		m_pMaterials[i].pTexture = NULL;
		TextureManager::LoadTexture( m_pMaterials[i].szTexture, pd3dDevice, m_pMaterials[i].pTexture, 0, 0, TRUE );
	}
}

HRESULT DxEffectMultiTex::CloneData ( DxEffectBase* pSrcEffect, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;

	DxEffectMultiTex* pSrcEff = (DxEffectMultiTex*) pSrcEffect;
	GASSERT(pSrcEff->m_pSrcVB);
	GASSERT(pSrcEff->m_pColorVB);
	GASSERT(pSrcEff->m_pIB);
	GASSERT(pd3dDevice);

	m_dwVertices = pSrcEff->m_dwVertices;
	m_dwFaces	= pSrcEff->m_dwFaces;

	SAFE_RELEASE(m_pSrcVB);
	SAFE_RELEASE(m_pColorVB);
	SAFE_RELEASE(m_pIB);



	//	Note : Vertex Buffer 복제.
	//
	SAFE_RELEASE ( m_pSrcVB );
	pd3dDevice->CreateVertexBuffer ( m_dwVertices*sizeof(VERTEX), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, VERTEX::FVF, D3DPOOL_SYSTEMMEM, &m_pSrcVB, NULL );

	PBYTE pbSrcPoints, pbDesPoints;
	hr = pSrcEff->m_pSrcVB->Lock ( 0, 0, (VOID**)&pbSrcPoints, NULL );
	if (FAILED(hr))	goto E_ERROR;

	hr = m_pSrcVB->Lock ( 0, 0, (VOID**)&pbDesPoints, NULL );
	if (FAILED(hr))	goto E_ERROR;

	memcpy ( pbDesPoints, pbSrcPoints, sizeof(VERTEX)*m_dwVertices );

	pSrcEff->m_pSrcVB->Unlock ();

	m_pSrcVB->Unlock ();

	//	Note : Vertex Buffer 복제.	Color
	//
	SAFE_RELEASE ( m_pColorVB );
	pd3dDevice->CreateVertexBuffer ( m_dwVertices*sizeof(VERTEXCOLORTEX2), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, VERTEXCOLORTEX2::FVF, D3DPOOL_SYSTEMMEM, &m_pColorVB, NULL );

	hr = pSrcEff->m_pColorVB->Lock ( 0, 0, (VOID**)&pbSrcPoints, NULL );
	if (FAILED(hr))	goto E_ERROR;

	hr = m_pColorVB->Lock ( 0, 0, (VOID**)&pbDesPoints, NULL );
	if (FAILED(hr))	goto E_ERROR;

	memcpy ( pbDesPoints, pbSrcPoints, sizeof(VERTEXCOLORTEX2)*m_dwVertices );

	pSrcEff->m_pColorVB->Unlock ();

	m_pColorVB->Unlock ();


	//	Note : Index Buffer 복제.
	//
	SAFE_RELEASE ( m_pIB );
	pd3dDevice->CreateIndexBuffer ( m_dwFaces*3*sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL );

	hr = pSrcEff->m_pIB->Lock ( 0, 0, (VOID**)&pbSrcPoints, NULL );
	if (FAILED(hr))	goto E_ERROR;

	hr = m_pIB->Lock ( 0, 0, (VOID**)&pbDesPoints, NULL );
	if (FAILED(hr))	goto E_ERROR;

	memcpy ( pbDesPoints, pbSrcPoints, sizeof(WORD)*m_dwFaces*3 );

	pSrcEff->m_pIB->Unlock ();
	m_pIB->Unlock ();



	//	Note : 초기 UV 좌표 저장한 것..
	//
	if ( pSrcEff->m_pTexUV )
	{
		SAFE_DELETE_ARRAY(m_pTexUV);
		m_pTexUV = new D3DXVECTOR2[m_dwVertices];
		memcpy ( m_pTexUV, pSrcEff->m_pTexUV, sizeof(D3DXVECTOR2)*m_dwVertices );
	}


	// AttributeTable 저장
	if ( pSrcEff->m_pAttribTable )
	{
		m_dwAttribTableSize = pSrcEff->m_dwAttribTableSize;

		SAFE_DELETE_ARRAY ( m_pAttribTable );
		m_pAttribTable = new D3DXATTRIBUTERANGE[pSrcEff->m_dwAttribTableSize];
		memcpy ( m_pAttribTable, pSrcEff->m_pAttribTable, sizeof(D3DXATTRIBUTERANGE)*m_dwAttribTableSize );
	}


	// Material 저장
	if ( pSrcEff->m_pMaterials )
	{
		m_cMaterials = pSrcEff->m_cMaterials;

		SAFE_DELETE_ARRAY ( m_pMaterials );
		m_pMaterials = new DXMATERIAL_MULTITEX[pSrcEff->m_cMaterials];
		memcpy ( m_pMaterials, pSrcEff->m_pMaterials, sizeof(DXMATERIAL_MULTITEX)*m_cMaterials );
	}


	return S_OK;


E_ERROR:
	CDebugSet::ToLogFile( "DxEffectMultiTex::CloneData() -- goto E_ERROR;" );

	pSrcEff->m_pSrcVB->Unlock ();
	pSrcEff->m_pColorVB->Unlock ();
	pSrcEff->m_pIB->Unlock ();

	m_pSrcVB->Unlock ();
	m_pColorVB->Unlock ();
	m_pIB->Unlock ();

	SAFE_RELEASE(m_pSrcVB);
	SAFE_RELEASE(m_pColorVB);
	SAFE_RELEASE(m_pIB);


	return E_FAIL;
}

HRESULT DxEffectMultiTex::SetBoundBox ()
{
	VERTEX*	pVertices;

	m_pSrcVB->Lock ( 0, 0, (VOID**)&pVertices, 0 );

	m_vMax = m_vMin = pVertices[0].vPos;	// 초기값 셋팅

	for ( DWORD i=0; i<m_dwVertices; i++ )
	{
		m_vMax.x = (pVertices[i].vPos.x > m_vMax.x) ? pVertices[i].vPos.x : m_vMax.x ;
		m_vMax.y = (pVertices[i].vPos.y > m_vMax.y) ? pVertices[i].vPos.y : m_vMax.y ;
		m_vMax.z = (pVertices[i].vPos.z > m_vMax.z) ? pVertices[i].vPos.z : m_vMax.z ;

		m_vMin.x = (pVertices[i].vPos.x < m_vMin.x) ? pVertices[i].vPos.x : m_vMin.x ;
		m_vMin.y = (pVertices[i].vPos.y < m_vMin.y) ? pVertices[i].vPos.y : m_vMin.y ;
		m_vMin.z = (pVertices[i].vPos.z < m_vMin.z) ? pVertices[i].vPos.z : m_vMin.z ;
	}
	m_pSrcVB->Unlock ();

	return S_OK;
}

void DxEffectMultiTex::GetAABBSize( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin )
{
	vMax = m_vMax;
	vMin = m_vMin;

	D3DXMATRIX		matIdentity;
	D3DXMatrixIdentity ( &matIdentity );

	COLLISION::TransformAABB( vMax, vMin, m_matFrameComb );
}


BOOL DxEffectMultiTex::IsDetectDivision( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin )
{
	D3DXVECTOR3  vCenter;
	vCenter.x = 0.f;
	vCenter.y = 0.f;
	vCenter.z = 0.f;

	return COLLISION::IsWithInPoint( vDivMax, vDivMin, vCenter );
}





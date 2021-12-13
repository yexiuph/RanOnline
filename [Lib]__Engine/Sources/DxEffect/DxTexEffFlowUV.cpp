#include "pch.h"

#include "./GLDefine.h"
#include "./SerialFile.h"
#include "./DxDynamicVB.h"
#include "./TextureManager.h"

#include "./DxTexEffFlowUV.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const DWORD DxTexEffFlowUV::TYPEID = DEF_TEXEFF_FLOWUV;
const DWORD	DxTexEffFlowUV::VERSION = 0x00000100;
const char DxTexEffFlowUV::NAME[] = "[ Flow UV ]";

// Note : TEXEFF_FLOWUV_PROPERTY
TEXEFF_FLOWUV_PROPERTY::TEXEFF_FLOWUV_PROPERTY() :
	dwFlag(0L),
	fScale(1.f),
	fAlpha(1.f),
	vSpeed(0.f,1.f),
	fSpeed(1.f),
	vDirect(0.f,1.f),
	pAlphaTex(NULL),
	pFlowTex(NULL)
{
	strAlphaTex ="";
	strFlowTex ="";
}

TEXEFF_FLOWUV_PROPERTY::~TEXEFF_FLOWUV_PROPERTY()
{
	TextureManager::ReleaseTexture( strAlphaTex.c_str(), pAlphaTex );
	TextureManager::ReleaseTexture( strFlowTex.c_str(), pFlowTex );
}

void TEXEFF_FLOWUV_PROPERTY::Save( CSerialFile& SFile )
{
	SFile << dwFlag;
	SFile << fScale;
	SFile << fAlpha;
	SFile << vSpeed.x;
	SFile << vSpeed.y;
	SFile << fSpeed;
	SFile << vDirect.x;
	SFile << vDirect.y;
	SFile << strAlphaTex;
	SFile << strFlowTex;
}

void TEXEFF_FLOWUV_PROPERTY::Load( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile )
{
	TextureManager::ReleaseTexture( strAlphaTex.c_str(), pAlphaTex );
	TextureManager::ReleaseTexture( strFlowTex.c_str(), pFlowTex );

	SFile >> dwFlag;
	SFile >> fScale;
	SFile >> fAlpha;
	SFile >> vSpeed.x;
	SFile >> vSpeed.y;
	SFile >> fSpeed;
	SFile >> vDirect.x;
	SFile >> vDirect.y;
	SFile >> strAlphaTex;
	SFile >> strFlowTex;

	TextureManager::LoadTexture( strAlphaTex.c_str(), pd3dDevice, pAlphaTex, 0 , 0 );
	TextureManager::LoadTexture( strFlowTex.c_str(), pd3dDevice, pFlowTex, 0 , 0 );
}

// Note : DxTexEffFlowUV
LPDIRECT3DSTATEBLOCK9	DxTexEffFlowUV::m_pSavedSB = NULL;
LPDIRECT3DSTATEBLOCK9	DxTexEffFlowUV::m_pDrawSB = NULL;

DxTexEffFlowUV::DxTexEffFlowUV() :
	m_bUp(TRUE),
	m_vLastSpeed(0.f,0.f),
	m_fTime(-D3DX_PI),
	m_fRandSpeed(1.f),
	m_dwColor(0L)
{
}

DxTexEffFlowUV::~DxTexEffFlowUV()
{
}

void DxTexEffFlowUV::SetProperty( LPDIRECT3DDEVICEQ pd3dDevice, PBYTE &pProp )
{
	TextureManager::ReleaseTexture( m_sProp.strAlphaTex.c_str(), m_sProp.pAlphaTex );
	TextureManager::ReleaseTexture( m_sProp.strFlowTex.c_str(), m_sProp.pFlowTex );

	TEXEFF_FLOWUV_PROPERTY* pTemp = (TEXEFF_FLOWUV_PROPERTY*)pProp;
	m_sProp.dwFlag		= pTemp->dwFlag;
	m_sProp.fScale		= pTemp->fScale;
	m_sProp.fAlpha		= pTemp->fAlpha;
	m_sProp.vSpeed		= pTemp->vSpeed;
	m_sProp.strAlphaTex	= pTemp->strAlphaTex.c_str();
	m_sProp.strFlowTex	= pTemp->strFlowTex.c_str();

	TextureManager::LoadTexture( m_sProp.strAlphaTex.c_str(), pd3dDevice, m_sProp.pAlphaTex, 0 , 0 );
	TextureManager::LoadTexture( m_sProp.strFlowTex.c_str(), pd3dDevice, m_sProp.pFlowTex, 0 , 0 );

	// Note : 변환..
	m_sProp.fSpeed		= sqrtf( (m_sProp.vSpeed.x*m_sProp.vSpeed.x) + (m_sProp.vSpeed.y*m_sProp.vSpeed.y) );
	if( m_sProp.fSpeed > 0.f )
	{
		m_sProp.vDirect.x	= m_sProp.vSpeed.x / m_sProp.fSpeed;
		m_sProp.vDirect.y	= m_sProp.vSpeed.y / m_sProp.fSpeed;
	}
}

void DxTexEffFlowUV::OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	D3DCAPSQ d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );

	// Anisotropic Filter 확인
	DWORD dwMinFilter;
	if( d3dCaps.TextureFilterCaps & D3DPTFILTERCAPS_MINFANISOTROPIC )	dwMinFilter = D3DTEXF_ANISOTROPIC;
	else if( d3dCaps.TextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR )	dwMinFilter = D3DTEXF_LINEAR;
	else																dwMinFilter = D3DTEXF_POINT;

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		if( dwMinFilter == D3DTEXF_ANISOTROPIC )
		{
			pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, dwMinFilter );
			pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );

			pd3dDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, dwMinFilter );
			pd3dDevice->SetSamplerState( 1, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
		}

		// Note : SetRenderState()	 변해도 상관없음.
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_ONE );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_ONE );

		// Note : SetRenderState()
		float fBias = -0.0001f;
		pd3dDevice->SetRenderState( D3DRS_DEPTHBIAS,	*((DWORD*)&fBias) );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,			0x00000000 );

		// Note : SetTextureStageState()
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1 );
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,	D3DTOP_MODULATE );

		if( which==0 )	pd3dDevice->EndStateBlock ( &m_pSavedSB );
		else			pd3dDevice->EndStateBlock ( &m_pDrawSB );
	}
}

void DxTexEffFlowUV::OnReleaseDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSavedSB );
	SAFE_RELEASE( m_pDrawSB );
}

//void DxTextureEffMan::RenderMultiTex( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMesh, const DWORD dwAttrib, TEXEFF_PROPERTY* pProp )
//{
//	// Note : GetTexture
//	if( !pProp->pMultiBaseTex )
//	{
//		TextureManager::GetTexture( pProp->strMultiBaseTex.c_str(), pProp->pMultiBaseTex );
//		if( !pProp->pMultiBaseTex )	return;
//	}
//	if( !pProp->pMultiFlowTex )
//	{
//		TextureManager::GetTexture( pProp->strMultiFlowTex.c_str(), pProp->pMultiFlowTex );
//		if( !pProp->pMultiFlowTex )	return;
//	}
//
//	// Note : 텍스쳐를 흘린다.
//	if( pProp->bfMultiTexNEWUV )
//	{
//		FrameMoveMultiTex( pProp->fMultiTexSpeed, pProp->fMultiTexUVScale, pProp->fMultiTexAddUV, pMesh );
//	}
//	else
//	{
//		FrameMoveMultiTex( pProp->fMultiTexSpeed, pMesh );
//	}
//
//	// Note : Setting
//	pd3dDevice->SetTexture( 0, pProp->pMultiBaseTex );
//	pd3dDevice->SetTexture( 1, pProp->pMultiFlowTex  );
//
//	
//	m_pMultiTexSavedSB->Capture();
//	m_pMultiTexDrawSB->Apply();
//
//	// Note : Specular Render
//	pMesh->DrawSubset( dwAttrib );
//
//	m_dwMultiTexSavedSB->Apply();
//
//	// Note : SAFE
//	pd3dDevice->SetTexture( 1, NULL );
//}
//
//void DxTextureEffMan::FrameMoveMultiTex( float fBaseSpeed, LPD3DXMESH pMesh )
//{
//	float fSpeed = fBaseSpeed * 0.01f;
//
//	struct VERTEXTEX2 { D3DXVECTOR3 vPos, vNor; D3DXVECTOR2 vTex1, vTex2; };
//
//	VERTEXTEX2* pVert;
//	pMesh->LockVertexBuffer( 0L, (VOID**)&pVert );
//	for( DWORD i=0; i<pMesh->GetNumVertices(); ++i )
//	{
//		pVert[i].vTex2 += D3DXVECTOR2( fSpeed, fSpeed );
//	}
//	pMesh->UnlockVertexBuffer();
//}

void DxTexEffFlowUV::FrameMove( const float fElapsedTime )
{
	if( m_sProp.dwFlag&TEXEFFFLOWUV_NEWUV )	{}

	if( m_sProp.dwFlag&TEXEFFFLOWUV_RANDOMSPEED )
	{
		// Note : fTime을 더해준다.
		m_fTime += m_fRandSpeed * fElapsedTime;

		// Note : Random Speed Setting
		if( m_fTime > 0.f && m_bUp )		
		{
			m_bUp = FALSE;
			m_fRandSpeed = (RANDOM_POS*5.f+1.f);
		}
		else if( m_fTime>D3DX_PI && !m_bUp )
		{
			m_fTime -= D3DX_PI*2.f;	// 값을 빼준다.
			m_bUp = TRUE;
			m_fRandSpeed = (RANDOM_POS*5.f+1.f);
		}

		// Note : 오르락 내리락
		float fSpeed = cosf( m_fTime );
		fSpeed += 2.f;
		fSpeed *= 0.33f;

		m_vLastSpeed += m_sProp.vDirect * (fSpeed*m_sProp.fSpeed) * fElapsedTime;
	}
	else
	{
		m_vLastSpeed += m_sProp.vSpeed * fElapsedTime;
	}
}

void DxTexEffFlowUV::FrameMoveMultiTex( LPDIRECT3DVERTEXBUFFERQ pVB, DWORD dwVert, VERTEX* pVertSrc )
{
	struct VERTEXTEX2 { D3DXVECTOR3 vPos; D3DXVECTOR2 vTex1, vTex2; float fTex3; };
	VERTEXTEX2 sBackUp;
	VERTEXTEX2* pVert;
	HRESULT hr = pVB->Lock( 0, 0, (VOID**)&pVert, D3DLOCK_DISCARD );
	if( FAILED(hr) )	return;
	for( DWORD i=0; i<dwVert; ++i )
	{
		sBackUp.vPos = pVertSrc[i].vPos;
		sBackUp.vTex1 = pVertSrc[i].vTex;
		sBackUp.vTex2 = pVertSrc[i].vTex + m_vLastSpeed;
		sBackUp.fTex3 = 1.f;

		pVert[i] = sBackUp;
	}
	pVB->Unlock();
}

void DxTexEffFlowUV::Render( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DVERTEXBUFFERQ pVB, LPDIRECT3DINDEXBUFFERQ pIB, 
							LPD3DXATTRIBUTERANGE pAttrib, const DWORD dwAttrib, VERTEX* pVertSrc, DWORD dwVert )
{
	FrameMoveMultiTex( pVB, dwVert, pVertSrc );

	SetRenderBegin( pd3dDevice );

	pd3dDevice->SetStreamSource( 0, pVB, 0, sizeof(VERTEX) );
	pd3dDevice->SetIndices( pIB );

	pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,
									0,
									pAttrib[dwAttrib].VertexStart,
									pAttrib[dwAttrib].VertexCount,
									pAttrib[dwAttrib].FaceStart*3,
									pAttrib[dwAttrib].FaceCount );

	SetRenderEnd( pd3dDevice );
}

void DxTexEffFlowUV::Render( LPDIRECT3DDEVICEQ pd3dDevice, const DWORD dwVert, const DWORD dwFaces, VERTEXNORCOLORTEX* pVertSrc )
{
	// Note : Lock을 하기 위한 D3DLOCK_DISCARD or D3DLOCK_NOOVERWRITE
	DWORD dwFlag = D3DLOCK_NOOVERWRITE; 
	DWORD dwVertexSizeFULL = dwVert*sizeof(VERTEX);
	if( dwVertexSizeFULL > DxDynamicVB::m_sVB_PNT.nFullByte )
	{
		CDebugSet::ToLogFile( "DxTexEffFlowUV::Render() -- dwVertexSizeFULL > DxDynamicVB::m_sVB_PNT.nFullByte" );
		return;	// 사이즈가 너무 크다.
	}
	if( DxDynamicVB::m_sVB_PNT.nOffsetToLock + dwVertexSizeFULL > DxDynamicVB::m_sVB_PNT.nFullByte )
	{
		dwFlag = D3DLOCK_DISCARD; 
		DxDynamicVB::m_sVB_PNT.nVertexCount = 0;
		DxDynamicVB::m_sVB_PNT.nOffsetToLock = 0; 
	}

	struct VERTEXTEX2 { D3DXVECTOR3 vPos; D3DXVECTOR2 vTex1, vTex2; float fTex3; };
	VERTEXTEX2 sBackUp;
	VERTEXTEX2 *pVertices;
	DxDynamicVB::m_sVB_PNT.pVB->Lock( DxDynamicVB::m_sVB_PNT.nOffsetToLock, dwVertexSizeFULL, (VOID**)&pVertices, dwFlag );
	for( DWORD i=0; i<dwVert; ++i )
	{
		sBackUp.vPos = pVertSrc[i].vPos;
		sBackUp.vTex1 = pVertSrc[i].vTex;
		sBackUp.vTex2 = pVertSrc[i].vTex + m_vLastSpeed;
		sBackUp.fTex3 = 1.f;

		pVertices[i] = sBackUp;
	}
	DxDynamicVB::m_sVB_PNT.pVB->Unlock ();

	//pd3dDevice->SetFVF ( VERTEXCOLORTEX::FVF );	// SetRenderBegin에서 부른다.
	pd3dDevice->SetStreamSource ( 0, DxDynamicVB::m_sVB_PNT.pVB, 0, sizeof(VERTEX) );
	//pd3dDevice->SetIndices ( m_pIBuffer );		// 위쪽에서 IB를 부르게 한다.

	pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, DxDynamicVB::m_sVB_PNT.nVertexCount, 0, dwVert, 0, dwFaces );

	// Note : 값을 더해줌. 
	DxDynamicVB::m_sVB_PNT.nVertexCount += dwVert;
	DxDynamicVB::m_sVB_PNT.nOffsetToLock += dwVertexSizeFULL;
}

void DxTexEffFlowUV::SetRenderBegin( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Note : GetTexture
	if( !m_sProp.pAlphaTex )
	{
		TextureManager::GetTexture( m_sProp.strAlphaTex.c_str(), m_sProp.pAlphaTex );
	}
	if( !m_sProp.pFlowTex )
	{
		TextureManager::GetTexture( m_sProp.strFlowTex.c_str(), m_sProp.pFlowTex );
	}
	pd3dDevice->SetTexture( 0, m_sProp.pAlphaTex );
	pd3dDevice->SetTexture( 1, m_sProp.pFlowTex );
	
	m_pSavedSB->Capture();
	m_pDrawSB->Apply();

	pd3dDevice->SetFVF( D3DFVF_XYZ|D3DFVF_TEX3|D3DFVF_TEXCOORDSIZE1(2) );

	if( m_sProp.dwFlag&TEXEFFFLOWUV_MODULATE2X )
	{
		pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,	D3DTOP_MODULATE2X );
	}
}

void DxTexEffFlowUV::SetRenderEnd( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pSavedSB->Apply();
	pd3dDevice->SetTexture( 1, NULL );
}

void DxTexEffFlowUV::SavePSF( CSerialFile& SFile )
{
	SFile << VERSION;

	SFile.BeginBlock( EMBLOCK_02 );
	m_sProp.Save( SFile );
	SFile.EndBlock( EMBLOCK_02 );
}

void DxTexEffFlowUV::LoadPSF( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile )
{
	DWORD dwSize;
	DWORD dwVer;
	SFile >> dwVer;
	SFile >> dwSize;

	if( dwVer==VERSION )
	{
		m_sProp.Load( pd3dDevice, SFile );
	}
	else
	{
		DWORD dwCurBuffer = SFile.GetfTell();
		SFile.SetOffSet( dwCurBuffer+dwSize );
	}
}

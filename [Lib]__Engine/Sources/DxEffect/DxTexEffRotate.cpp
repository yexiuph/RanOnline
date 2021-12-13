#include "pch.h"

#include "./SerialFile.h"
#include "./DxDynamicVB.h"
#include "./TextureManager.h"

#include "./DxTexEffRotate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const DWORD DxTexEffRotate::TYPEID = DEF_TEXEFF_ROTATE;
const DWORD	DxTexEffRotate::VERSION = 0x00000100;
const char DxTexEffRotate::NAME[] = "[ Rotate Tex ]";

// Note : TEXEFF_ROTATE_PROPERTY
TEXEFF_ROTATE_PROPERTY::TEXEFF_ROTATE_PROPERTY() :
	fSpeed(1.f),
	pTex(NULL)
{
	strTex ="";
}

TEXEFF_ROTATE_PROPERTY::~TEXEFF_ROTATE_PROPERTY()
{
	TextureManager::ReleaseTexture( strTex.c_str(), pTex );
}

void TEXEFF_ROTATE_PROPERTY::Save( CSerialFile& SFile )
{
	SFile << fSpeed;
	SFile << strTex;
}

void TEXEFF_ROTATE_PROPERTY::Load( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile )
{
	TextureManager::ReleaseTexture( strTex.c_str(), pTex );

	SFile >> fSpeed;
	SFile >> strTex;

	TextureManager::LoadTexture( strTex.c_str(), pd3dDevice, pTex, 0 , 0 );
}

// Note : DxTexEffRotate
LPDIRECT3DSTATEBLOCK9	DxTexEffRotate::m_pSavedSB = NULL;
LPDIRECT3DSTATEBLOCK9	DxTexEffRotate::m_pDrawSB = NULL;

DxTexEffRotate::DxTexEffRotate() :
	m_fTime(0.f),
	m_fSin_Value(0.f),
	m_fCos_Value(0.f),
	m_fRotate_ValueEND(0.f)
{
}

DxTexEffRotate::~DxTexEffRotate()
{
}

void DxTexEffRotate::SetProperty( LPDIRECT3DDEVICEQ pd3dDevice, PBYTE &pProp )
{
	TextureManager::ReleaseTexture( m_sProp.strTex.c_str(), m_sProp.pTex );

	TEXEFF_ROTATE_PROPERTY* pTemp = (TEXEFF_ROTATE_PROPERTY*)pProp;
	m_sProp.fSpeed		= pTemp->fSpeed;
	m_sProp.strTex		= pTemp->strTex;

	TextureManager::LoadTexture( m_sProp.strTex.c_str(), pd3dDevice, m_sProp.pTex, 0, 0 );
}

void DxTexEffRotate::OnCreateDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	D3DCAPSQ d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		float fBias = -0.0001f;
		pd3dDevice->SetRenderState( D3DRS_DEPTHBIAS,	*((DWORD*)&fBias) );
		pd3dDevice->SetRenderState( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	TRUE );

		pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
		pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );

		if( which==0 )	pd3dDevice->EndStateBlock ( &m_pSavedSB );
		else			pd3dDevice->EndStateBlock ( &m_pDrawSB );
	}
}

void DxTexEffRotate::OnReleaseDevice( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSavedSB );
	SAFE_RELEASE( m_pDrawSB );
}

void DxTexEffRotate::FrameMove( const float fElapsedTime )
{
	m_fRotate_ValueEND += m_sProp.fSpeed * fElapsedTime;

	m_fSin_Value = sinf( m_fRotate_ValueEND );
	m_fCos_Value = cosf( m_fRotate_ValueEND );
}

void DxTexEffRotate::FrameMoveMultiTex( LPDIRECT3DVERTEXBUFFERQ pVB, DWORD dwVert, VERTEX* pVertSrc )
{
	float fUV_X, fUV_Y;
	float fUV_X2, fUV_Y2;
	VERTEX sBackUp;
	VERTEX *pVert;
	pVB->Lock( 0, 0, (VOID**)&pVert, D3DLOCK_DISCARD );
	for( DWORD i=0; i<dwVert; ++i )
	{
		fUV_X = (pVertSrc[i].vTex.x*2.f)-1.f;
		fUV_Y = (pVertSrc[i].vTex.y*2.f)-1.f;

		fUV_X2 = (fUV_X*m_fCos_Value) + (fUV_Y*m_fSin_Value);
		fUV_Y2 = (fUV_Y*m_fCos_Value) - (fUV_X*m_fSin_Value);

		fUV_X2 = (fUV_X2+1.f)*0.5f;
		fUV_Y2 = (fUV_Y2+1.f)*0.5f;

		sBackUp.vPos = pVertSrc[i].vPos;
		sBackUp.vNor = pVertSrc[i].vNor;
		sBackUp.vTex.x = fUV_X2;
		sBackUp.vTex.y = fUV_Y2;

		pVert[i] = sBackUp;
	}
	pVB->Unlock();
}

void DxTexEffRotate::Render( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DVERTEXBUFFERQ pVB, LPDIRECT3DINDEXBUFFERQ pIB, 
							LPD3DXATTRIBUTERANGE pAttrib, const DWORD dwAttrib, VERTEX* pVertSrc, DWORD dwVert )
{
	FrameMoveMultiTex( pVB, dwVert, pVertSrc );

	SetRenderBegin( pd3dDevice );

	pd3dDevice->SetStreamSource( 0, pVB, 0, sizeof(VERTEX) );
	pd3dDevice->SetIndices ( pIB );

	pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 
									0,
									pAttrib[dwAttrib].VertexStart,
									pAttrib[dwAttrib].VertexCount,
									pAttrib[dwAttrib].FaceStart*3,
									pAttrib[dwAttrib].FaceCount );

	SetRenderEnd( pd3dDevice );
}

void DxTexEffRotate::Render( LPDIRECT3DDEVICEQ pd3dDevice, const DWORD dwVert, const DWORD dwFaces, VERTEXNORCOLORTEX* pVertSrc )
{
	// Note : Lock을 하기 위한 D3DLOCK_DISCARD or D3DLOCK_NOOVERWRITE
	DWORD dwFlag = D3DLOCK_NOOVERWRITE; 
	DWORD dwVertexSizeFULL = dwVert*sizeof(VERTEX);
	if( dwVertexSizeFULL > DxDynamicVB::m_sVB_PNT.nFullByte )
	{
		CDebugSet::ToLogFile( "DxTexEffRotate::Render() -- dwVertexSizeFULL > DxDynamicVB::m_sVB_PNT.nFullByte" );
		return;	// 사이즈가 너무 크다.
	}
	if( DxDynamicVB::m_sVB_PNT.nOffsetToLock + dwVertexSizeFULL > DxDynamicVB::m_sVB_PNT.nFullByte )
	{
		dwFlag = D3DLOCK_DISCARD; 
		DxDynamicVB::m_sVB_PNT.nVertexCount = 0;
		DxDynamicVB::m_sVB_PNT.nOffsetToLock = 0; 
	}

	float fUV_X, fUV_Y;
	float fUV_X2, fUV_Y2;
	VERTEX sBackUp;
	VERTEX *pVertices;
	DxDynamicVB::m_sVB_PNT.pVB->Lock( DxDynamicVB::m_sVB_PNT.nOffsetToLock, dwVertexSizeFULL, (VOID**)&pVertices, dwFlag );
	for( DWORD i=0; i<dwVert; ++i )
	{
		fUV_X = (pVertSrc[i].vTex.x*2.f)-1.f;
		fUV_Y = (pVertSrc[i].vTex.y*2.f)-1.f;

		fUV_X2 = (fUV_X*m_fCos_Value) + (fUV_Y*m_fSin_Value);
		fUV_Y2 = (fUV_Y*m_fCos_Value) - (fUV_X*m_fSin_Value);

		fUV_X2 = (fUV_X2+1.f)*0.5f;
		fUV_Y2 = (fUV_Y2+1.f)*0.5f;

		sBackUp.vPos = pVertSrc[i].vPos;
		sBackUp.vNor = pVertSrc[i].vNor;
		sBackUp.vTex.x = fUV_X2;
		sBackUp.vTex.y = fUV_Y2;

		pVertices[i] = sBackUp;
	}
	DxDynamicVB::m_sVB_PNT.pVB->Unlock ();

	pd3dDevice->SetStreamSource ( 0, DxDynamicVB::m_sVB_PNT.pVB, 0, sizeof(VERTEX) );

	pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, DxDynamicVB::m_sVB_PNT.nVertexCount, 0, dwVert, 0, dwFaces );

	// Note : 값을 더해줌. 
	DxDynamicVB::m_sVB_PNT.nVertexCount += dwVert;
	DxDynamicVB::m_sVB_PNT.nOffsetToLock += dwVertexSizeFULL;
}

void DxTexEffRotate::SetRenderBegin( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// Note : GetTexture
	if( !m_sProp.pTex )
	{
		TextureManager::GetTexture( m_sProp.strTex.c_str(), m_sProp.pTex );
	}
	pd3dDevice->SetTexture( 0, m_sProp.pTex );
	
	m_pSavedSB->Capture();
	m_pDrawSB->Apply();

	pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, m_dwColor );

	pd3dDevice->SetFVF( D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1 );
}

void DxTexEffRotate::SetRenderEnd( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pSavedSB->Apply();
	pd3dDevice->SetTexture( 1, NULL );
}

void DxTexEffRotate::SavePSF( CSerialFile& SFile )
{
	SFile << VERSION;

	SFile.BeginBlock( EMBLOCK_02 );
	m_sProp.Save( SFile );
	SFile.EndBlock( EMBLOCK_02 );
}

void DxTexEffRotate::LoadPSF( LPDIRECT3DDEVICEQ pd3dDevice, CSerialFile& SFile )
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

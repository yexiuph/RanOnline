// Terrain0.cpp: implementation of the DxEffectRiver class.
//
//	UPDATE [03.02.20] : 새롭게 물 효과를 바꿈
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "./TextureManager.h"
#include "./DxFrameMesh.h"
#include "./DxEffectDefine.h"

#include "./DxEffectMan.h"
#include "./DxShadowMap.h"
#include "./DxSurfaceTex.h"
#include "./DxEnvironment.h"
#include "./DxCubeMap.h"
#include "./DxLandMan.h"

#include "./DxViewPort.h"

#include "./SerialFile.h"
#include "./Collision.h"

#include "./DxEffectRiver.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//	Note : TnL 용으로 범프를 사용 못할때 사용한다.  ( 기본적인 일렁이는 모습 표현 )
//
HRESULT DxEffectRiver::RenderTnL ( LPDIRECT3DDEVICEQ pd3dDevice, DxLandMan* pLandMan )
{
	pd3dDevice->SetTexture( 0, m_pddsTexFlash );
	pd3dDevice->SetTexture( 1, m_pddsTexFlash );

	m_pSB_TnL_SAVE->Capture();
	m_pSB_TnL->Apply();

	if ( !(m_dwFlag&USEREFLECT) && (m_dwFlag&USESEE) )
	{
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	FALSE );
	}

	pd3dDevice->SetFVF ( D3DWATERVERTEX::FVF );
	{
		DWORD dwColor = ((DWORD)(0xff)<<24) + ((DWORD)(m_vColor.x)<<16) + ((DWORD)(m_vColor.y)<<8) + (DWORD)(m_vColor.z);

		float fTime			= m_fTime*DEFAULT_SPEED;
		float fElapsedTime	= m_fElapsedTime*DEFAULT_SPEED;

		m_vAddTex1 += D3DXVECTOR2 ( sinf(fTime)*fElapsedTime*m_fVel, cosf(fTime)*fElapsedTime*m_fVel );
		m_vAddTex2 += D3DXVECTOR2 ( sinf(fTime+(D3DX_PI*0.5f))*fElapsedTime*m_fVel, cosf(fTime+(D3DX_PI*0.5f))*fElapsedTime*m_fVel );

		float fScale = m_fScale * DEFAULT_SCALE;
		CLIPVOLUME sCV = DxViewPort::GetInstance().GetClipVolume ();

		if ( pLandMan )	
		{
			m_sWaterAABB.DynamicLoad ( pd3dDevice, pLandMan->GetSerialFile(), sCV );
			m_sWaterAABB.Render ( pd3dDevice, sCV, TRUE, dwColor, fScale, m_vAddTex1, m_vAddTex2 );
		}
		else
		{
			m_sWaterAABB.Render ( pd3dDevice, sCV, FALSE, dwColor, fScale, m_vAddTex1, m_vAddTex2 );
		}
	}

	m_pSB_TnL_SAVE->Apply();

	pd3dDevice->SetTexture ( 1, NULL );	

	return S_OK;
}

HRESULT DxEffectRiver::RenderDark ( LPDIRECT3DDEVICEQ pd3dDevice, DxLandMan* pLandMan )
{
	UpdateWaveTex ( pd3dDevice );

	pd3dDevice->SetTexture( 0, DxSurfaceTex::GetInstance().m_pTempTex256 );

	m_pSB_Dark_SAVE->Capture();
	m_pSB_Dark->Apply();

	pd3dDevice->SetFVF ( D3DWATERVERTEX::FVF );
	{
		DWORD dwColor = ((DWORD)(0xff)<<24) + ((DWORD)(m_vColor.x)<<16) + ((DWORD)(m_vColor.y)<<8) + (DWORD)(m_vColor.z);

		float fTime			= m_fTime*DEFAULT_SPEED;
		float fElapsedTime	= m_fElapsedTime*DEFAULT_SPEED;

		m_vAddTex_Dark += D3DXVECTOR2 ( fElapsedTime*m_vDarkVel.x, fElapsedTime*m_vDarkVel.y );
		D3DXVECTOR2 vTemp = D3DXVECTOR2 ( 0.f, 0.f );

		float fScale = m_fDarkScale * DEFAULT_SCALE;
		CLIPVOLUME sCV = DxViewPort::GetInstance().GetClipVolume ();
		if ( pLandMan )	
		{
			m_sWaterAABB.DynamicLoad ( pd3dDevice, pLandMan->GetSerialFile(), sCV );
			m_sWaterAABB.Render ( pd3dDevice, sCV, TRUE, dwColor, fScale, m_vAddTex_Dark, vTemp );
		}
		else
		{
			m_sWaterAABB.Render ( pd3dDevice, sCV, FALSE, dwColor, fScale, m_vAddTex_Dark, vTemp );
		}
	}

	m_pSB_Dark_SAVE->Apply();

	pd3dDevice->SetTexture ( 1, NULL );	
	return S_OK;
}

HRESULT DxEffectRiver::UpdateWaveTex ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	LPDIRECT3DSURFACEQ	pSrcSurface, pSrcZBuffer;
	pd3dDevice->GetRenderTarget ( 0, &pSrcSurface );
	pd3dDevice->GetDepthStencilSurface ( &pSrcZBuffer );


	pd3dDevice->SetRenderTarget ( 0, DxSurfaceTex::GetInstance().m_pTempSuf256 );
	pd3dDevice->SetDepthStencilSurface ( NULL );
	pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(255,255,255), 1.0f, 0L);

	D3DWAVETEX*	pVertices;

	int		nCount;
	float	fRateX;
	float	fRateY;
	DWORD	dwWidth	= 256;
	DWORD	dwHeight = 256;

	m_pWaveTexVB->Lock ( 0, 0, (VOID**)&pVertices, 0L );

	for ( DWORD i=0; i<5; i++ )
	{
		for ( DWORD j=0; j<5; j++ )
		{
			nCount = (i*5)+j;
			fRateX = (float)j/4;
			fRateY = (float)i/4;
			pVertices[nCount].vPos = D3DXVECTOR4 ( (float)(dwWidth*fRateX), (float)(dwHeight*fRateY), 1.f, 1.f );
			pVertices[nCount].vTex = D3DXVECTOR2 ( fRateX+sinf(m_fTime+i)*0.05f, fRateY+sinf(m_fTime+j)*0.05f );
		}
	}

	m_pWaveTexVB->Unlock ();

	pd3dDevice->SetTexture ( 0, m_pddsTexDark );

	m_pSB_WaveTex_SAVE->Capture();
	m_pSB_WaveTex->Apply();

	//	Draw 1
	{
		pd3dDevice->SetFVF ( D3DWAVETEX::FVF );
		pd3dDevice->SetStreamSource ( 0, m_pWaveTexVB, 0, sizeof(D3DWAVETEX) );
		pd3dDevice->SetIndices ( m_pWaveTexIB );

		pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0, 0, 25, 0, 32 );
	}

	m_pSB_WaveTex_SAVE->Apply();

	pd3dDevice->SetRenderTarget ( 0, pSrcSurface );
	pd3dDevice->SetDepthStencilSurface ( pSrcZBuffer );
	SAFE_RELEASE ( pSrcSurface );
	SAFE_RELEASE ( pSrcZBuffer );

	return S_OK;
}

HRESULT DxEffectRiver::RenderReflection ( LPDIRECT3DDEVICEQ pd3dDevice, DxLandMan* pLandMan )
{
	if ( !DxSurfaceTex::GetInstance().m_pReflectTex )	return S_OK;

	D3DXMATRIX	matReflect, matProj;
	pd3dDevice->GetTransform ( D3DTS_PROJECTION,	&matProj );
	D3DXMatrixMultiply ( &matReflect, &DxEnvironment::GetInstance().GetMatrix_ReflectView(),	&matProj );
	
	D3DXMATRIX matTex;
	D3DXMatrixTexScaleBias ( matTex );
	D3DXMatrixMultiply ( &matReflect, &matReflect, &matTex );				// 반사





	pd3dDevice->SetTexture( 0, DxEffectMan::GetInstance().GetBumpTexture() );
	pd3dDevice->SetTexture( 1, DxSurfaceTex::GetInstance().m_pReflectTex );

	//	Note : 반사
	m_pSB_Reflect_SAVE->Capture();
	m_pSB_Reflect->Apply();

	if ( m_dwFlag&USESEE )	
	{
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE, FALSE );
	}

	D3DXMATRIX matVP;
	D3DXMatrixMultiply ( &matVP, &DxEnvironment::GetInstance().GetMatrix_ReflectView(), &matProj );

	DxSetTextureMatrix ( pd3dDevice, 1, matVP );

	pd3dDevice->SetTextureStageState ( 0, D3DTSS_BUMPENVMAT10, FtoDW(m_fBumpMat) );
	pd3dDevice->SetTextureStageState ( 0, D3DTSS_BUMPENVMAT11, FtoDW(m_fBumpMat) );

	pd3dDevice->SetFVF ( D3DWATERVERTEX::FVF );
	{
		DWORD dwColor = ((DWORD)(0x90)<<24) + ((DWORD)(0xff*m_fBumpAlpha)<<16) + ((DWORD)(0xff*m_fBumpAlpha)<<8) + (DWORD)(0xff*m_fBumpAlpha);
		D3DXVECTOR2 vTemp = D3DXVECTOR2 ( 0.f, 0.5f*m_fTime );
		float fScale = m_fScale * DEFAULT_SCALE;

		CLIPVOLUME sCV = DxViewPort::GetInstance().GetClipVolume ();
		if ( pLandMan )	
		{
			m_sWaterAABB.DynamicLoad ( pd3dDevice, pLandMan->GetSerialFile(), sCV );
			m_sWaterAABB.Render ( pd3dDevice, sCV, TRUE, dwColor, fScale, vTemp );
		}
		else
		{
			m_sWaterAABB.Render ( pd3dDevice, sCV, FALSE, dwColor, fScale, vTemp );
		}
	}

	DxResetTextureMatrix ( pd3dDevice, 1 );

	m_pSB_Reflect_SAVE->Apply();

	pd3dDevice->SetTexture( 1, NULL );

	return S_OK;
}

HRESULT DxEffectRiver::RenderReflectionEX ( LPDIRECT3DDEVICEQ pd3dDevice, DxLandMan* pLandMan )
{
	if ( !DxSurfaceTex::GetInstance().m_pReflectTex )	return S_OK;

	pd3dDevice->SetTexture( 0, DxEffectMan::GetInstance().GetBumpTexture() );
	pd3dDevice->SetTexture( 1, DxSurfaceTex::GetInstance().m_pReflectTex );

	//	Note : 반사
	m_pSB_Reflect_SAVE->Capture();
	m_pSB_Reflect->Apply();

	if ( m_dwFlag&USESEE )	
	{
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE, FALSE );
	}

	D3DXMATRIX matTex, matVP, matView, matProj;
	pd3dDevice->GetTransform ( D3DTS_VIEW,			&matView );
	pd3dDevice->GetTransform ( D3DTS_PROJECTION,	&matProj );
	D3DXMatrixMultiply ( &matVP, &matView, &matProj );

	DxSetTextureMatrix ( pd3dDevice, 1, matVP );

	pd3dDevice->SetTextureStageState ( 0, D3DTSS_BUMPENVMAT10, FtoDW(m_fBumpMat) );
	pd3dDevice->SetTextureStageState ( 0, D3DTSS_BUMPENVMAT11, FtoDW(m_fBumpMat) );

	pd3dDevice->SetFVF ( D3DWATERVERTEX::FVF );
	{
		DWORD dwColor = ((DWORD)(0x90)<<24) + ((DWORD)(0xff*m_fBumpAlpha)<<16) + ((DWORD)(0xff*m_fBumpAlpha)<<8) + (DWORD)(0xff*m_fBumpAlpha);
		D3DXVECTOR2 vTemp = D3DXVECTOR2 ( 0.f, 0.5f*m_fTime );
		float fScale = m_fScale * DEFAULT_SCALE;

		CLIPVOLUME sCV = DxViewPort::GetInstance().GetClipVolume ();
		if ( pLandMan )	
		{
			m_sWaterAABB.DynamicLoad ( pd3dDevice, pLandMan->GetSerialFile(), sCV );
			m_sWaterAABB.Render ( pd3dDevice, sCV, TRUE, dwColor, fScale, vTemp );
		}
		else
		{
			m_sWaterAABB.Render ( pd3dDevice, sCV, FALSE, dwColor, fScale, vTemp );
		}
	}
	DxResetTextureMatrix ( pd3dDevice, 1 );

	m_pSB_Reflect_SAVE->Apply();

	pd3dDevice->SetTexture( 1, NULL );

	return S_OK;
}

HRESULT DxEffectRiver::Render_VS_PS ( LPDIRECT3DDEVICEQ pd3dDevice, DxLandMan* pLandMan )
{
	D3DXMATRIX matW, matVP, matWVP, matWorld, matView, matProj;

	D3DXMatrixIdentity ( &matWorld );	

	pd3dDevice->GetTransform ( D3DTS_VIEW,		&matView );
	pd3dDevice->GetTransform ( D3DTS_PROJECTION,	&matProj );

	D3DXMatrixTranspose ( &matW, &matWorld );
	pd3dDevice->SetVertexShaderConstantF ( VSC_MATWVP_01, (float*)&matW, 4 );	

	D3DXMatrixMultiply ( &matWVP, &matWorld, &matView );
	D3DXMatrixMultiply ( &matWVP, &matWVP, &matProj );
	D3DXMatrixTranspose ( &matWVP, &matWVP );

	pd3dDevice->SetVertexShaderConstantF ( VSC_MATWVP_02, (float*) &matWVP, 4 );

	D3DXMatrixMultiply ( &matVP, &DxEnvironment::GetInstance().GetMatrix_ReflectView(), &DxViewPort::GetInstance().GetMatProj () );
	D3DXMATRIX matTex, InvV;
    D3DXMatrixInverse ( &InvV, NULL, &matView ); // gives us world coords

    D3DXMATRIX matTexScale;

    SecureZeroMemory( &matTexScale, sizeof( D3DMATRIX ) );
    matTexScale._11 = 0.5f;
    matTexScale._22 = -0.5f;
    matTexScale._33 = 0.0f; 
    matTexScale._41 = 0.5f; 
    matTexScale._42 = 0.5f;
    matTexScale._43 = 1.0f; 
    matTexScale._44 = 1.0f;

    D3DXMATRIX mat, mat2;
    D3DXMatrixMultiply ( &mat2, &matVP, &matTexScale ); 
    D3DXMatrixMultiply ( &matTex, &InvV, &mat2);

	D3DXMatrixTranspose ( &matTex, &matTex );

	pd3dDevice->SetVertexShaderConstantF ( 28+1, (float*)&matTex, 4 );

	D3DXVECTOR3 vAmbient, vDirect, vHighLightColor;
	DXLIGHT &Light = *DxLightMan::GetInstance()->GetDirectLight ();
	vAmbient.x = Light.m_Light.Ambient.r;
	vAmbient.y = Light.m_Light.Ambient.g;
	vAmbient.z = Light.m_Light.Ambient.b;
	vDirect.x = -Light.m_Light.Direction.x;
	vDirect.y = -Light.m_Light.Direction.z;		// 바꾸다.
	vDirect.z = -Light.m_Light.Direction.y;

	D3DXVec3Normalize ( &vDirect, &vDirect );

	vHighLightColor = D3DXVECTOR3 ( 0.8f, 0.76f, 0.62f );

	pd3dDevice->SetPixelShaderConstantF ( 0, (float*)&vAmbient, 1 );
	pd3dDevice->SetPixelShaderConstantF ( 1, (float*)&vDirect, 1 );			///rgb 돌리수 있음 좋겠다.
	pd3dDevice->SetPixelShaderConstantF ( 2, (float*)&vHighLightColor, 1 );

	pd3dDevice->SetTexture( 0, m_pNormalMap );
	pd3dDevice->SetTexture( 1, m_pNormalMap );
	pd3dDevice->SetTexture( 2, DxCubeMap::GetInstance().GetCubeTexture() );	
	pd3dDevice->SetTexture( 3, m_p1DMap );

	m_pSB_VSPS_SAVE->Capture();
	m_pSB_VSPS->Apply();

	//pd3dDevice->SetFVF( NULL );
	//pd3dDevice->SetVertexShader( m_pOceanWaterVS );
	//pd3dDevice->SetVertexDeclaration( m_pRiverDCRT );
	//pd3dDevice->SetPixelShader ( m_pOceanWaterPS );
	//pd3dDevice->SetIndices ( m_pStaticIB );
	//pd3dDevice->SetStreamSource ( 0, m_pWaterVB, 0, sizeof(D3DWATERVERTEX) );

	//pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0, 0, m_dwVertices, 0, m_dwRenderFaces );

	pd3dDevice->SetVertexShader( NULL );
	pd3dDevice->SetVertexDeclaration( NULL );
	pd3dDevice->SetPixelShader( NULL );

	m_pSB_VSPS_SAVE->Apply();

	pd3dDevice->SetTexture( 1, NULL );
	pd3dDevice->SetTexture( 2, NULL );
	pd3dDevice->SetTexture( 3, NULL );

	return S_OK;
}

HRESULT DxEffectRiver::Render_VS_PS_2 ( LPDIRECT3DDEVICEQ pd3dDevice, DxLandMan* pLandMan )
{
	D3DXMATRIX matW, matVP, matWVP, matView, matProj;

	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity ( &matIdentity );

	pd3dDevice->GetTransform ( D3DTS_VIEW,			&matView );
	pd3dDevice->GetTransform ( D3DTS_PROJECTION,	&matProj );

	pd3dDevice->SetVertexShaderConstantF ( VSC_MATWVP_01, (float*)&matIdentity, 4 );

	D3DXMatrixMultiply ( &matWVP, &matView, &matProj );
	D3DXMatrixTranspose ( &matWVP, &matWVP );
	pd3dDevice->SetVertexShaderConstantF ( VSC_MATWVP_02, (float*)&matWVP, 4 );

	D3DXMATRIX matTex, matTexScale;

	D3DXMatrixIdentity ( &matTexScale );
    matTexScale._11 = 0.5f;
    matTexScale._22 = -0.5f;
    matTexScale._33 = 0.0f;
    matTexScale._41 = 0.5f;
    matTexScale._42 = 0.5f;
    matTexScale._43 = 1.0f;
    matTexScale._44 = 1.0f;

	if ( DxEnvironment::GetInstance().IsRecentReflect() )	matWVP = matView;
	else												matWVP = DxEnvironment::GetInstance().GetMatrix_ReflectView();

	D3DXMatrixMultiply ( &matWVP, &matWVP, &matProj );
	D3DXMatrixMultiply ( &matTex, &matWVP, &matTexScale ); 
	D3DXMatrixTranspose ( &matTex, &matTex );

	pd3dDevice->SetVertexShaderConstantF ( 29, (float*)&matTex, 4 );

	// PixelShaderConstant 설정
	D3DXVECTOR3 vBase, vReflect, vRefract, vBaseEX;
	vBase = D3DXVECTOR3 ( 0.f, 0.3f, 0.5f );
	vReflect = D3DXVECTOR3 ( 0.01f, 0.01f, 0.01f );
	vRefract = D3DXVECTOR3 ( 0.01f, 0.01f, 0.01f );
	vBaseEX = D3DXVECTOR3 ( 1.f, 3.f, 5.f );
	pd3dDevice->SetPixelShaderConstantF ( 0, (float*)&vBase, 1 );
	pd3dDevice->SetPixelShaderConstantF ( 1, (float*)&vReflect, 1 );
	pd3dDevice->SetPixelShaderConstantF ( 2, (float*)&vRefract, 1 );
	pd3dDevice->SetPixelShaderConstantF ( 3, (float*)&vBaseEX, 1 );

	pd3dDevice->SetTexture( 0, m_pNormalMap );
	pd3dDevice->SetTexture( 1, m_pNormalMap );
	pd3dDevice->SetTexture( 2, DxSurfaceTex::GetInstance().m_pReflectTex );

	m_pSB_VSPS_SAVE->Capture();
	m_pSB_VSPS->Apply();

	if ( m_dwFlag&USESEE )	pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE, FALSE );

	pd3dDevice->SetFVF( NULL );
	pd3dDevice->SetVertexShader( m_pRiverVS );
	pd3dDevice->SetVertexDeclaration( m_pRiverDCRT );
	pd3dDevice->SetPixelShader( m_pRiverPS );
	{
		DWORD dwColor = ((DWORD)(0xff)<<24) + ((DWORD)(m_vColor.x)<<16) + ((DWORD)(m_vColor.y)<<8) + (DWORD)(m_vColor.z);

		float fTime			= m_fTime*DEFAULT_SPEED;
		float fElapsedTime	= m_fElapsedTime*DEFAULT_SPEED;

		m_vAddTex1 += D3DXVECTOR2 ( sinf(fTime)*fElapsedTime*m_fVel, cosf(fTime)*fElapsedTime*m_fVel );
		m_vAddTex2 += D3DXVECTOR2 ( sinf(fTime+(D3DX_PI*0.5f))*fElapsedTime*m_fVel, cosf(fTime+(D3DX_PI*0.5f))*fElapsedTime*m_fVel );

		float fScale = m_fScale * DEFAULT_SCALE;
		CLIPVOLUME sCV = DxViewPort::GetInstance().GetClipVolume ();
		if ( pLandMan )	
		{
			m_sWaterAABB.DynamicLoad ( pd3dDevice, pLandMan->GetSerialFile(), sCV );
			m_sWaterAABB.Render ( pd3dDevice, sCV, TRUE, dwColor, fScale, m_vAddTex1, m_vAddTex2 );
		}
		else
		{
			m_sWaterAABB.Render ( pd3dDevice, sCV, FALSE, dwColor, fScale, m_vAddTex1, m_vAddTex2 );
		}
	}
	pd3dDevice->SetVertexShader( NULL );
	pd3dDevice->SetVertexDeclaration( NULL );
	pd3dDevice->SetPixelShader ( NULL );

	m_pSB_VSPS_SAVE->Apply();

	pd3dDevice->SetTexture( 1, NULL );
	pd3dDevice->SetTexture( 2, NULL );

	return S_OK;
}
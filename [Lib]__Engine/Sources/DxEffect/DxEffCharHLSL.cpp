#include "pch.h"

#include "./DxViewPort.h"
#include "./DxEffSinglePropGMan.h"

#include "./DxSkinMesh9.h"
#include "./RENDERPARAM.h"

#include "./DxEffCharHLSL.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace NSCHARHLSL
{
	ID3DXEffect*		m_pNormalFX		= NULL;
	ID3DXEffect*		m_pSpecularFX	= NULL;
	ID3DXEffect*		m_pCubeFX		= NULL;
	ID3DXEffect*		m_pReflectFX	= NULL;
	ID3DXEffect*		m_pLevel1FX		= NULL;
	ID3DXEffect*		m_pToonFX		= NULL;
	ID3DXEffect*		m_pEdgeFX		= NULL;

	ID3DXEffect*		m_pNormalBaseFX		= NULL;
	ID3DXEffect*		m_pNormalSpecularFX	= NULL;

	HRESULT LoadFX( LPDIRECT3DDEVICEQ pd3dDevice, TCHAR* pszName, LPD3DXEFFECT& pFX )
	{
		char szPathName[MAX_PATH] = _T("");
		StringCchCopy( szPathName, MAX_PATH, DxEffSinglePropGMan::GetInstance().GetPath() );
		StringCchCat( szPathName, MAX_PATH, pszName );

		DWORD dwShaderFlags = D3DXFX_NOT_CLONEABLE;

		#ifdef DEBUG_VS
			dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
		#endif
		#ifdef DEBUG_PS
			dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
		#endif

		// If this fails, there should be debug output as to
		// they the .fx file failed to compile
		HRESULT hr(S_OK);
		V_RETURN( D3DXCreateEffectFromFileA( pd3dDevice, szPathName, NULL, NULL, dwShaderFlags, NULL, &pFX, NULL ) );

		return S_OK;
	}

	HRESULT CreateDevice( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		switch( RENDERPARAM::emCharRenderTYPE )
		{
		case EMCRT_VERTEX:
			LoadFX( pd3dDevice, _T("SkinnedMesh.fx"),		m_pNormalFX );
			LoadFX( pd3dDevice, _T("SkinnedSpecular.fx"),	m_pSpecularFX );
			LoadFX( pd3dDevice, _T("SkinnedCube.fx"),		m_pCubeFX );
			LoadFX( pd3dDevice, _T("SkinnedReflect.fx"),	m_pReflectFX );
			LoadFX( pd3dDevice, _T("SkinnedLevel1.fx"),		m_pLevel1FX );
			LoadFX( pd3dDevice, _T("SkinnedToon.fx"),		m_pToonFX );
			LoadFX( pd3dDevice, _T("SkinnedEdge.fx"),		m_pEdgeFX );
			break;
		case EMCRT_PIXEL:
			LoadFX( pd3dDevice, _T("SkinnedMesh.fx"),		m_pNormalFX );
			LoadFX( pd3dDevice, _T("SkinnedSpecular.fx"),	m_pSpecularFX );
			LoadFX( pd3dDevice, _T("SkinnedCube.fx"),		m_pCubeFX );
			LoadFX( pd3dDevice, _T("SkinnedReflect.fx"),	m_pReflectFX );
			LoadFX( pd3dDevice, _T("SkinnedLevel1.fx"),		m_pLevel1FX );
			LoadFX( pd3dDevice, _T("PixelToon.fx"),			m_pToonFX );
			LoadFX( pd3dDevice, _T("SkinnedEdge.fx"),		m_pEdgeFX );

			LoadFX( pd3dDevice, _T("NormalBase.fx"),		m_pNormalBaseFX );
			LoadFX( pd3dDevice, _T("NormalSpecular.fx"),	m_pNormalSpecularFX );
			break;
		};

		return S_OK;
	}

	HRESULT ResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		HRESULT	hr(S_OK);
		if( m_pNormalFX )	V_RETURN( m_pNormalFX->OnResetDevice() );
		if( m_pSpecularFX )	V_RETURN( m_pSpecularFX->OnResetDevice() );
		if( m_pCubeFX )		V_RETURN( m_pCubeFX->OnResetDevice() );
		if( m_pReflectFX )	V_RETURN( m_pReflectFX->OnResetDevice() );
		if( m_pLevel1FX )	V_RETURN( m_pLevel1FX->OnResetDevice() );
		if( m_pToonFX )		V_RETURN( m_pToonFX->OnResetDevice() );
		if( m_pEdgeFX )		V_RETURN( m_pEdgeFX->OnResetDevice() );

		if( m_pNormalBaseFX )		V_RETURN( m_pNormalBaseFX->OnResetDevice() );
		if( m_pNormalSpecularFX )	V_RETURN( m_pNormalSpecularFX->OnResetDevice() );

		return S_OK;
	}

	HRESULT LostDevice()
	{
		HRESULT	hr(S_OK);
		if( m_pNormalFX )	V_RETURN( m_pNormalFX->OnLostDevice() );
		if( m_pSpecularFX )	V_RETURN( m_pSpecularFX->OnLostDevice() );
		if( m_pCubeFX )		V_RETURN( m_pCubeFX->OnLostDevice() );
		if( m_pReflectFX )	V_RETURN( m_pReflectFX->OnLostDevice() );
		if( m_pLevel1FX )	V_RETURN( m_pLevel1FX->OnLostDevice() );
		if( m_pToonFX )		V_RETURN( m_pToonFX->OnLostDevice() );
		if( m_pEdgeFX )		V_RETURN( m_pEdgeFX->OnLostDevice() );

		if( m_pNormalBaseFX )		V_RETURN( m_pNormalBaseFX->OnLostDevice() );
		if( m_pNormalSpecularFX )	V_RETURN( m_pNormalSpecularFX->OnLostDevice() );

		return S_OK;
	}

	void DestroyDevice()
	{
		SAFE_RELEASE( m_pNormalFX );
		SAFE_RELEASE( m_pSpecularFX );
		SAFE_RELEASE( m_pCubeFX );
		SAFE_RELEASE( m_pReflectFX );
		SAFE_RELEASE( m_pLevel1FX );
		SAFE_RELEASE( m_pToonFX );
		SAFE_RELEASE( m_pEdgeFX );

		SAFE_RELEASE( m_pNormalBaseFX );
		SAFE_RELEASE( m_pNormalSpecularFX );
	}

	void FrameMove()
	{
		switch( RENDERPARAM::emCharRenderTYPE )
		{
		case EMCRT_SOFTWARE:
		case EMCRT_NORMAL:
			return;
			break;
		}


		D3DXVECTOR3 vDir( 1.f, -1.f, 1.f );
		D3DXVec3Normalize( &vDir, &vDir );
		vDir = -vDir;

		D3DXVECTOR4 vDir4;
		vDir4.x = vDir.x;
		vDir4.y = vDir.y;
		vDir4.z = vDir.z;
		vDir4.w = 1.f;

		D3DXVECTOR3 vCameraDir3;
		D3DXVECTOR4 vCameraDir;
		vCameraDir3 = DxViewPort::GetInstance().GetLookDir();
		vCameraDir.x = -vCameraDir3.x;
		vCameraDir.y = -vCameraDir3.y;
		vCameraDir.z = -vCameraDir3.z;
		vCameraDir.w = 1.f;

		D3DXVECTOR3 vCameraPos3;
		D3DXVECTOR4 vCameraPos;
		vCameraPos3 = DxViewPort::GetInstance().GetFromPt();
		vCameraPos.x = vCameraPos3.x;
		vCameraPos.y = vCameraPos3.y;
		vCameraPos.z = vCameraPos3.z;
		vCameraPos.w = 1.f;


		HRESULT hr(S_OK);
		V( m_pNormalFX->SetVector( "lhtDir", &vDir4) );

		V( m_pSpecularFX->SetVector( "lhtDir", &vDir4) );
		V( m_pSpecularFX->SetVector( "CameraDir", &vCameraDir) );

		V( m_pCubeFX->SetVector( "lhtDir", &vDir4) );
		V( m_pCubeFX->SetVector( "CameraDir", &vCameraDir) );

		V( m_pReflectFX->SetVector( "lhtDir", &vDir4) );
		V( m_pReflectFX->SetVector( "CameraDir", &vCameraDir) );

		// Set Light for vertex shader
		V( m_pToonFX->SetVector( "lhtDir", &vDir4) );

		if( RENDERPARAM::emCharRenderTYPE==EMCRT_PIXEL )
		{
			V( m_pNormalBaseFX->SetVector( "lhtDir", &vDir4 ) );
			V( m_pNormalBaseFX->SetFloat( "g_fLightIntensity", 1.f ) );

			V( m_pNormalSpecularFX->SetFloat( "g_fLightIntensity", 1.f ) );
			V( m_pNormalSpecularFX->SetVector( "lhtDir", &vDir4 ) );
			V( m_pNormalSpecularFX->SetVector( "CameraPos", &vCameraPos) );
		}
	}
};



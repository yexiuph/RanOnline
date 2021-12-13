#include "pch.h"
#include "dxmeshtexman.h"
#include "DxRenderStates.h"
#include "DxViewPort.h"
#include "./TextureManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DxMeshTexMan& DxMeshTexMan::GetInstance()
{
	static DxMeshTexMan Instance;
	return Instance;
}

HRESULT DxMeshTex::LoadMesh ( LPDIRECT3DDEVICEQ pd3dDevice, std::string strMeshName )
{
	m_pMesh = DxSimpleMeshMan::GetInstance().Load ( strMeshName.c_str() , pd3dDevice, D3DFVF_ITEM_VERTEX );
	//m_pMesh->SetFVF( D3DFVF_ITEM_VERTEX );

	D3DXVECTOR3	vMax, vMin, vSize;
	m_pMesh->GetBoundBox ( vMax, vMin );
	m_pMesh->GetBoundSphere ( m_vObjectCenter, m_fObjectRadius );
	m_fScalingFactor = (float)0.5f * 0.9f * TEXTURE_SIZE_X / m_fObjectRadius;
	
    vSize = vMax - vMin;

	m_sTexturePos.left = 0.5f-m_fScalingFactor*(0.55f*vSize.x )/TEXTURE_SIZE_X;
	m_sTexturePos.top  = 0.5f-m_fScalingFactor*(0.55f*vSize.y)/TEXTURE_SIZE_Y;
	m_sTexturePos.sizex= 1.1f*m_fScalingFactor * vSize.x/TEXTURE_SIZE_X;
	m_sTexturePos.sizey= 1.1f*m_fScalingFactor * vSize.y/TEXTURE_SIZE_Y;

	//m_vObjectCenter = vSize/2.f;		//사이즈 상 오브젝트 중점

	D3DXVECTOR3 vEye = D3DXVECTOR3( 0.0f, 0.0f, -500.0f );
	D3DXVECTOR3 vAt  = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUp  = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	D3DXMatrixLookAtLH ( &m_matView, &vEye, &vAt, &vUp );
	D3DXMatrixScaling ( &m_matScaling, m_fScalingFactor , m_fScalingFactor , m_fScalingFactor );
	D3DXMatrixTranslation ( &m_matTrans, -m_fScalingFactor*m_vObjectCenter.x,-m_fScalingFactor*m_vObjectCenter.y, -m_fScalingFactor*m_vObjectCenter.z );
	
	return S_OK;
}

HRESULT DxMeshTex::Create ( LPDIRECT3DDEVICEQ pd3dDevice, std::string strMeshName, BOOL bDynamic )
{
	HRESULT hr=S_OK;
	m_strMeshName = strMeshName;
	m_bDynamic = bDynamic;

	MakeTexture ( pd3dDevice );

	if ( !m_bDynamic )	LoadMesh ( pd3dDevice, strMeshName );
	if ( !m_bDynamic )	DrawTextureStatic ( pd3dDevice );

	return S_OK;
}

HRESULT DxMeshTex::DrawTextureStatic ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr=S_OK;
	if ( !m_pMeshTexture )	return E_FAIL;

	D3DXVECTOR3 vEye = D3DXVECTOR3( 0.0f, 0.0f, -500.0f );
	D3DXVECTOR3 vAt  = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUp  = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	D3DXMatrixLookAtLH ( &m_matView, &vEye, &vAt, &vUp );
	D3DXMatrixScaling ( &m_matScaling, m_fScalingFactor , m_fScalingFactor , m_fScalingFactor );
	D3DXMatrixTranslation ( &m_matTrans, -m_fScalingFactor*m_vObjectCenter.x,-m_fScalingFactor*m_vObjectCenter.y, -m_fScalingFactor*m_vObjectCenter.z );
	//현재 렌더 타겟 백업
	LPDIRECT3DSURFACEQ pOldSurf, pDepthSurf;
	hr = pd3dDevice->GetDepthStencilSurface ( &pDepthSurf );
	GASSERT(SUCCEEDED(hr));

	hr = pd3dDevice->GetRenderTarget ( 0, &pOldSurf );
	GASSERT(SUCCEEDED(hr));

	//렌더 타켓으로 쓰일 텍스처의 표면 가져오기	
	LPDIRECT3DSURFACEQ pRenderSurf;
	hr = m_pMeshTexture->GetSurfaceLevel ( 0, &pRenderSurf );
	GASSERT(SUCCEEDED(hr));

	//새로운 렌더 타켓 설정
	hr = pd3dDevice->SetRenderTarget( 0, pRenderSurf );
	pd3dDevice->SetDepthStencilSurface( NULL );
	GASSERT(SUCCEEDED(hr));
	hr = pd3dDevice->Clear ( 0L, NULL, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0L );
	GASSERT(SUCCEEDED(hr));

	hr = pd3dDevice->SetTransform ( D3DTS_VIEW, &m_matView );
	GASSERT(SUCCEEDED(hr));

	//D3DXMatrixPerspectiveFovLH ( &m_matProj, D3DX_PI/4, TEXTURE_SIZE_X / TEXTURE_SIZE_Y, 0./0f, 1000.0f );
	D3DXMatrixOrthoLH ( &m_matProj, (float)TEXTURE_SIZE_X, (float)TEXTURE_SIZE_Y, 3.0f, 2000.0f );
	hr = pd3dDevice->SetTransform ( D3DTS_PROJECTION, &m_matProj );
	GASSERT(SUCCEEDED(hr));

	D3DXMATRIX worldmat, matRotate;

	D3DXMatrixRotationY ( &matRotate, 0.f );
	D3DXMatrixMultiply (&worldmat, &m_matScaling, &m_matTrans);
	D3DXMatrixMultiply (&worldmat, &worldmat, &matRotate );

	hr = pd3dDevice->SetTransform ( D3DTS_WORLD, &worldmat );
	GASSERT(SUCCEEDED(hr));

	if( SUCCEEDED( pd3dDevice->BeginScene() ) )
	{
		DxRenderStates::GetInstance().SetMake3DMap ( pd3dDevice );

		//메쉬 렌더						
		hr = m_pMesh->Render( pd3dDevice );
		GASSERT(SUCCEEDED(hr));

		DxRenderStates::GetInstance().ReSetMake3DMap( pd3dDevice );
		pd3dDevice->EndScene();
	}

	//텍스쳐 표면 해제하기
	hr = pRenderSurf->Release();
	GASSERT(SUCCEEDED(hr));

	//원래 렌더 타겟 설정
	hr = pd3dDevice->SetRenderTarget( 0, pOldSurf );
	pd3dDevice->SetDepthStencilSurface( pDepthSurf );
	GASSERT(SUCCEEDED(hr));

	//렌더표면 깊이표면 해제하기
	hr = pOldSurf->Release();
	GASSERT(SUCCEEDED(hr));
	
	hr = pDepthSurf->Release();
	GASSERT(SUCCEEDED(hr));

	//이전 뷰 매트릭스와 투영 매트릭스로 되돌리기
	hr = pd3dDevice->SetTransform(D3DTS_VIEW , &DxViewPort::GetInstance().GetMatView());
	GASSERT(SUCCEEDED(hr));

	hr = pd3dDevice->SetTransform(D3DTS_PROJECTION , &DxViewPort::GetInstance().GetMatProj());
	GASSERT(SUCCEEDED(hr));

	//hr = D3DXSaveTextureToFile ( "test.mesh.bmp", D3DXIFF_BMP, m_pMeshTexture, NULL );
	GASSERT(SUCCEEDED(hr));

	return S_OK;
}

HRESULT DxMeshTex::MakeTexture ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr=S_OK;
	m_pMeshTexture = DxMeshTexMan::GetInstance().PopTexture ();

	return hr;
}

HRESULT DxMeshTex::ReleaseTexture ()
{
	DxMeshTexMan::GetInstance().PushTexture ( m_pMeshTexture );

	return S_OK;
}

HRESULT DxMeshTex::Delete ()
{
	ReleaseTexture ();
	DxSimpleMeshMan::GetInstance().Release ( m_pMesh->m_szFileName, D3DFVF_ITEM_VERTEX );

	return S_OK;
}

int DxMeshTex::SetupOrthoProjForRenderCopy(LPDIRECT3DDEVICEQ pD3DDevice)
{
	// Note : View Matrix 설정
	//
	pD3DDevice->SetTransform( D3DTS_VIEW, &m_matView );
	// Note : 메쉬를 직교 투영 좌표 설정
	// 
	D3DXMatrixOrthoLH(&m_matProj, (float)TEXTURE_SIZE_X, (float)TEXTURE_SIZE_Y, 1.0, 1000.0);
	pD3DDevice->SetTransform( D3DTS_PROJECTION, &m_matProj );

	D3DXMATRIX worldmat, matRotate;
	// 회전 이동 행렬 설정
	//
	D3DXMatrixRotationY( &matRotate, m_fAngle * D3DX_PI);
	D3DXMatrixMultiply(&worldmat, &m_matScaling, &m_matTrans);
	D3DXMatrixMultiply(&worldmat, &worldmat, &matRotate );

	pD3DDevice->SetTransform( D3DTS_WORLD, &worldmat );

	return 0;
}

HRESULT DxMeshTex::UpdateRender ( float fElapsedTime, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;
	if ( !m_pMeshTexture )	return E_FAIL;

	GASSERT(m_pMesh && "메쉬가 없습니다.");
	if ( !m_pMeshTexture )	return -1;		//텍스처 표면이 없는 경우 실패

	m_fAngle += 1.0f * fElapsedTime;
	if ( m_fAngle > 2.f )	m_fAngle = 0.f;

	//현재 렌더 타겟 백업
	LPDIRECT3DSURFACEQ pOldSurf, pDepthSurf;
	pd3dDevice->GetDepthStencilSurface ( &pDepthSurf );
	pd3dDevice->GetRenderTarget ( 0, &pOldSurf );

	//렌더 타켓으로 쓰일 텍스처의 표면 가져오기	
	LPDIRECT3DSURFACEQ pRenderSurf;
	m_pMeshTexture->GetSurfaceLevel(0, &pRenderSurf);	

	//새로운 렌더 타켓 설정
	pd3dDevice->SetRenderTarget( 0, pRenderSurf );
	pd3dDevice->SetDepthStencilSurface( NULL );
	hr = pd3dDevice->Clear ( 0L, NULL, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0L );

	SetupOrthoProjForRenderCopy(pd3dDevice);

	//////////////////////////////////////////////////////////////////////////////////////
	//																					//
	if( SUCCEEDED( pd3dDevice->BeginScene() ) )
	{
		DxRenderStates::GetInstance().SetMake3DMap( pd3dDevice );											//
		//메쉬 렌더																			//
		hr = m_pMesh->Render( pd3dDevice );													//	
		GASSERT(SUCCEEDED(hr));																//
		//																					//
		DxRenderStates::GetInstance().ReSetMake3DMap( pd3dDevice );
	
		pd3dDevice->EndScene();
	}
	//																					//
	//////////////////////////////////////////////////////////////////////////////////////

	//텍스쳐 표면 해제하기
	pRenderSurf->Release();
	//원래 렌더 타겟 설정
	pd3dDevice->SetRenderTarget( 0, pOldSurf  );
	pd3dDevice->SetDepthStencilSurface( pDepthSurf );
	//렌더표면 깊이표면 해제하기
	pOldSurf->Release();
	pDepthSurf->Release();

	//이전 뷰 매트릭스와 투영 매트릭스로 되돌리기
	pd3dDevice->SetTransform(D3DTS_VIEW , &DxViewPort::GetInstance().GetMatView());
	pd3dDevice->SetTransform(D3DTS_PROJECTION , &DxViewPort::GetInstance().GetMatProj());

	GASSERT(SUCCEEDED(hr));

	return S_OK;
}

HRESULT DxMeshTex::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	MakeTexture ( pd3dDevice );
	if ( !m_bDynamic )	DrawTextureStatic ( pd3dDevice );

	return S_OK;
}

HRESULT DxMeshTex::InvalidateDeviceObjects ()
{
	ReleaseTexture ();

	return S_OK;
}


DxMeshTexMan::DxMeshTexMan(void)
{
}

DxMeshTexMan::~DxMeshTexMan(void)
{
}

HRESULT DxMeshTexMan::MakeTexture ( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DTEXTUREQ &pMeshTexture )
{
	HRESULT hr=S_OK;

	LPDIRECT3DQ pDirect3D=NULL;
	hr = pd3dDevice->GetDirect3D ( &pDirect3D );
	if ( FAILED(hr) )	goto RETERN_GOTO;

	D3DCAPSQ d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );

	D3DSURFACE_DESC   d3dsdBackBuffer;
	LPDIRECT3DSURFACEQ pBackBuffer=NULL;
	hr = pd3dDevice->GetBackBuffer ( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
	if ( FAILED(hr) )	goto RETERN_GOTO;

	pBackBuffer->GetDesc( &d3dsdBackBuffer );
	pBackBuffer->Release();

	hr = pDirect3D->CheckDeviceFormat ( d3dCaps.AdapterOrdinal,
								d3dCaps.DeviceType, d3dsdBackBuffer.Format,
								D3DUSAGE_RENDERTARGET, D3DRTYPE_TEXTURE,
								D3DFMT_A8R8G8B8 );
	if ( SUCCEEDED(hr) )
	{
		hr = D3DXCreateTexture ( pd3dDevice, DxMeshTex::TEXTURE_SIZE_X, DxMeshTex::TEXTURE_SIZE_Y, 1,
							D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8,
							D3DPOOL_DEFAULT, &pMeshTexture );
	}

	if ( pMeshTexture )	goto RETERN_GOTO;

	if ( !pMeshTexture )
	{
		hr = pDirect3D->CheckDeviceFormat ( d3dCaps.AdapterOrdinal,
									d3dCaps.DeviceType, d3dsdBackBuffer.Format,
									D3DUSAGE_RENDERTARGET, D3DRTYPE_TEXTURE,
									D3DFMT_A4R4G4B4 );
		if ( SUCCEEDED(hr) )
		{
			hr = D3DXCreateTexture ( pd3dDevice, DxMeshTex::TEXTURE_SIZE_X, DxMeshTex::TEXTURE_SIZE_Y, 1,
								D3DUSAGE_RENDERTARGET, D3DFMT_A4R4G4B4,
								D3DPOOL_DEFAULT, &pMeshTexture );
		}
	}

RETERN_GOTO:
	SAFE_RELEASE(pDirect3D);

	return hr;
}

HRESULT DxMeshTexMan::ReleaseTextureAll ()
{
	REQTEXTURE_ITER iter = m_ReqTexture.begin();
	REQTEXTURE_ITER iter_end = m_ReqTexture.end();
	for ( ; iter!=iter_end; ++iter )
	{
		SAFE_RELEASE( (*iter) );
	}

	m_ReqTexture.clear ();

	return S_OK;
}

HRESULT DxMeshTexMan::ReserveTexture ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr=S_OK;

	//int i = m_ReqTexture.size();
	//for ( ; i<26; ++i )
	//{
	//	LPDIRECT3DTEXTUREQ pMeshTexture=NULL;
	//	hr = MakeTexture ( pd3dDevice, pMeshTexture );
	//	if ( SUCCEEDED(hr) && pMeshTexture )
	//	{
	//		m_ReqTexture.push_back ( pMeshTexture );
	//	}
	//}

	return hr;
}

void DxMeshTexMan::PushTexture ( LPDIRECT3DTEXTUREQ pTexture )
{
	if ( !pTexture )	return;
	m_ReqTexture.push_back ( pTexture );
}

LPDIRECT3DTEXTUREQ DxMeshTexMan::PopTexture ()
{
	if ( m_ReqTexture.empty() )		return NULL;

	LPDIRECT3DTEXTUREQ pTexture = *m_ReqTexture.begin();
	m_ReqTexture.pop_front();

	return pTexture;
}


PDXMESHTEX DxMeshTexMan::CreateTexture ( LPDIRECT3DDEVICEQ pd3dDevice, std::string strMeshName, BOOL bDynamic )
{
	MESHTEXMAP_ITER iter = m_MeshTexMap.find ( strMeshName );
	if ( iter != m_MeshTexMap.end() )
	{
		(*iter).second->m_dwRef++;
		return (*iter).second;
	}

	PDXMESHTEX pMeshTex = new DxMeshTex;
	HRESULT hr = pMeshTex->Create ( pd3dDevice, strMeshName, bDynamic );
	if ( FAILED(hr) )
	{
		MessageBox ( NULL, "Mesh Texture Load Fail.", strMeshName.c_str(), MB_OK );

		SAFE_DELETE(pMeshTex);
		return NULL;
	}

	m_MeshTexMap.insert ( std::make_pair(strMeshName,pMeshTex) );

	pMeshTex->m_dwRef++;
	return pMeshTex;
}

HRESULT DxMeshTexMan::ReleaseTexture ( std::string strMeshName )
{
	MESHTEXMAP_ITER iter = m_MeshTexMap.find ( strMeshName );
	if ( iter != m_MeshTexMap.end() )
	{
		(*iter).second->m_dwRef--;
		if ( (*iter).second->m_dwRef==0 )
		{
			(*iter).second->Delete ();

			SAFE_DELETE ( (*iter).second );
		}

		return S_OK;
	}

	return S_FALSE;
}

HRESULT DxMeshTexMan::DeleteAll ()
{
	MESHTEXMAP_ITER iter = m_MeshTexMap.begin();
	MESHTEXMAP_ITER iter_end = m_MeshTexMap.end();
	for ( ; iter!=iter_end; ++iter )
	{
		if ( (*iter).second )	(*iter).second->Delete ();
		SAFE_DELETE ( (*iter).second );	
	}
	
	m_MeshTexMap.clear();

	return S_OK;
}

HRESULT DxMeshTexMan::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	ReserveTexture(pd3dDevice);

	MESHTEXMAP_ITER iter = m_MeshTexMap.begin();
	MESHTEXMAP_ITER iter_end = m_MeshTexMap.end();
	for ( ; iter!=iter_end; ++iter )
	{
		(*iter).second->RestoreDeviceObjects ( pd3dDevice );
	}

	return S_OK;
}

HRESULT DxMeshTexMan::InvalidateDeviceObjects ()
{
	MESHTEXMAP_ITER iter = m_MeshTexMap.begin();
	MESHTEXMAP_ITER iter_end = m_MeshTexMap.end();
	for ( ; iter!=iter_end; ++iter )
	{
		(*iter).second->InvalidateDeviceObjects ();
	}

	ReleaseTextureAll ();

	return S_OK;
}

HRESULT DxMeshTexMan::FrameMove ( float fTime, float fElapsedTime )
{
	DWORD dwReqTextures = DWORD(m_ReqTexture.size());

	//CDebugSet::ToView ( 9, "REQ TEXTURE  %d", dwReqTextures );

	return S_OK;
}

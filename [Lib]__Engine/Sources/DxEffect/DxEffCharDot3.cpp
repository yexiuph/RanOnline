#include "pch.h"
#include "./DxEffSingle.h"
#include "./DxEffSinglePropGMan.h"
#include "./SerialFile.h"

#include "./DxLightMan.h"
#include "./DxViewPort.h"
#include "./DxEffectMan.h"

#include "./DxCharPart.h"
#include "./DxSkinPieceContainer.h"

#include "./DxEffCharDot3.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DWORD		DxEffCharDot3::TYPEID			= EMEFFCHAR_DOT3;
DWORD		DxEffCharDot3::VERSION			= 0x0100;
char		DxEffCharDot3::NAME[MAX_PATH]	= "3.범프_스펙_반사";

LPDIRECT3DSTATEBLOCK9		DxEffCharDot3::m_pSavedStateBlock		= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffCharDot3::m_pEffectStateBlock	= NULL;

DxEffCharDot3::DxEffCharDot3(void) :
	DxEffChar(),
	m_pSkinMesh(NULL),
	m_pmcMesh(NULL),
	m_pTexture(NULL),
	m_pTexture1(NULL),
	m_pTexture2(NULL),
	m_pTexture3(NULL),
	m_pDot3Tex1(NULL),
	m_pDot3Tex2(NULL),
	m_pDot3Tex3(NULL)
{
}

DxEffCharDot3::~DxEffCharDot3(void)
{
	CleanUp ();
}

DxEffChar* DxEffCharDot3::CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece )
{
	HRESULT hr;
	DxEffCharDot3 *pEffChar = new DxEffCharDot3;
	pEffChar->SetLinkObj ( pCharPart, pSkinPiece );
	pEffChar->SetProperty ( &m_Property );

	hr = pEffChar->Create ( pd3dDevice );
	if ( FAILED(hr) )
	{
		SAFE_DELETE(pEffChar);
		return NULL;
	}

	return pEffChar;
}

HRESULT DxEffCharDot3::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{	
//	HRESULT	hr;

	//DWORD dwDecl[] =
	//{
	//	D3DVSD_STREAM(0),
	//	D3DVSD_REG(0, D3DVSDT_FLOAT3 ),		//	D3DVSDE_POSITION,  0  
	//	D3DVSD_REG(3, D3DVSDT_FLOAT3 ),		//	Normal
	//	D3DVSD_REG(7, D3DVSDT_FLOAT2 ),		//	Tex1
	//	D3DVSD_END()
	//};



	//hr = pd3dDevice->CreateVertexShader ( dwDecl, (DWORD*)dwDot3VertexShader, &m_dwEffect, 
	//										DxEffectMan::GetInstance().GetUseSwShader() );
	//if( FAILED(hr) )
	//{
	//	CDebugSet::ToListView ( "[ERROR] VS _ Dot3 FAILED" );
	//	return E_FAIL;
	//}

	//DWORD dwDecl2[] =
	//{
	//	D3DVSD_STREAM(0),
	//	D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_FLOAT3), // Position in object space
	//	D3DVSD_REG(D3DVSDE_NORMAL, D3DVSDT_FLOAT3), // Normal in object space
	//	D3DVSD_SKIP(1), // Skip diffuse color
	//	D3DVSD_REG(D3DVSDE_TEXCOORD0, D3DVSDT_FLOAT2), // Texture coordinates
	//	D3DVSD_REG(D3DVSDE_TEXCOORD1, D3DVSDT_FLOAT3), // Tangent in object space
	//	D3DVSD_REG(D3DVSDE_TEXCOORD2, D3DVSDT_FLOAT3), // Binormal in object space
	//	D3DVSD_REG(D3DVSDE_TEXCOORD3, D3DVSDT_FLOAT3), // Normal in object space
	//	D3DVSD_END()
	//};

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		//	Note : SetRenderState() 선언
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		TRUE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	FALSE );
		pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,			FALSE );

		//	Note : SetTextureStageState() 선언
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedStateBlock );
		else			pd3dDevice->EndStateBlock( &m_pEffectStateBlock );
	}

	return S_OK;
}

HRESULT DxEffCharDot3::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSavedStateBlock );
	SAFE_RELEASE( m_pEffectStateBlock );

	return S_OK;
}

HRESULT DxEffCharDot3::OneTimeSceneInit ()
{

	return S_OK;
}

HRESULT DxEffCharDot3::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;

	if ( m_pCharPart )
	{
		m_pSkinMesh = m_pCharPart->m_pSkinMesh;
		m_pmcMesh = m_pCharPart->m_pmcMesh;
	}
	else if ( m_pSkinPiece )
	{
		m_pSkinMesh = m_pSkinPiece->m_pSkinMesh;
		m_pmcMesh = m_pSkinPiece->m_pmcMesh;
	}

	TextureManager::LoadTexture ( m_szTexture, pd3dDevice, m_pTexture, 0, 0, TRUE );
	TextureManager::LoadTexture ( m_szTexture_1, pd3dDevice, m_pTexture1, 0, 0, TRUE );
	TextureManager::LoadTexture ( m_szTexture_2, pd3dDevice, m_pTexture2, 0, 0, TRUE );
	TextureManager::LoadTexture ( m_szTexture_3, pd3dDevice, m_pTexture3, 0, 0, TRUE );

	TextureManager::LoadTexture ( m_szDot3Tex_1, pd3dDevice, m_pDot3Tex1, 0, 0, TRUE );
	TextureManager::LoadTexture ( m_szDot3Tex_2, pd3dDevice, m_pDot3Tex2, 0, 0, TRUE );
	TextureManager::LoadTexture ( m_szDot3Tex_3, pd3dDevice, m_pDot3Tex3, 0, 0, TRUE );


	//	Note : 비어있다면 건너 뛴다.
	//
	if ( !m_pmcMesh )	return S_OK;

	for ( DWORD i=0; i < m_pmcMesh->GetNumMaterials(); i++ )
	{
		if ( i == 0 )
		{
			m_dwFlag |= ISTEXTURE1;
			StringCchCopy( m_szSrcTex_1, MAX_PATH, m_pmcMesh->pMaterials[i].pTextureFilename );
		}
		else if ( i == 1 )
		{
			m_dwFlag |= ISTEXTURE2;
			StringCchCopy( m_szSrcTex_2, MAX_PATH, m_pmcMesh->pMaterials[i].pTextureFilename );
		}
		else if ( i == 2 )
		{
			m_dwFlag |= ISTEXTURE3;
			StringCchCopy( m_szSrcTex_3, MAX_PATH, m_pmcMesh->pMaterials[i].pTextureFilename );
		}
	}

	return S_OK;
}

HRESULT DxEffCharDot3::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	return S_OK;
}

HRESULT DxEffCharDot3::InvalidateDeviceObjects ()
{
	m_pd3dDevice = NULL;

	return S_OK;
}

HRESULT DxEffCharDot3::DeleteDeviceObjects ()
{
	TextureManager::ReleaseTexture( m_szTexture, m_pTexture );
	TextureManager::ReleaseTexture( m_szTexture_1, m_pTexture1 );
	TextureManager::ReleaseTexture( m_szTexture_2, m_pTexture2 );
	TextureManager::ReleaseTexture( m_szTexture_3, m_pTexture3 );
	TextureManager::ReleaseTexture( m_szDot3Tex_1, m_pDot3Tex1 );
	TextureManager::ReleaseTexture( m_szDot3Tex_2, m_pDot3Tex2 );
	TextureManager::ReleaseTexture( m_szDot3Tex_3, m_pDot3Tex3 );

	return S_OK;
}

HRESULT DxEffCharDot3::FinalCleanup ()
{

	return S_OK;
}

HRESULT DxEffCharDot3::FrameMove ( float fTime, float fElapsedTime )
{
	HRESULT hr = S_OK;

	return S_OK;
}

void	DxEffCharDot3::Render ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bPieceRender )
{
	HRESULT hr = S_OK;

	if ( !m_pmcMesh->MeshData.pMesh )	return;
	if ( !m_pSkinMesh )					return;

//	D3DXVECTOR3	vHalfVector;
//	D3DXVECTOR3	vLightVector;
//	D3DXVECTOR3	vAmbient;
//	D3DXVECTOR3	vPow;
//
//	D3DXMATRIX matView, matProj;
//
//	matView = DxViewPort::GetInstance().GetMatView();
//	matProj = DxViewPort::GetInstance().GetMatProj();
//
//	D3DXVECTOR3	vLookatPt	= DxViewPort::GetInstance().GetLookatPt();
//	D3DXVECTOR3	vFromPt		= DxViewPort::GetInstance().GetFromPt();
//	D3DXVECTOR3	vDir		= vLookatPt-vFromPt;
//	D3DXVec3Normalize ( &vDir, &vDir);
//
//	vHalfVector		= DxLightMan::GetInstance()->GetDirectLight()->m_Light.Direction;
//
//	D3DXVec3TransformNormal ( &vDir, &vDir, &matView );
//	D3DXVec3Normalize ( &vDir, &vDir);
//
//	vHalfVector		= vDir;
//
//	vLightVector	= DxLightMan::GetInstance()->GetDirectLight()->m_Light.Direction;
//	vLightVector	= vLightVector;
//
////	D3DXVec3TransformNormal ( &vLightVector, &vLightVector, &matView );
//	D3DXVec3Normalize ( &vLightVector, &vLightVector);
//
////	D3DXVec3TransformCoord ( &vFromPt, &vFromPt, &matView );
//
//	vLightVector.x	= (0.5f * vLightVector.x) + 0.5f;
//	vLightVector.y	= (0.5f * vLightVector.y) + 0.5f;
//	vLightVector.z	= (0.5f * vLightVector.z) + 0.5f;
//
//	vAmbient.x		= DxLightMan::GetInstance()->GetDirectLight()->m_Light.Ambient.r + 50.f*DIV_1_255;
//	vAmbient.y		= DxLightMan::GetInstance()->GetDirectLight()->m_Light.Ambient.g + 50.f*DIV_1_255;
//	vAmbient.z		= DxLightMan::GetInstance()->GetDirectLight()->m_Light.Ambient.b + 50.f*DIV_1_255;
//
//	pd3dDevice->SetPixelShaderConstant ( 0, &vLightVector, 1 );
//	pd3dDevice->SetPixelShaderConstant ( 1, &vHalfVector, 1 );
//	pd3dDevice->SetPixelShaderConstant ( 2, &vAmbient, 1 );
//
//	if ( DxEffectMan::GetInstance().GetSkinDetail() == SKD_BUMP_SPEC )
//	{
//		pd3dDevice->SetTexture ( 2, m_pTexture );
//
//		m_pSkinMesh->DrawMeshContainer ( pd3dDevice, m_pmcMesh, TRUE, FALSE, FALSE, m_dwDetail_1, m_dwDetail_2, m_dwDetail_3,
//										m_pTexture1, m_pTexture2, m_pTexture3, m_pDot3Tex1, m_pDot3Tex2, m_pDot3Tex3 );
//
//		pd3dDevice->SetPixelShader ( NULL );
//
//		pd3dDevice->SetTexture ( 0, NULL );
//		pd3dDevice->SetTexture ( 1, NULL );
//		pd3dDevice->SetTexture ( 2, NULL );
//	}
//	else if ( DxEffectMan::GetInstance().GetSkinDetail() == SKD_SPEC )
//	{
//		pd3dDevice->SetTexture ( 2, m_pTexture );
//
//		m_pSkinMesh->DrawMeshContainer ( pd3dDevice, m_pmcMesh, TRUE, FALSE, FALSE, m_dwDetail_1, m_dwDetail_2, m_dwDetail_3,
//										m_pTexture1, m_pTexture2, m_pTexture3 );
//
//		pd3dDevice->SetPixelShader ( NULL );
//
//		pd3dDevice->SetTexture ( 0, NULL );
//		pd3dDevice->SetTexture ( 1, NULL );
//		pd3dDevice->SetTexture ( 2, NULL );
//	}
//	else
//	{
//		m_pSkinMesh->DrawMeshContainer ( pd3dDevice, m_pmcMesh, TRUE, FALSE, FALSE );
//
//		pd3dDevice->SetPixelShader ( NULL );
//
//		pd3dDevice->SetTexture ( 0, NULL );
//		pd3dDevice->SetTexture ( 1, NULL );
//		pd3dDevice->SetTexture ( 2, NULL );
//	}
}

HRESULT	DxEffCharDot3::LoadFile ( basestream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	//	Note : 버전이 일치할 경우. 
	//
	if ( dwVer == VERSION )
	{
		//	Note : 이팩트의 Property 를 저장.
		//
		SFile.ReadBuffer ( &m_Property, sizeof(m_Property) );

		//	Note : Device 자원을 생성한다.
		//
		hr = Create ( pd3dDevice );
		if ( FAILED(hr) )	return hr;

	}
	else
	{
		//	Note : 버전이 틀릴 경우에는 파일에 쓰여진 DATA영역을 건더 띄는 작업을 진행.
		//
		SFile.SetOffSet ( SFile.GetfTell()+dwSize );
		
		return E_FAIL;
	}

	return S_OK;
}

HRESULT	DxEffCharDot3::SaveFile ( basestream &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	//	Note : DATA의 사이즈를 기록한다. Load 시에 버전이 틀릴 경우 사용됨.
	//
	SFile << (DWORD) ( sizeof(m_Property) );

	//	Note : 이팩트의 Property 를 저장.
	//
	SFile.WriteBuffer ( &m_Property, sizeof(m_Property) );

	return S_OK;
}















//		//	Note : SetFVF에 World와 WorldViewProjection 행렬 집어 넣기
//		//
//		D3DXMATRIX	matWorld, matView, matProj, matWVP;
//
//		matView = DxViewPort::GetInstance().GetMatView();
//		matProj = DxViewPort::GetInstance().GetMatProj();
//
//		//D3DXMatrixTranspose( &matWorld, &(pframeCur->matCombined) );
//		//pd3dDevice->SetVertexShaderConstant ( VSC_MATWORLD,			&matWorld,	4 );
//
//		//D3DXMatrixMultiply ( &matWVP, &(pframeCur->matCombined), &matView );
//		//D3DXMatrixMultiply ( &matWVP, &matWVP, &matProj );
//		//D3DXMatrixTranspose( &matWVP, &matWVP );
//
//		// Send light direction in object space
//		D3DXMATRIX world2obj;
//		D3DXMatrixInverse(&world2obj, 0, &matWorld);
//		D3DXVECTOR3 lightDirectionInObjectSpace;
//		D3DXVECTOR3	vLightDirection = DxLightMan::GetInstance()->GetDirectLight()->m_Light.Direction;
//		D3DXVec3TransformNormal(&lightDirectionInObjectSpace, &vLightDirection, &world2obj);
//		// shader math requires that the light direction vector points towards the light.
//		lightDirectionInObjectSpace = - lightDirectionInObjectSpace;
//	//	cgD3D8SetUniform(m_BumpDot3x2DiffuseSpecularVSLightVector, lightDirectionInObjectSpace);
//
//		// Send eye position in object space
//		D3DXMATRIX view2world;
//		matView = DxViewPort::GetInstance().GetMatView();
//		D3DXMatrixInverse(&view2world, 0, &matView);
//		D3DXVECTOR3 eyePositionInWorldSpace(view2world._41, view2world._42, view2world._43);
//		D3DXVECTOR3 eyePositionInObjectSpace;
//		D3DXVec3TransformCoord(&eyePositionInObjectSpace, &eyePositionInWorldSpace, &world2obj);
////		cgD3D8SetUniform(m_BumpDot3x2DiffuseSpecularVSEyePosition, eyePositionInObjectSpace);
//
//		pd3dDevice->SetVertexShaderConstant ( 0, &matWVP,	4 );
//		pd3dDevice->SetVertexShaderConstant ( 4, &lightDirectionInObjectSpace, 1 );
//		pd3dDevice->SetVertexShaderConstant ( 5, &eyePositionInObjectSpace, 1 );
//		pd3dDevice->SetVertexShaderConstant ( 6, &D3DXVECTOR4( 0.5f, 0.5f, 0.5f, 0.0f ), 1 );
//

//
//		m_pmcMesh->SetMaterial ( pd3dDevice, 0 );
//
//		//	빛위치
//		D3DXVECTOR3		vDir, vSrcDir, vLookatPt, vFromPt, vWidth, vUp;
//		vSrcDir		= DxLightMan::GetInstance()->GetDirectLight()->m_Light.Direction;
//		vLookatPt	= DxViewPort::GetInstance().GetLookatPt();
//		vFromPt		= DxViewPort::GetInstance().GetFromPt();
//		vDir		= vLookatPt-vFromPt;
//		////vDir.x		+= sinf(D3DX_PI*0.5f);
//		vWidth.x	= vDir.x;
//		vWidth.y	= 0.f;
//		vWidth.z	= vDir.z;
//		vUp.x		= 0.f;
//		vUp.y		= 1.f;
//		vUp.x		= 0.f;
//		//vWidth	*= 0.5f;
//		vDir	+= vWidth;
//		////vDir.z		+= cosf(D3DX_PI*0.5f);
//		//D3DXVec3Normalize ( &vDir, &vDir );
//		//DxLightMan::GetInstance()->GetDirectLight()->m_Light.Direction.x = vDir.x;
//		//DxLightMan::GetInstance()->GetDirectLight()->m_Light.Direction.y = vDir.y;
//		//DxLightMan::GetInstance()->GetDirectLight()->m_Light.Direction.z = vDir.z;
//
//		D3DLIGHTQ	pSrcLight;
//		BOOL	bLightEnable;
//
//		pd3dDevice->GetLightEnable	( 7, &bLightEnable );
//		pd3dDevice->GetLight		( 7, &pSrcLight );
//
//		D3DLIGHTQ	pLight;
//
//		pLight.Type        = D3DLIGHT_DIRECTIONAL;
//		pLight.Diffuse.r   = 1.0f;
//		pLight.Diffuse.g   = 1.0f;
//		pLight.Diffuse.b   = 1.0f;
//		pLight.Ambient.r	= 0.6f;
//		pLight.Ambient.g	= 0.6f;
//		pLight.Ambient.b	= 0.6f;
//		pLight.Specular.r   = 1.0f;
//		pLight.Specular.g   = 0.0f;
//		pLight.Specular.b   = 0.0f;
//		pLight.Direction	= vDir;
//		pLight.Range		= 80.0f;
//		pLight.Attenuation0 = 1.0f;
//		pLight.Attenuation1 = 0.0f;
//		pLight.Attenuation2 = 0.0f;
//
//		pd3dDevice->LightEnable ( 7, TRUE );
//		pd3dDevice->SetLight ( 7, &pLight );

		//pd3dDevice->SetTexture ( 1, m_pTexture1 );
		//pd3dDevice->SetTexture ( 2, m_pDot3Tex1 );


















//
//		m_pSavedStateBlock->Capture();
//		m_pEffectStateBlock->Apply();
//
//		pd3dDevice->SetFVF ( m_dw2VertexEffect );
//
//		D3DXVECTOR3	vHalfVector;
//		D3DXVECTOR3	vLightVector;
//		D3DXVECTOR3	vAmbient;
//		D3DXVECTOR3	vPow;
//
//		D3DXVECTOR3	vLookatPt	= DxViewPort::GetInstance().GetLookatPt();
//		D3DXVECTOR3	vFromPt		= DxViewPort::GetInstance().GetFromPt();
//		D3DXVECTOR3	vDir		= vLookatPt-vFromPt;
//		D3DXVec3Normalize ( &vDir, &vDir);
//
//		vHalfVector		= DxLightMan::GetInstance()->GetDirectLight()->m_Light.Direction;
//		vHalfVector		= vDir;
//	//	vHalfVector.x	= (vHalfVector.x - 0.5f) * 2.f;
//	//	vHalfVector.y	= (vHalfVector.y - 0.5f) * 2.f;
//	//	vHalfVector.z	= (vHalfVector.z - 0.5f) * 2.f;
//
//		vLightVector	= DxLightMan::GetInstance()->GetDirectLight()->m_Light.Direction;
//		vLightVector	= vLightVector;
//		vLightVector.x	= (0.5f * vLightVector.x) + 0.5f;
//		vLightVector.y	= (0.5f * vLightVector.y) + 0.5f;
//		vLightVector.z	= (0.5f * vLightVector.z) + 0.5f;
//	//	oDiffuse.xyz    = 0.5 * mul(World, Normal) + 0.5.xxx;
//	//	vLightVector.x	= (vLightVector.x - 0.5f) * 2.f;
//	//	vLightVector.y	= (vLightVector.y - 0.5f) * 2.f;
//	//	vLightVector.z	= (vLightVector.z - 0.5f) * 2.f;
//
//		vAmbient.x		= DxLightMan::GetInstance()->GetDirectLight()->m_Light.Ambient.r;
//		vAmbient.y		= DxLightMan::GetInstance()->GetDirectLight()->m_Light.Ambient.g;
//		vAmbient.z		= DxLightMan::GetInstance()->GetDirectLight()->m_Light.Ambient.b;
//
//		vPow.x = 5.f;
//		vPow.y = 5.f;
//		vPow.z = 5.f;
//		//vAmbient.x		= 0.5f;
//		//vAmbient.y		= 0.5f;
//		//vAmbient.z		= 0.5f;
//
//	//	pd3dDevice->SetPixelShaderConstant ( 0, &vHalfVector, 1 );
////		pd3dDevice->SetPixelShaderConstant ( 1, &vLightVector, 1 );
//		//pd3dDevice->SetPixelShaderConstant ( 2, &vAmbient, 1 );
//		//pd3dDevice->SetPixelShaderConstant ( 3, &vPow, 1 );
//		pd3dDevice->SetPixelShaderConstant ( 0, &vLightVector, 1 );
//		pd3dDevice->SetPixelShaderConstant ( 1, &vHalfVector, 1 );
//		pd3dDevice->SetPixelShaderConstant ( 2, &vAmbient, 1 );
//
//		//pd3dDevice->SetPixelShader ( m_dw2PixelEffect );
//
//		pd3dDevice->SetTexture ( 2, m_pTexture );
//
//		m_pSkinMesh->DrawMeshContainer ( pd3dDevice, m_pmcMesh, TRUE, FALSE, FALSE, m_pSrcTex, m_pTexture1,
//										m_pTexture2, m_pTexture3, m_pDot3Tex1, m_pDot3Tex2, m_pDot3Tex3 );
//
//		pd3dDevice->SetPixelShader ( NULL );
//
//		//pd3dDevice->LightEnable ( 7, bLightEnable );
//		//pd3dDevice->SetLight ( 7, &pSrcLight );
//
//		//	빛위치
//	//	DxLightMan::GetInstance()->GetDirectLight()->m_Light.Direction = vSrcDir;
//
//		pd3dDevice->SetTexture ( 0, NULL );
//		pd3dDevice->SetTexture ( 1, NULL );
//		pd3dDevice->SetTexture ( 2, NULL );
//
//		m_dwSavedStateBlock->Apply();
//	}
//	else	{}
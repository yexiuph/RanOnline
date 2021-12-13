#include "pch.h"
#include "./DxViewPort.h"
#include "./SerialFile.h"

#include "./TextureManager.h"

#include "./DxQUADTree.h"

#include "./StlFunctions.h"

#include "./GLDefine.h"

#include "./DxEffectMan.h"

#include "./DxMapEditMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//	Note : DxMapEditMan, 버전
//
const DWORD DxMapEditMan::VERSION		= 0x00000001;
const DWORD	DxMapEditMan::VERSION_DATA	= 0x00000001;
const DWORD	DxMapEditMan::VERSION_TEX	= 0x00000001;

const DWORD		DxMapEditMan::VERTEX::FVF			= D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1;

char			DxMapEditMan::m_szPathData[]		= "";
char			DxMapEditMan::m_szPathTex[]			= "";
char			DxMapEditMan::m_szFullNameData[]	= "";
char			DxMapEditMan::m_szFullNameTex[]		= "";
char			DxMapEditMan::m_szExtNameMain[]		= ".ted";
char			DxMapEditMan::m_szExtNameEdit[]		= ".edt";
char			DxMapEditMan::m_szExtNameGame[]		= ".gam";

const float		DxMapEditMan::fCURVEVALUE		= -0.92f;		// -1.f ~ 1.f

LPDIRECT3DSTATEBLOCK9	DxMapEditMan::m_pSavedSB = NULL;
LPDIRECT3DSTATEBLOCK9	DxMapEditMan::m_pDrawSB = NULL;

DWORD			DxMapEditMan::m_dwLength_EDIT = 40;

D3DXVECTOR3		DxMapEditMan::m_vMin = D3DXVECTOR3(0.f,0.f,0.f);
D3DXVECTOR3		DxMapEditMan::m_vMax = D3DXVECTOR3(0.f,0.f,0.f);

int				DxMapEditMan::m_nTILE_X		= 0;
int				DxMapEditMan::m_nTILE_Z		= 0;
int				DxMapEditMan::m_nPOINT_X	= 0;
int				DxMapEditMan::m_nPOINT_Z	= 0;
int				DxMapEditMan::m_nTILE_X_S	= 0;
int				DxMapEditMan::m_nTILE_Z_S	= 0;
int				DxMapEditMan::m_nPOINT_X_S	= 0;
int				DxMapEditMan::m_nPOINT_Z_S	= 0;
D3DXVECTOR3		DxMapEditMan::m_vPOINT		= D3DXVECTOR3 ( 0.f, 0.f, 0.f );

DxMapEditMan::DxMapEditMan (void) :
	m_bBlushCircle_EDIT(TRUE),
	m_nBlushScale_EDIT(1),
	m_fHeightValue_EDIT(1.f),
	m_bAbsolute_EDIT(TRUE),
	m_bUneven_EDIT(FALSE),
	m_bUp_EDIT(TRUE),
	m_bTILE_DRAW(TRUE),
	m_bCOLOR_DRAW(FALSE),
	m_fBright_EDIT(0.f),
	m_fRandom_EDIT(0.f),
	m_nColor_EDIT(0),
	m_nBASE_X(0),
	m_nBASE_Z(0),
	m_bFINDTILE(FALSE),
	m_pQuadList(NULL),
	m_pQuadTree(NULL)
{
	m_dwGrid_Length = EM_GRID*m_dwLength_EDIT;
	m_szDefaultFileName = "Default.tld";

	for ( int i=0; i<14; ++i )
	{
		m_cColor_EDIT[i] = D3DXCOLOR ( 0.5f, 0.5f, 0.5f, 0.f );
	}

	memset( m_szMapName, 0, sizeof(char)*128 );
	memset( m_cBaseName, 0, sizeof(char)*100 );
}

DxMapEditMan::~DxMapEditMan (void)
{
	m_listModify.clear();
	m_listModifyWAIT.clear();

	m_listPOINT_EDIT.clear();
	m_listTILE_EDIT.clear();

	m_mapSortPOINT.clear();
	m_mapSortTILE.clear();

	// Note : Data 로딩이 Thread 에서 이루어짐으로 Thread 가 종료 후에 삭제를 한다.
	while (1)
	{
		if ( m_sLoadingThread.sData.bDelete )
		{
			SAFE_DELETE ( m_pQuadTree );	// 삭제.!!
			SAFE_DELETE ( m_pQuadList );	// 삭제.!!

			break;
		}
	}

	NSQUADTREE::m_mapBaseEDIT.clear();
}

HRESULT DxMapEditMan::ActiveMap ()
{
	return S_OK;
}

HRESULT DxMapEditMan::OneTimeSceneInit ()
{
	HRESULT hr = S_OK;
	return S_OK;
}

HRESULT DxMapEditMan::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;

	m_sLoadingThread.StartThread ( pd3dDevice );
	m_sPieceMan.InitDeviceObjects ( pd3dDevice );

	return S_OK;
}

HRESULT DxMapEditMan::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG2 );

		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP,	D3DTOP_SELECTARG1 );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG1 );

		pd3dDevice->SetTextureStageState ( 1, D3DTSS_ALPHAARG2,	D3DTA_CURRENT );
		pd3dDevice->SetTextureStageState ( 1, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG2 );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedSB );
		else			pd3dDevice->EndStateBlock( &m_pDrawSB );
	}

	return S_OK;
}

HRESULT DxMapEditMan::InvalidateDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSavedSB );
	SAFE_RELEASE( m_pDrawSB );

	return S_OK;
}

HRESULT DxMapEditMan::DeleteDeviceObjects ()
{
	HRESULT hr = S_OK;

	m_sLoadingThread.EndThread ();
	m_sPieceMan.DeleteDeviceObjects();

	return S_OK;
}

HRESULT DxMapEditMan::FinalCleanup ()
{
	HRESULT hr = S_OK;
	return S_OK;
}

HRESULT DxMapEditMan::FrameMove ( float fTime, float fElapsedTime )
{
	HRESULT hr = S_OK;

	return S_OK;
}

HRESULT DxMapEditMan::Render ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bGame )
{
	HRESULT hr = S_OK;

	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity ( &matIdentity );

	pd3dDevice->SetTransform ( D3DTS_WORLD, &matIdentity );
	
	D3DMATERIALQ sMaterial;
	sMaterial.Diffuse.r = 1.f;
	sMaterial.Diffuse.g = 1.f;
	sMaterial.Diffuse.b = 1.f;
	sMaterial.Diffuse.a = 1.f;
	sMaterial.Ambient.r = 1.f;
	sMaterial.Ambient.g = 1.f;
	sMaterial.Ambient.b = 1.f;
	sMaterial.Ambient.a = 1.f;
	sMaterial.Emissive.r = 0.f;
	sMaterial.Emissive.g = 0.f;
	sMaterial.Emissive.b = 0.f;
	sMaterial.Emissive.a = 0.f;
	sMaterial.Specular.r = 0.f;
	sMaterial.Specular.g = 0.f;
	sMaterial.Specular.b = 0.f;
	sMaterial.Specular.a = 0.f;
	pd3dDevice->SetMaterial ( &sMaterial );

	CLIPVOLUME	cv = DxViewPort::GetInstance().GetClipVolume ();
	NSQUADTREE::RenderQUADTree ( pd3dDevice, &cv, m_pQuadTree, bGame );

	// StaticPiece ~~
	m_sPieceMan.SetMousePoint ( m_vPOINT );

	if ( bGame )	m_sPieceMan.Render ( pd3dDevice, &cv );
	else 			m_sPieceMan.Render_EDIT ( pd3dDevice, &cv );


	return S_OK;
}

HRESULT DxMapEditMan::Render_PieceEDIT ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity ( &matIdentity );

	pd3dDevice->SetTransform ( D3DTS_WORLD, &matIdentity );
	
	D3DMATERIALQ sMaterial;
	sMaterial.Diffuse.r = 1.f;
	sMaterial.Diffuse.g = 1.f;
	sMaterial.Diffuse.b = 1.f;
	sMaterial.Diffuse.a = 1.f;
	sMaterial.Ambient.r = 1.f;
	sMaterial.Ambient.g = 1.f;
	sMaterial.Ambient.b = 1.f;
	sMaterial.Ambient.a = 1.f;
	sMaterial.Emissive.r = 0.f;
	sMaterial.Emissive.g = 0.f;
	sMaterial.Emissive.b = 0.f;
	sMaterial.Emissive.a = 0.f;
	sMaterial.Specular.r = 0.f;
	sMaterial.Specular.g = 0.f;
	sMaterial.Specular.b = 0.f;
	sMaterial.Specular.a = 0.f;
	pd3dDevice->SetMaterial ( &sMaterial );

	CLIPVOLUME	cv = DxViewPort::GetInstance().GetClipVolume ();
	m_sPieceMan.Render_SELECT ( pd3dDevice, &cv );

	return S_OK;
}

void DxMapEditMan::SetPiece ( D3DXVECTOR3& vPickRayOrig, D3DXVECTOR3& vPickRayDir )
{
	m_sPieceMan.SetPickRay ( vPickRayOrig, vPickRayDir );
}

void DxMapEditMan::SetPiece ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_sPieceMan.SetLButtonClick ( pd3dDevice );
}

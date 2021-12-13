// Terrain0.cpp: implementation of the DxEffectTiling class.
//
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "StlFunctions.h"

#include "./TextureManager.h"
#include "./EditMeshs.h"
#include "./DxMaterial.h"
#include "./DxFrameMesh.h"
#include "./DxEffectDefine.h"

#include "./SerialFile.h"
#include "./Collision.h"

#include "./DxLandMan.h"
#include "./DxEffectMan.h"
#include "./DxStaticMeshThread.h"

#include "./DxEffectTiling.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const DWORD DxEffectTiling::TYPEID = DEF_EFFECT_TILING;
const DWORD	DxEffectTiling::VERSION = 0x00000102;
const char DxEffectTiling::NAME[] = "[ 타일 시스템 : 블렌딩 ]";
const DWORD DxEffectTiling::FLAG = _EFF_SINGLE;

const DWORD		DxEffectTiling::VERTEXCOLOR::FVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1;

LPDIRECT3DSTATEBLOCK9		DxEffectTiling::m_pSavedSB			= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffectTiling::m_pDrawSB			= NULL;

LPDIRECT3DSTATEBLOCK9		DxEffectTiling::m_pSavedSB_Select	= NULL;
LPDIRECT3DSTATEBLOCK9		DxEffectTiling::m_pDrawSB_Select	= NULL;

void DxEffectTiling::GetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD &dwVer )
{
	pProp = (PBYTE) &m_Property;
	dwSize = sizeof(TILING_PROPERTY);
	dwVer = VERSION;
}

void DxEffectTiling::SetProperty ( PBYTE &pProp, DWORD &dwSize, DWORD dwVer )
{
	if ( dwVer==VERSION && dwSize==sizeof(TILING_PROPERTY) )
	{
		memcpy ( &m_Property, pProp, dwSize );
	}
	if ( dwVer==0x00000101 && dwSize==sizeof(TILING_PROPERTY) )
	{
		memcpy ( &m_Property, pProp, dwSize );
	}
	else if ( dwVer==0x00000100 && dwSize==sizeof(TILING_PROPERTY) )
	{
		memcpy ( &m_Property, pProp, dwSize );
	}
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
DxEffectTiling::DxEffectTiling () :
	m_pPoint(NULL),
	m_pPointEX(NULL),
	m_pPointEX_Load(NULL),
	m_pTriangle(NULL),
	m_pMaterials(NULL),
	m_dwPointEX(0),
	m_dwPointEX_Load(0),
	m_dwMaterials(0),
	m_dwCurTILE(0),
	m_dwCurPOINT(0),
	m_dwCurPOINT_PREV(0),
	m_bSelectDRAW(FALSE),
	m_vMax(0.f,0.f,0.f),
	m_vMin(0.f,0.f,0.f),
	m_dwVertices(0),
	m_dwFaces(0),
	m_dwMaterials_B(0),
	m_dwUNIQUE_NUMBER(1)
{

	m_dwFlag = USE_TEX_DRAW;


	UseAffineMatrix ();
	D3DXMatrixIdentity ( &m_matFrameComb );

	m_cCurCOLOR.a = 0.f;
	m_cCurCOLOR.r = 0.6f;
	m_cCurCOLOR.g = 0.6f;
	m_cCurCOLOR.b = 0.6f;

	m_cCurRANDOM.a = 0.7f;
	m_cCurRANDOM.r = 0.4f;
	m_cCurRANDOM.g = 0.4f;
	m_cCurRANDOM.b = 0.4f;
}

DxEffectTiling::~DxEffectTiling ()
{
	//SAFE_DELETE_ARRAY(m_szAdaptFrame);

	SAFE_DELETE_ARRAY ( m_pPoint );
	SAFE_DELETE_ARRAY ( m_pPointEX );
	SAFE_DELETE_ARRAY ( m_pPointEX_Load );
	SAFE_DELETE_ARRAY ( m_pTriangle );
	SAFE_DELETE_ARRAY ( m_pMaterials );

	std::for_each ( m_mapMaterials.begin(), m_mapMaterials.end(), std_afunc::DeleteMapObject() );
	m_mapMaterials.clear();

	m_listSelectTriangle.clear();

	m_mapFindEX.clear();

	//CleanUp ();
}

HRESULT DxEffectTiling::AdaptToDxFrameChild ( DxFrame *pframeCur, LPDIRECT3DDEVICEQ pd3dDevice )
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
				SetBaseQuad( pd3dDevice, pmsMeshs );
			}

			pmsMeshs = pmsMeshs->pMeshNext;
		}
	}

	return hr;
}

//	Note : DxFrame 에 효과를 붙일 경우에 사용된다.
//
HRESULT DxEffectTiling::AdaptToDxFrame ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !pFrame )		return S_OK;

	m_pAdaptFrame = pFrame;
	m_matFrameComb = pFrame->matCombined;

	this->pLocalFrameEffNext = pFrame->pEffectNext;
	pFrame->pEffectNext = this;

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

HRESULT DxEffectTiling::AdaptToEffList ( const DxFrame *const pFrame, LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT ( pFrame );

	m_pAdaptFrame = NULL;
	m_matFrameComb = pFrame->matCombined;

	if ( pFrame->szName )
	{
		SAFE_DELETE_ARRAY(m_szAdaptFrame);
		size_t nStrLen = strlen(pFrame->szName)+1;
		m_szAdaptFrame = new char[nStrLen];

		StringCchCopy( m_szAdaptFrame, nStrLen, pFrame->szName );
	}

	return S_OK;
}

HRESULT DxEffectTiling::OneTimeSceneInit ()
{
	HRESULT hr = S_OK;

	return hr;
}

HRESULT DxEffectTiling::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	float fBias = -0.0001f;
	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState( D3DRS_LIGHTING,			FALSE );

		pd3dDevice->SetRenderState( D3DRS_DEPTHBIAS,		*((DWORD*)&fBias) );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_INVSRCALPHA );

		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG2 );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedSB );
		else			pd3dDevice->EndStateBlock( &m_pDrawSB );
	}

	fBias = -0.0002f;
	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,			*((DWORD*)&fBias) );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR,		0x44000000 );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1 );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG1 );

		if( which==0 )	pd3dDevice->EndStateBlock( &m_pSavedSB_Select );
		else			pd3dDevice->EndStateBlock( &m_pDrawSB_Select );
	}

	return S_OK;
}

HRESULT DxEffectTiling::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	SAFE_RELEASE( m_pSavedSB );
	SAFE_RELEASE( m_pDrawSB );
	SAFE_RELEASE( m_pSavedSB_Select );
	SAFE_RELEASE( m_pDrawSB_Select );

	return S_OK;
}

HRESULT DxEffectTiling::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !m_pPointEX_Load )	return S_OK;

	D3DXVECTOR3 vLoadPos, vPos;
	for ( DWORD i=0; i<m_dwPointEX_Load; ++i )
	{
		for ( DWORD j=0; j<m_dwPointEX; ++j )
		{
			vLoadPos	= m_pPointEX_Load[i].vPos;
			vPos		= m_pPointEX[j].vPos;
			if ( ((vLoadPos.x+0.01f)>vPos.x) && (vLoadPos.x<(vPos.x+0.01f)) && 
				((vLoadPos.y+0.01f)>vPos.y) && (vLoadPos.y<(vPos.y+0.01f)) && 
				((vLoadPos.z+0.01f)>vPos.z) && (vLoadPos.z<(vPos.z+0.01f)) )
			{
				m_pPointEX[j].dwMaterial	= m_pPointEX_Load[i].dwMaterial;		// Load 한 데이터를 읽는다.
				m_pPointEX[j].dwColor		= m_pPointEX_Load[i].dwColor;			// Load 한 데이터를 읽는다.

				m_pPointEX_Load[i].pPoint	= m_pPointEX[j].pPoint;			// 임시로 넣어준 값이다.

				j = m_dwPointEX;		// 다음 루프로 돌린다.
			}
		}
	}

	// 삼각형 리스트를 만든다.. FrameMesh에서는 꼭 필요하다.
	DWORD				dwMaterial;
	DWORD				dwFaceNUM;
	POINTA*				pPoint;
	MATERIALMAP_ITER	iter;
	TRIANGLELIST_ITER	_iter;
	for ( DWORD i=0; i<m_dwPointEX_Load; ++i )
	{
		dwMaterial = m_pPointEX_Load[i].dwMaterial;
		iter = m_mapMaterials.find(dwMaterial);

		if ( iter != m_mapMaterials.end() )
		{
			pPoint = m_pPointEX_Load[i].pPoint;
			while (pPoint)
			{
				BOOL bUse = FALSE;
				dwFaceNUM = m_pPoint[pPoint->dwIndex].dwFace_NUM;

				_iter = (*iter).second->listTriangle.begin();
				for ( ; _iter!=(*iter).second->listTriangle.end(); ++_iter )
				{
					if ( (*_iter) == dwFaceNUM )
					{
						bUse = TRUE;
						//break;
					}
				}

				if ( !bUse )		// 값이 없을 때만 동작한다.
				{
					(*iter).second->listTriangle.push_back ( dwFaceNUM );
				}

				pPoint = pPoint->pNext;
			}
		}
		else	// 매트리얼이 없을 수 없다.. 버리자... 아니다 에러 체크 해 놓자.
		{
		}

		m_pPointEX_Load[i].pPoint = NULL;		// 이거 없으면 위험하다. 임시 값이다. 파괴 될 시 값이 있으면 안된다.
	}


	SAFE_DELETE_ARRAY ( m_pPointEX_Load );

	return S_OK;
}

HRESULT DxEffectTiling::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT DxEffectTiling::InvalidateDeviceObjects ()
{
	return S_OK;
}

HRESULT DxEffectTiling::DeleteDeviceObjects ()
{
	DxStaticMeshColor_THREAD::GetInstance().DeleteList();	//	관련 List를 지운다.

	return S_OK;
}

HRESULT DxEffectTiling::FinalCleanup ()
{
	return S_OK;
}

//	Note : 랜더링 되지 않을때 필요 없는 부분을 제거하고
//		랜더링 되지 않을때의 부하 경감을 위해 가장 단순하게 유지한다.
//
HRESULT DxEffectTiling::FrameMove ( float fTime, float fElapsedTime )
{
	m_listSelectTriangle.clear();

	m_dwCurPOINT_PREV = m_dwCurPOINT;

	// Note : 옥트리일 경우만 작동
	if( !DxStaticMeshColor_THREAD::GetInstance().m_bOctreeMode )	return S_OK;

	// Note : 컬러 계산을 다하여서 리스트가 비어있다면 다시 삽입한다.
	if( DxStaticMeshColor_THREAD::GetInstance().IsInsertListData() )
	{
		DxStaticMeshColor_THREAD::GetInstance().EnterCS();
		{
			LOADINGDATALIST& sListColorData = DxStaticMeshColor_THREAD::GetInstance().m_listColorData;

			MATERIALMAP_ITER iter = m_mapMaterials.begin();
			for ( ; iter!=m_mapMaterials.end(); ++iter )
			{
				if( !(*iter).second->m_pMeshList )
				{
					DxStaticMeshColor_THREAD::GetInstance().LeaveCS();
					return S_OK;
				}

				(*iter).second->m_pMeshList->InsertColorList( sListColorData );
			}
		}
		DxStaticMeshColor_THREAD::GetInstance().LeaveCS();
	}

	return S_OK;
}

HRESULT DxEffectTiling::Render ( DxFrame *pFrame, LPDIRECT3DDEVICEQ pd3dDevice, DxFrameMesh *pFrameMesh, DxLandMan* pLandMan )
{
	PROFILE_BEGIN("DxEffectTiling");

	HRESULT hr = S_OK;

	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity ( &matIdentity );
	matIdentity._42 += 0.05f;
	pd3dDevice->SetTransform ( D3DTS_WORLD, &matIdentity );

	if ( pFrameMesh )
	{
		PROFILE_BEGIN("DxEffectTiling_Create");
		CreateTiles ( pd3dDevice );					// 삼각형 생성		// 삼각형 선택시 원본이 보이도록 해야 한다.
		PROFILE_END("DxEffectTiling_Create");

		PROFILE_BEGIN("DxEffectTiling_Render");
		RenderTiles ( pd3dDevice, m_mapMaterials );	// 삼각형 뿌리기
		PROFILE_END("DxEffectTiling_Render");

		if ( m_bSelectDRAW )
		{
			m_listSelectTriangle.clear();
			MATERIALMAP_ITER iter = m_mapMaterials.find(m_dwCurTILE);
			if ( iter != m_mapMaterials.end() )
			{
				TRIANGLELIST_ITER iter2 = (*iter).second->listTriangle.begin();
				for ( ; iter2!=(*iter).second->listTriangle.end(); ++iter2 )
				{
					m_listSelectTriangle.push_back ( (*iter2) );
				}
			}
			Render_Select ( pd3dDevice, FALSE );
		}
	}

	// 옥트리 일때
	if ( pLandMan )
	{
		m_pSavedSB->Capture();
		m_pDrawSB->Apply();

		CLIPVOLUME	cv = DxViewPort::GetInstance().GetClipVolume ();

		MATERIALMAP_ITER iter = m_mapMaterials.begin();
		for ( ; iter!=m_mapMaterials.end(); ++iter )
		{
			if( (*iter).second->m_bAlpha )	pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );
			else							pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG2 );

			if ( (*iter).second->m_pMeshList )
			{
				CSerialFile& SFile = pLandMan->GetSerialFile();
				(*iter).second->m_pMeshList->RenderDYNAMIC( pd3dDevice, cv, &SFile );
			}
		}

		m_pSavedSB->Apply();
	}

	PROFILE_END("DxEffectTiling");

	return S_OK;
}

void DxEffectTiling::RenderTiles ( LPDIRECT3DDEVICEQ pd3dDevice, MATERIALMAP& mapMaterials )
{
	m_pSavedSB->Capture();
	m_pDrawSB->Apply();

	pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );	// FrameMesh는 Normal을 사용한다.

	CLIPVOLUME	cv = DxViewPort::GetInstance().GetClipVolume ();

	MATERIALMAP_ITER iter = m_mapMaterials.begin();
	for ( ; iter!=m_mapMaterials.end(); ++iter )
	{
		if( (*iter).second->m_bAlpha )	pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );
		else							pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG2 );

		RenderTiles ( pd3dDevice, (*iter).second );
		//if ( (*iter).first == m_dwCurTILE )
		//{
		//	RenderTiles ( pd3dDevice, (*iter).second );
		//}
		//else if ( (*iter).second->m_pMeshList )
		//{
		//	(*iter).second->m_pMeshList->Render ( pd3dDevice, &cv );
		//}
	}

	m_pSavedSB->Apply();
}

void DxEffectTiling::RenderTiles ( LPDIRECT3DDEVICEQ pd3dDevice, MATERIALEX* pMaterialEX )
{
	pd3dDevice->SetFVF ( VERTEXCOLOR::FVF );

	pd3dDevice->SetTexture ( 0, pMaterialEX->pTexture );

	FACELIST_ITER iter = pMaterialEX->listFace.begin();
	for ( ; iter!=pMaterialEX->listFace.end(); ++iter )
	{
		pd3dDevice->DrawPrimitiveUP ( D3DPT_TRIANGLELIST, 1, (*iter).sVertex, sizeof(VERTEXCOLOR) );
	}
}

HRESULT DxEffectTiling::Render_Select ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bCenter )
{
	D3DXMATRIX	matIdentity;
	D3DXMatrixIdentity ( &matIdentity );
	pd3dDevice->SetTransform ( D3DTS_WORLD, &matIdentity );

	m_pSavedSB_Select->Capture();
	m_pDrawSB_Select->Apply();

	pd3dDevice->SetTexture ( 0, NULL );

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

	D3DCOLOR	cColor_Src = 0x44000000;
	D3DCOLOR	cColor_Dest = 0x00000000;

	if ( bCenter )
	{
		cColor_Src = 0x4400ff00;
		cColor_Dest = 0x0000ff00;
	}
	else
	{
		cColor_Src = 0x44ff0000;
		cColor_Dest = 0x44ff0000;
	}

	int nCount = 0;
	DWORD dwVERTEX_NUM;
	DWORD dwFaces_NUM;

	dwFaces_NUM = (DWORD)m_listSelectTriangle.size();
	VERTEXCOLOR* pVertex = new VERTEXCOLOR[dwFaces_NUM*3];

	TRIANGLELIST_ITER iter = m_listSelectTriangle.begin();
	for ( ; iter != m_listSelectTriangle.end(); ++iter )
	{
		dwFaces_NUM = (*iter);

		dwVERTEX_NUM = dwFaces_NUM*3+0;
		pVertex[nCount].vPos = m_pPoint[dwVERTEX_NUM].vPos;
		pVertex[nCount].vNor = m_pPoint[dwVERTEX_NUM].vNor;
		pVertex[nCount].dwColor = cColor_Dest;
		pVertex[nCount].vTex = m_pPoint[dwVERTEX_NUM].vTex;
		if ( m_dwCurPOINT == m_pPoint[dwVERTEX_NUM].dwPointEx  )
		{
			pVertex[nCount].dwColor = cColor_Src;
		}
		++nCount;

		dwVERTEX_NUM = dwFaces_NUM*3+1;
		pVertex[nCount].vPos = m_pPoint[dwVERTEX_NUM].vPos;
		pVertex[nCount].vNor = m_pPoint[dwVERTEX_NUM].vNor;
		pVertex[nCount].dwColor = cColor_Dest;
		pVertex[nCount].vTex = m_pPoint[dwVERTEX_NUM].vTex;
		if ( m_dwCurPOINT == m_pPoint[dwVERTEX_NUM].dwPointEx  )
		{
			pVertex[nCount].dwColor = cColor_Src;
		}
		++nCount;

		dwVERTEX_NUM = dwFaces_NUM*3+2;
		pVertex[nCount].vPos = m_pPoint[dwVERTEX_NUM].vPos;
		pVertex[nCount].vNor = m_pPoint[dwVERTEX_NUM].vNor;
		pVertex[nCount].dwColor = cColor_Dest;
		pVertex[nCount].vTex = m_pPoint[dwVERTEX_NUM].vTex;
		if ( m_dwCurPOINT == m_pPoint[dwVERTEX_NUM].dwPointEx  )
		{
			pVertex[nCount].dwColor = cColor_Src;
		}
		++nCount;
	}

	dwFaces_NUM = (DWORD)m_listSelectTriangle.size();

	pd3dDevice->SetFVF ( VERTEXCOLOR::FVF );
	pd3dDevice->DrawPrimitiveUP ( D3DPT_TRIANGLELIST, dwFaces_NUM, pVertex, sizeof(VERTEXCOLOR) );

	m_pSavedSB_Select->Apply();

	SAFE_DELETE ( pVertex );

	return S_OK;
}

HRESULT DxEffectTiling::CloneData ( DxEffectBase* pSrcEffect, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DxEffectTiling *pEffSrc = (DxEffectTiling*)pSrcEffect;

	m_matFrameComb	= pEffSrc->m_matFrameComb;

	std::for_each ( m_mapMaterials.begin(), m_mapMaterials.end(), std_afunc::DeleteMapObject() );
	m_mapMaterials.clear();

	MATERIALMAP_ITER iter		= pEffSrc->m_mapMaterials.begin();
	MATERIALMAP_ITER iter_end	= pEffSrc->m_mapMaterials.end();

	for ( ; iter!=pEffSrc->m_mapMaterials.end(); ++iter )
	{
		MATERIALEX* pMaterials = new MATERIALEX;
		pMaterials->strTexture = ((*iter).second)->strTexture;
		TextureManager::LoadTexture ( pMaterials->strTexture.c_str(), pd3dDevice, pMaterials->pTexture, 0, 0 );

		pMaterials->m_bAlpha = (BOOL)TextureManager::GetTexType( pMaterials->strTexture.c_str() );	// 알파 쓰는지 안쓰는지 체크

		FACELIST_ITER _iter = (*iter).second->listFace.begin();
		for ( ; _iter!=(*iter).second->listFace.end(); ++_iter )
		{
			if ( ((*_iter).sVertex[0].dwColor>>24) || ((*_iter).sVertex[1].dwColor>>24) || ((*_iter).sVertex[2].dwColor>>24) )
			{
				pMaterials->listFace.push_back ( (*_iter) );
			}
		}

		m_mapMaterials.insert ( std::make_pair((*iter).first,pMaterials) );		// 삽입
	}

	iter = m_mapMaterials.begin();
	for ( ; iter!=m_mapMaterials.end(); ++iter )
	{
		CreateAABBOctree ( pd3dDevice, (*iter).second );
	}

	return S_OK;
}

void DxEffectTiling::CreateAABBOctree ( LPDIRECT3DDEVICEQ pd3dDevice, MATERIALEX* pMaterialEX )
{
	DWORD dwSize = (DWORD)pMaterialEX->listFace.size();
	if ( dwSize )
	{
		VERTEXCOLOR* pVert = new VERTEXCOLOR[dwSize*3];

		int nCount = 0;
		FACELIST_ITER iter = pMaterialEX->listFace.begin();
		for ( ; iter!=pMaterialEX->listFace.end(); ++iter )
		{
			pVert[nCount*3+0] = (*iter).sVertex[0];
			pVert[nCount*3+1] = (*iter).sVertex[1];
			pVert[nCount*3+2] = (*iter).sVertex[2];

            ++nCount;
		}

		SAFE_DELETE ( pMaterialEX->m_pMeshList );
		pMaterialEX->m_pMeshList = new DxSingleTexMesh;
		pMaterialEX->m_pMeshList->Create( pd3dDevice, pMaterialEX->strTexture.c_str(), dwSize, (BYTE*)pVert, VERTEXCOLOR::FVF, FALSE );

		SAFE_DELETE_ARRAY ( pVert );
	}
}



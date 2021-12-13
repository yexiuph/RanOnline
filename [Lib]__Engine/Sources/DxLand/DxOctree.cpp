#include "pch.h"
#include "./DxOctree.h"
#include "./DxLandMan.h"

#include "./DxEffectRender.h"

#include "./EditMeshs.h"
#include "./DxEffectDefine.h"
#include "./DxEffectFrame.h"
#include "./DxFrameMesh.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace	OctreeDebugInfo
{
	int			TotalNodesDrawn;
	int			EndNodeCount;
	int			AmountDxFrame;
	int			TodalDxFrameDrawn;
	int			FaceViewCount;
	int			VertexViewCount;
};

DWORD	DxOctree::m_MaxTriangles = 0;
DWORD	DxOctree::m_MaxSubdivisions = 0;
DWORD	DxOctree::m_CurrentSubdivision = 0;

LPDIRECT3DDEVICEQ	DxOctree::m_pd3dDevice = NULL;
DxFrameMesh*		DxOctree::m_pFrameMesh = NULL;

//	Note	:	디버그용,	SKY
BOOL			g_bDebugLine = FALSE;

DxOctree::DxOctree() :
	m_pDxFrameHead(NULL),
	m_pDxAlphaMapFrameHead(NULL),
	m_bSubDivided(FALSE),
	m_SFile(NULL),
	m_pEffGlobalHead(NULL),
	m_vMin(D3DXVECTOR3(-FLT_MAX,-FLT_MAX,-FLT_MAX)),
	m_vMax(D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX)),
	m_bLoaded(FALSE),
	m_DataAddress(0)
{	
	memset( m_pOctreeNodes, 0, sizeof(DxOctree*)*8 );
}
 
DxOctree::~DxOctree()
{	
	m_bSubDivided = FALSE;
	for ( int i = 0; i < 8; i++ )
	{
		SAFE_DELETE( m_pOctreeNodes[i] );		
	}
	m_vMin = D3DXVECTOR3 ( -FLT_MAX, -FLT_MAX, -FLT_MAX );
	m_vMax = D3DXVECTOR3 ( FLT_MAX, FLT_MAX, FLT_MAX );
	SAFE_DELETE ( m_pDxFrameHead );

}


HRESULT DxOctree::InitDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice)
{
	m_pd3dDevice = pd3dDevice;

	m_SFile				= NULL;
	m_pEffGlobalHead	= NULL;
	m_pDxAlphaMapFrameHead = NULL;

	return	S_OK;
}

HRESULT DxOctree::DeleteDeviceObjects()
{
	m_bSubDivided = FALSE;
	for ( int i = 0; i < 8; i++ )
	{
		SAFE_DELETE( m_pOctreeNodes[i] );		
	}
	m_vMin = D3DXVECTOR3 ( -FLT_MAX, -FLT_MAX, -FLT_MAX );
	m_vMax = D3DXVECTOR3 ( FLT_MAX, FLT_MAX, FLT_MAX );
	SAFE_DELETE ( m_pDxFrameHead );

	m_pFrameMesh = NULL;
	m_pDxAlphaMapFrameHead = NULL;

	m_MaxTriangles = 0;
	m_MaxSubdivisions = 0;
	m_CurrentSubdivision = 0;
	
	return S_OK;
}

void DxOctree::DrawFrame ( DxFrame *pframeCur, DxLandMan* pLandMan )
{
	//	Note : 원본 Render 전 이팩트
	//
	DxEffectBase* pEffectCur;
	pEffectCur = pframeCur->pEffectPrev;
	while ( pEffectCur )
	{
		pEffectCur->Render ( pframeCur, m_pd3dDevice );
		pEffectCur = pEffectCur->pLocalFrameEffNext;
	}

	if ( pframeCur->pEffect )
	{
		//	Note : 원본 대체 이팩트
		//
		pEffectCur = pframeCur->pEffect;
		while ( pEffectCur )
		{
			pEffectCur->Render ( pframeCur, m_pd3dDevice );
			pEffectCur = pEffectCur->pLocalFrameEffNext;
		}
	}
	else
	{
		//	Note : 원본 Render
		//
		if ( pframeCur->pmsMeshs != NULL )
		{
			//	Note : 메쉬 그리기.
			//
			DxMeshes *pmsMeshs = pframeCur->pmsMeshs;
			while ( pmsMeshs != NULL )
			{
				if ( pmsMeshs->m_pDxOctreeMesh )
				{
					pmsMeshs->RenderOctree ( m_pd3dDevice );
				}
				pmsMeshs = pmsMeshs->pMeshNext;
			}
		}
	}

	//	Note : 원본 Render 후 이팩트
	//
	pEffectCur = pframeCur->pEffectNext;
	for ( ; pEffectCur; pEffectCur = pEffectCur->pLocalFrameEffNext )
	{
		if ( pEffectCur->GetFlag() & _EFF_AFTER )
		{
			pLandMan->GetAfterRender()->AfterRender ( 0, DXEFFECTREND(pEffectCur,pframeCur,pLandMan) );
		}
		else
		{
			pEffectCur->Render ( pframeCur, m_pd3dDevice, NULL, pLandMan );
		}
	}
}

void DxOctree::DrawFrame_NOEFF ( DxFrame *pframeCur )
{
	DxEffectBase* pEffectCur;
	pEffectCur = pframeCur->pEffectNext;

	for ( ; pEffectCur; pEffectCur = pEffectCur->pLocalFrameEffNext )
	{
		if ( pEffectCur->GetFlag() & _EFF_AFTER )		return;
	}

	if ( pframeCur->pEffect )
	{
		//	Note : 원본 대체 이팩트
		//
		pEffectCur = pframeCur->pEffect;
		while ( pEffectCur )
		{
			if ( pEffectCur->GetTypeID() == DEF_EFFECT_SHADOW )		{}
			else													pEffectCur->Render ( pframeCur, m_pd3dDevice );

			pEffectCur = pEffectCur->pLocalFrameEffNext;
		}
	}
	else
	{
		//	Note : 원본 Render
		//
		if ( pframeCur->pmsMeshs != NULL )
		{
			//	Note : 메쉬 그리기.
			//
			DxMeshes *pmsMeshs = pframeCur->pmsMeshs;
			while ( pmsMeshs != NULL )
			{
				if ( pmsMeshs->m_pDxOctreeMesh )
				{
					pmsMeshs->RenderOctree ( m_pd3dDevice );
				}
				pmsMeshs = pmsMeshs->pMeshNext;
			}
		}
	}
}

void DxOctree::Render ( CLIPVOLUME &cv, CSerialFile& SFile, LPDIRECT3DDEVICEQ pd3dDevice, PDXEFFECTBASE &pEffGlobalHead, DxLandMan* pLandMan )
{
	//	Note	:	프리즘에 걸려서 찍히는 거 설정할 부분
	if ( BoxIsInTheFrustum(cv) )
	{
		if ( !m_bLoaded )				DynamicLoad ( SFile, pd3dDevice, pEffGlobalHead );
		
		if ( m_bSubDivided )
		{
			if ( m_pOctreeNodes_0 )		m_pOctreeNodes_0->Render ( cv, SFile, pd3dDevice, pEffGlobalHead, pLandMan);
			if ( m_pOctreeNodes_1 )		m_pOctreeNodes_1->Render ( cv, SFile, pd3dDevice, pEffGlobalHead, pLandMan);
			if ( m_pOctreeNodes_2 )		m_pOctreeNodes_2->Render ( cv, SFile, pd3dDevice, pEffGlobalHead, pLandMan);
			if ( m_pOctreeNodes_3 )		m_pOctreeNodes_3->Render ( cv, SFile, pd3dDevice, pEffGlobalHead, pLandMan);
			if ( m_pOctreeNodes_4 )		m_pOctreeNodes_4->Render ( cv, SFile, pd3dDevice, pEffGlobalHead, pLandMan);
			if ( m_pOctreeNodes_5 )		m_pOctreeNodes_5->Render ( cv, SFile, pd3dDevice, pEffGlobalHead, pLandMan);
			if ( m_pOctreeNodes_6 )		m_pOctreeNodes_6->Render ( cv, SFile, pd3dDevice, pEffGlobalHead, pLandMan);
			if ( m_pOctreeNodes_7 )		m_pOctreeNodes_7->Render ( cv, SFile, pd3dDevice, pEffGlobalHead, pLandMan);
		}
		else
		{
			DxFrame	*pDxFrameCur = m_pDxFrameHead;
			if ( pDxFrameCur )						OctreeDebugInfo::TotalNodesDrawn++;
			if ( g_bDebugLine && pDxFrameCur )		RenderDebugBox();


			while ( pDxFrameCur )
			{
				DrawFrame ( pDxFrameCur, pLandMan );

				OctreeDebugInfo::TodalDxFrameDrawn++;
				pDxFrameCur = pDxFrameCur->pframeSibling;
			}
		}
	}
}

void DxOctree::Render_NOEFF ( CLIPVOLUME &cv, CSerialFile& SFile, LPDIRECT3DDEVICEQ pd3dDevice, PDXEFFECTBASE &pEffGlobalHead )
{
	//	Note	:	프리즘에 걸려서 찍히는 거 설정할 부분
	if ( !BoxIsInTheFrustum(cv) ) return;

	if ( !m_bLoaded )
	{
		DynamicLoad ( SFile, pd3dDevice, pEffGlobalHead );
	}
	
	if(m_bSubDivided)
	{
		if ( m_pOctreeNodes_0 )		m_pOctreeNodes_0->Render_NOEFF(cv, SFile, pd3dDevice, pEffGlobalHead);
		if ( m_pOctreeNodes_1 )		m_pOctreeNodes_1->Render_NOEFF(cv, SFile, pd3dDevice, pEffGlobalHead);
		if ( m_pOctreeNodes_2 )		m_pOctreeNodes_2->Render_NOEFF(cv, SFile, pd3dDevice, pEffGlobalHead);
		if ( m_pOctreeNodes_3 )		m_pOctreeNodes_3->Render_NOEFF(cv, SFile, pd3dDevice, pEffGlobalHead);
		if ( m_pOctreeNodes_4 )		m_pOctreeNodes_4->Render_NOEFF(cv, SFile, pd3dDevice, pEffGlobalHead);
		if ( m_pOctreeNodes_5 )		m_pOctreeNodes_5->Render_NOEFF(cv, SFile, pd3dDevice, pEffGlobalHead);
		if ( m_pOctreeNodes_6 )		m_pOctreeNodes_6->Render_NOEFF(cv, SFile, pd3dDevice, pEffGlobalHead);
		if ( m_pOctreeNodes_7 )		m_pOctreeNodes_7->Render_NOEFF(cv, SFile, pd3dDevice, pEffGlobalHead);					
	}
	else
	{
		DxFrame	*pDxFrameCur = m_pDxFrameHead;
		if ( pDxFrameCur )						OctreeDebugInfo::TotalNodesDrawn++;
		if ( g_bDebugLine && pDxFrameCur )		RenderDebugBox();

		while ( pDxFrameCur )
		{
			DrawFrame_NOEFF ( pDxFrameCur );

			OctreeDebugInfo::TodalDxFrameDrawn++;
			pDxFrameCur = pDxFrameCur->pframeSibling;
		}
	}
}

void DxOctree::RenderAlpha ( CLIPVOLUME &cv, BOOL bShadow )
{
	//	Note	:	프리즘에 걸려서 찍히는 거 설정할 부분
	if(!BoxIsInTheFrustum(cv)) return;
	
	if(m_bSubDivided)
	{
		if ( m_pOctreeNodes_0 )		m_pOctreeNodes_0->RenderAlpha (cv,bShadow);
		if ( m_pOctreeNodes_1 )		m_pOctreeNodes_1->RenderAlpha (cv,bShadow);
		if ( m_pOctreeNodes_2 )		m_pOctreeNodes_2->RenderAlpha (cv,bShadow);
		if ( m_pOctreeNodes_3 )		m_pOctreeNodes_3->RenderAlpha (cv,bShadow);
		if ( m_pOctreeNodes_4 )		m_pOctreeNodes_4->RenderAlpha (cv,bShadow);
		if ( m_pOctreeNodes_5 )		m_pOctreeNodes_5->RenderAlpha (cv,bShadow);
		if ( m_pOctreeNodes_6 )		m_pOctreeNodes_6->RenderAlpha (cv,bShadow);
		if ( m_pOctreeNodes_7 )		m_pOctreeNodes_7->RenderAlpha (cv,bShadow);
	}
	else
	{
		DxMeshes *pmsMeshs;
		DxFrame	*pDxFrameCur = m_pDxAlphaMapFrameHead;

		if ( pDxFrameCur )
		{
			OctreeDebugInfo::TotalNodesDrawn++;
		}
		if ( g_bDebugLine && pDxFrameCur )
		{
			RenderDebugBox();
		}

		while ( pDxFrameCur )
		{
			if ( pDxFrameCur->pEffect )	{}
			else
			{
				//	Note : 메쉬 그리기.
				//
				pmsMeshs = pDxFrameCur->pmsMeshs;
				while ( pmsMeshs != NULL )
				{
					if ( pmsMeshs->m_pDxOctreeMesh )
					{
						pmsMeshs->RenderOctreeAlphaMap ( m_pd3dDevice, bShadow );
					}
					pmsMeshs = pmsMeshs->pMeshNext;
				}
			}

			OctreeDebugInfo::TodalDxFrameDrawn++;
			pDxFrameCur = pDxFrameCur->pNextAlphaMapFrm;
		}
	}
}

BOOL	DxOctree::RenderDebugBox(BOOL	bIsAnimationBox)
{
	if ( bIsAnimationBox )
	{
		D3DMATERIALQ mtrl;
		D3DUtil_InitMaterial( mtrl, 0.0f, 0.0f, 1.0f, 1.f );
		m_pd3dDevice->SetMaterial( &mtrl );
		m_pd3dDevice->SetTexture ( 0, NULL );
	}
	else
	{
		D3DMATERIALQ mtrl;
		D3DUtil_InitMaterial( mtrl, 1.0f, 0.0f, 1.0f, 1.f );
		m_pd3dDevice->SetMaterial( &mtrl );
		m_pd3dDevice->SetTexture ( 0, NULL );
	}

	EDITMESHS::RENDERAABB ( m_pd3dDevice, m_vMax, m_vMin );
	
	return TRUE;
}

void DxOctree::SetLoadState ( BOOL bLoaded )
{
	m_bLoaded = bLoaded;
}



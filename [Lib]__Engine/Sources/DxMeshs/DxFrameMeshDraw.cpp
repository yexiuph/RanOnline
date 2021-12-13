#include "pch.h"

#include "./DxMethods.h"
#include "./DxViewPort.h"
#include "./DxEnvironment.h"

#include "./DxTextureEffMan.h"
#include "./DxEffectFrame.h"
#include "./Collision.h"
#include "dxreplacecontainer.h"
#include "DxAnimationManager.h"
#include "./editmeshs.h"
#include "./DxLandMan.h"

#include "./DxRenderStates.h"
#include "DxFrameMesh.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void DxFrameMesh::SetAnimationTimeZero()
{
	//	Note : 에니메이션 업데이트.
	DxAnimation* pAnimCur = pAnimHead;
	while ( pAnimCur != NULL )
	{
		pAnimCur->SetTime( 0.f );		// Note : Edit Mode
		pAnimCur = pAnimCur->pAnimNext;
	}

	// Note : Matrix 기본 값으로... 
	D3DXMATRIXA16 mCur;
	D3DXMatrixIdentity ( &mCur );
	UpdateFrames( m_pDxFrame, mCur );
}

HRESULT DxFrameMesh::FrameMovePiece ( float fTime, float fElapsedTime )
{
	HRESULT hr;

	//	Note : 에니메이션 업데이트.
	//
	DxAnimation* pAnimCur = pAnimHead;
	while ( pAnimCur != NULL )
	{
		pAnimCur->SetTime ( fCurTime );
		pAnimCur = pAnimCur->pAnimNext;
	}

	//	Note : 효과 업데이트.
	//
	DxEffectBase* pEffCur = m_pEffectHead;
	while ( pEffCur != NULL )
	{
		hr = pEffCur->FrameMove ( fTime, fElapsedTime );
		if ( FAILED(hr) )	return hr;

		pEffCur = pEffCur->pEffectNext;
	}

	return S_OK;
}

HRESULT DxFrameMesh::FrameMove ( float fTime, float fElapsedTime )
{
	m_fTime = fTime;
	m_fElapsedTime = fElapsedTime,

	//	Note : 현제 시간 업데이트.
	fCurTime += fElapsedTime * UNITANIKEY_PERSEC;
	if ( fCurTime > 1.0e15f )	fCurTime = 0;

	//	Note : 에니메이션 업데이트.
	DxAnimation* pAnimCur = pAnimHead;
	while ( pAnimCur != NULL )
	{
		pAnimCur->SetTime( fCurTime );
		pAnimCur = pAnimCur->pAnimNext;
	}

	//	Note : 효과 업데이트.
	//
	DxEffectBase* pEffCur = m_pEffectHead;
	while ( pEffCur != NULL )
	{
		pEffCur->FrameMove ( fTime, fElapsedTime );
		pEffCur = pEffCur->pEffectNext;
	}

	if( m_pTextureEff )	m_pTextureEff->FrameMove( fElapsedTime );
	
	return S_OK;
}

HRESULT DxFrameMesh::UpdateFrames ( DxFrame *pframeCur, D3DXMATRIX &matCur )
{
    HRESULT hr = S_OK;
    
	//	Note : pframeCur의 메트릭스를 계산. matCombined = matRot * matParent
    D3DXMatrixMultiply ( &pframeCur->matCombined, &pframeCur->matRot, &matCur );

	//	Note : 자식 프레임의 메트릭스 모두 계산.
	//
	DxFrame *pframeChild = pframeCur->pframeFirstChild;
    while ( pframeChild != NULL )
    {
        hr = UpdateFrames ( pframeChild, pframeCur->matCombined );
        if (FAILED(hr))
            return hr;

        pframeChild = pframeChild->pframeSibling;
    }

    return S_OK;
}

HRESULT DxFrameMesh::Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX *pWorld, CLIPVOLUME *pCV  )
{
	if ( !m_pDxFrame )	return S_OK;

	HRESULT hr;

	//	Note : 렌더 지점.
	//
	if ( pWorld )
	{
		m_pDxFrame->matRot = *pWorld;
	}
	else
	{
		D3DXMATRIX Mat;
		D3DXMatrixIdentity ( &Mat );
		m_pDxFrame->matRot = Mat;
	}

	//	Note : 메트릭스 계산.
	//
	D3DXMATRIXA16 mCur;
	D3DXMatrixIdentity ( &mCur );

	hr = UpdateFrames ( m_pDxFrame, mCur );
	if (FAILED(hr))		return hr;

	//	Note : 메쉬 그리기 수행.
	//
	hr = DrawFrames ( pd3dDevice, m_pDxFrame, pCV );
	if (FAILED(hr))		return hr;

	//	Note : Alpha 메쉬 그리기 수행.
	//
	hr = DrawAlphaMapFrames ( pd3dDevice, pCV );
	if (FAILED(hr))		return hr;

	return S_OK;
}

HRESULT DxFrameMesh::Render_EFF ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX *pWorld, CLIPVOLUME *pCV  )
{
	if ( !m_pDxFrame )	return S_OK;

	HRESULT hr;

	//	Note : 치환 조각 그리기 수행.
	float fDistance = DxViewPort::GetInstance().GetDistance();
	fDistance = fDistance*fDistance;
	DxReplaceContainer::EMREFLECTOPTION emReflect = DxReplaceContainer::EM_NULL;
	hr = DxReplaceContainer::DrawRePlacePiece( pd3dDevice, m_pPieceObjHead, m_pPieceObjTree, pCV, emReflect, fDistance );
	if (FAILED(hr))		return hr;

	return S_OK;
}

HRESULT DxFrameMesh::Render_NOEFF ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV  )
{
	HRESULT	hr;

	if ( !m_pDxFrame )	return S_OK;

	//	Note : 메쉬 그리기 수행.
	//
	hr = DrawFrames_NOEFF ( pd3dDevice, m_pDxFrame, pCV );
	if (FAILED(hr))		return hr;

	//	Note : Alpha 메쉬 그리기 수행.
	//
	hr = DrawAlphaMapFrames ( pd3dDevice, pCV );
	if (FAILED(hr))		return hr;

	//	Note : 치환 조각 그리기 수행.
	float fDistance = DxViewPort::GetInstance().GetDistance();
	fDistance = fDistance*fDistance;
	hr = DxReplaceContainer::DrawRePlacePiece ( pd3dDevice, m_pPieceObjHead, m_pPieceObjTree, pCV, DxReplaceContainer::EM_REFLECT_OLD, fDistance );
	if (FAILED(hr))		return hr;

	return S_OK;
}

HRESULT DxFrameMesh::Render_Reflect ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV  )
{
	HRESULT	hr;

	if ( !m_pDxFrame )	return S_OK;

	//	Note : 메쉬 그리기 수행.
	//
	hr = DrawFrames_Reflect ( pd3dDevice, m_pDxFrame, pCV );
	if (FAILED(hr))		return hr;

	//	Note : Alpha 메쉬 그리기 수행.
	//
	hr = DrawAlphaMapFrames ( pd3dDevice, pCV, TRUE );
	if (FAILED(hr))		return hr;

	//	Note : 치환 조각 그리기 수행.
	float fDistance = DxViewPort::GetInstance().GetDistance();
	fDistance = fDistance*fDistance;
	hr = DxReplaceContainer::DrawRePlacePiece( pd3dDevice, m_pPieceObjHead, m_pPieceObjTree, pCV, DxReplaceContainer::EM_REFLECT, fDistance );
	if (FAILED(hr))		return hr;

	return S_OK;
}

HRESULT DxMeshes::Render( LPDIRECT3DDEVICEQ pd3dDevice )
{
	for ( DWORD i=0; i<cMaterials; i++ )
	{
		if ( exMaterials[i].MtrlType != MATERIAL_NORMAL )		continue;

		//	Note : 메터리얼, 텍스쳐 지정.
		pd3dDevice->SetMaterial( &rgMaterials[i] );
		pd3dDevice->SetTexture( 0, pTextures[i] );

		m_pLocalMesh->DrawSubset( i );
	}

	return S_OK;
}

HRESULT DxMeshes::RenderAlpha ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bShadow )
{
	for ( DWORD i=0; i<cMaterials; i++ )
	{
		DXMTRLTYPE emMETR = exMaterials[i].MtrlType;
		if ( emMETR==MATERIAL_NORMAL )			continue;

		if ( !bShadow )
		{
			switch ( emMETR )
			{
			case MATERIAL_ALPHA:
				DxRenderStates::GetInstance().SetAlphaLand( pd3dDevice );
				break;
			case MATERIAL_ALPHA_NOZWRI:
				DxRenderStates::GetInstance().SetAlphaNoZWriLand( pd3dDevice );
				break;
			case MATERIAL_ALPHA_NOTEST:
				DxRenderStates::GetInstance().SetAlphaNoTestLand( pd3dDevice );
				break;
			case MATERIAL_ALPHA_HARD:
				DxRenderStates::GetInstance().SetOnAlphaMap( pd3dDevice );
				break;
			case MATERIAL_ALPHA_SOFT:
				DxRenderStates::GetInstance().SetOnSoftAlphaMap( pd3dDevice );
				break;
			};
		}

		//	Note : 메터리얼, 텍스쳐 지정.
		pd3dDevice->SetMaterial( &rgMaterials[i] );
		pd3dDevice->SetTexture( 0, pTextures[i] );
	
		m_pLocalMesh->DrawSubset( i );

		if ( !bShadow )
		{
			switch ( emMETR )
			{
			case MATERIAL_ALPHA:
				DxRenderStates::GetInstance().ReSetAlphaLand( pd3dDevice );
				break;
			case MATERIAL_ALPHA_NOZWRI:
				DxRenderStates::GetInstance().ReSetAlphaNoZWriLand( pd3dDevice );
				break;
			case MATERIAL_ALPHA_NOTEST:
				DxRenderStates::GetInstance().ReSetAlphaNoTestLand( pd3dDevice );
				break;
			case MATERIAL_ALPHA_HARD:
				DxRenderStates::GetInstance().ReSetOnAlphaMap( pd3dDevice );
				break;
			case MATERIAL_ALPHA_SOFT:
				DxRenderStates::GetInstance().ReSetOnSoftAlphaMap( pd3dDevice );
				break;
			};
		}
	}

	return S_OK;
}

HRESULT DxMeshes::RenderTriangle ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD OldFillMode;
	pd3dDevice->GetRenderState(D3DRS_FILLMODE, &OldFillMode );
	pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME );

	DWORD OldLighting;
	pd3dDevice->GetRenderState ( D3DRS_LIGHTING, &OldLighting );
	pd3dDevice->SetRenderState ( D3DRS_LIGHTING, FALSE );

	//	Note : 메터리얼, 텍스쳐 지정.
	//
	D3DMATERIALQ WhiteMaterial;
	memset(&WhiteMaterial, 0, sizeof(D3DMATERIALQ));
	WhiteMaterial.Diffuse.r = 1.0f;
	WhiteMaterial.Diffuse.g = 1.0f;
	WhiteMaterial.Diffuse.b = 1.0f;
	WhiteMaterial.Specular = WhiteMaterial.Diffuse;

	pd3dDevice->SetMaterial( &WhiteMaterial );
	pd3dDevice->SetTexture( 0, NULL );
	
	for ( DWORD i=0; i<cMaterials; i++ )
	{
		m_pLocalMesh->DrawSubset( i );
	}

	pd3dDevice->SetRenderState(D3DRS_FILLMODE, OldFillMode );
	pd3dDevice->SetRenderState ( D3DRS_LIGHTING, OldLighting );

	{
		DWORD OldFillMode;
		pd3dDevice->GetRenderState(D3DRS_FILLMODE, &OldFillMode );
		pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME );

		DWORD dwD3dColorOld;
		pd3dDevice->GetRenderState( D3DRS_AMBIENT, &dwD3dColorOld );

		DWORD dwD3dColor = D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,0);
		pd3dDevice->SetRenderState( D3DRS_AMBIENT, dwD3dColor );

		D3DLIGHTQ light, lightOld;
		pd3dDevice->GetLight ( 0, &lightOld );

		D3DUtil_InitLight ( light, D3DLIGHT_DIRECTIONAL, -1.0f, -1.0f, 2.0f );
		pd3dDevice->SetLight ( 0, &light );

		//	Note : 메터리얼, 텍스쳐 지정.
		//
		D3DMATERIALQ RedMaterial;
		memset(&RedMaterial, 0, sizeof(D3DMATERIALQ));
		RedMaterial.Diffuse.r = 1.0f;
		RedMaterial.Diffuse.g = 0.0f;
		RedMaterial.Diffuse.b = 0.0f;
		RedMaterial.Specular = RedMaterial.Diffuse;

		pd3dDevice->SetMaterial( &RedMaterial );
		pd3dDevice->SetTexture( 0, NULL );

		for ( DWORD i=0; i<cMaterials; i++ )
		{
			if ( exMaterials[i].bSubSetTriRend )
				m_pLocalMesh->DrawSubset( i );
		}

		pd3dDevice->SetRenderState(D3DRS_FILLMODE, OldFillMode );
		pd3dDevice->SetRenderState( D3DRS_AMBIENT, dwD3dColorOld );
		pd3dDevice->SetLight ( 0, &lightOld );
	}

	return S_OK;
}

HRESULT DxFrameMesh::DrawFrames ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeCur, CLIPVOLUME *pCV )
{
	HRESULT hr = S_OK;
	DxMeshes *pmsMeshs;
	DxFrame *pframeChild;

	//	Note : 치환 프레임 지정시.
	if( pframeCur->pRePlacePiece )	return S_OK;
	if( pframeCur->bPieceUSE )		return S_OK;

	//	Note : 내비게이션 메시 그리지 않음 검사
	if ( !m_bNaviDraw && (pframeCur->bFlag&DXFRM_NAVIFRAME) )	return S_OK;

	//	Note : 
	//
	if ( !pframeCur->BoxIsInTheFrustum ( &m_pDxFrame->matRot, *pCV, pd3dDevice ) )	return S_OK;

	//	Note : 원본 Render 전 이팩트
	//
	DxEffectBase* pEffectCur;
	pEffectCur = pframeCur->pEffectPrev;
	while ( pEffectCur )
	{
		pEffectCur->Render ( pframeCur, pd3dDevice );
		pEffectCur = pEffectCur->pLocalFrameEffNext;
	}

	if ( pframeCur->pEffect )
	{
		//	Note : 원본 대체 이팩트
		//
		pEffectCur = pframeCur->pEffect;
		while ( pEffectCur )
		{
			pEffectCur->Render ( pframeCur, pd3dDevice, this );
			pEffectCur = pEffectCur->pLocalFrameEffNext;
		}
	}
	else
	{
		//	Note : 원본 Render
		//
		if ( pframeCur->pmsMeshs != NULL )
		{
			//	Note : 트렌스폼 설정.
			//
			hr = pd3dDevice->SetTransform ( D3DTS_WORLD, &pframeCur->matCombined );
			if (FAILED(hr))
				return hr;

			//	Note : 메쉬 그리기.
			//
			pmsMeshs = pframeCur->pmsMeshs;
			while ( pmsMeshs != NULL )
			{
				if ( pmsMeshs->m_pLocalMesh )
				{
					if( m_pTextureEff )	m_pTextureEff->Render( pd3dDevice, pframeCur->pmsMeshs );
					else				pmsMeshs->Render( pd3dDevice );
				}

				pmsMeshs = pmsMeshs->pMeshNext;
			}
		}

		//	Note : 자식 프레임 그리기.
		//
		pframeChild = pframeCur->pframeFirstChild;
		while ( pframeChild != NULL )
		{
			hr = DrawFrames ( pd3dDevice, pframeChild, pCV );
			if (FAILED(hr))
				return hr;

			pframeChild = pframeChild->pframeSibling;
		}
	}

	//	Note : 원본 Render 후 이팩트
	//
	pEffectCur = pframeCur->pEffectNext;
	while ( pEffectCur )
	{
		pEffectCur->Render ( pframeCur, pd3dDevice, this );
		pEffectCur = pEffectCur->pLocalFrameEffNext;
	}

	return S_OK;
}

HRESULT DxFrameMesh::DrawFrames_NOEFF ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeCur, CLIPVOLUME *pCV )
{
	HRESULT hr = S_OK;
	DxMeshes *pmsMeshs;
	DxFrame *pframeChild;

	//	Note : 치환 프레임 지정시.
	if( pframeCur->pRePlacePiece )	return S_OK;
	if( pframeCur->bPieceUSE )		return S_OK;

	//	Note : 내비게이션 메시 그리지 않음 검사
	if ( pframeCur->bFlag&DXFRM_NAVIFRAME )	return S_OK;

	if ( pframeCur->pEffect )	{}
	else
	{
		if ( pframeCur->pmsMeshs != NULL )
		{
			//	Note : 트렌스폼 설정.
			//
			hr = pd3dDevice->SetTransform ( D3DTS_WORLD, &pframeCur->matCombined );
			if (FAILED(hr))
				return hr;

			//	Note : 메쉬 그리기.
			//
			pmsMeshs = pframeCur->pmsMeshs;
			while ( pmsMeshs != NULL )
			{
				if ( pmsMeshs->m_pLocalMesh )
				{
					if( m_pTextureEff )	m_pTextureEff->Render( pd3dDevice, pframeCur->pmsMeshs );
					else				pmsMeshs->Render( pd3dDevice );
				}

				pmsMeshs = pmsMeshs->pMeshNext;
			}
		}

		//	Note : 자식 프레임 그리기.
		//
		pframeChild = pframeCur->pframeFirstChild;
		while ( pframeChild != NULL )
		{
			hr = DrawFrames_NOEFF ( pd3dDevice, pframeChild, pCV );
			if (FAILED(hr))
				return hr;

			pframeChild = pframeChild->pframeSibling;
		}
	}

	return S_OK;
}

HRESULT DxFrameMesh::DrawFrames_Reflect ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeCur, CLIPVOLUME *pCV )
{
	HRESULT hr = S_OK;
	DxMeshes *pmsMeshs;
	DxFrame *pframeChild;

	//	Note : 치환 프레임 지정시.
	if( pframeCur->pRePlacePiece )	return S_OK;
	if( pframeCur->bPieceUSE )		return S_OK;

	//	Note : 내비게이션 메시 그리지 않음 검사
	if ( pframeCur->bFlag&DXFRM_NAVIFRAME )	return S_OK;

	if ( pframeCur->pEffect )	{}
	else
	{
		if ( pframeCur->pmsMeshs != NULL )
		{
			D3DXVECTOR3 vCenter = DxEnvironment::GetInstance().GetCenter();

			D3DXMATRIX matWorld, matScale;
			matWorld = pframeCur->matCombined;
			matWorld._42 -= (vCenter.y*2.f);
			D3DXMatrixMultiply_MIRROR( matWorld );

			DWORD	dwCullMode;
			pd3dDevice->GetRenderState ( D3DRS_CULLMODE, &dwCullMode );
			pd3dDevice->SetRenderState ( D3DRS_CULLMODE, D3DCULL_CW );

			//	Note : 트렌스폼 설정.
			//
			hr = pd3dDevice->SetTransform ( D3DTS_WORLD, &matWorld );
			if (FAILED(hr))
				return hr;

			//	Note : 메쉬 그리기.
			//
			pmsMeshs = pframeCur->pmsMeshs;
			while ( pmsMeshs != NULL )
			{
				if ( pmsMeshs->m_pLocalMesh )
				{
					if( m_pTextureEff )	m_pTextureEff->Render( pd3dDevice, pframeCur->pmsMeshs );
					else				pmsMeshs->Render( pd3dDevice );
				}

				pmsMeshs = pmsMeshs->pMeshNext;
			}

			pd3dDevice->SetRenderState ( D3DRS_CULLMODE, dwCullMode );
		}

		//	Note : 자식 프레임 그리기.
		//
		pframeChild = pframeCur->pframeFirstChild;
		while ( pframeChild != NULL )
		{
			hr = DrawFrames_Reflect ( pd3dDevice, pframeChild, pCV );
			if (FAILED(hr))
				return hr;

			pframeChild = pframeChild->pframeSibling;
		}
	}

	return S_OK;
}

HRESULT DxFrameMesh::DrawAlphaMapFrames ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV, BOOL bRefelct )
{
	HRESULT hr = S_OK;

	DxMeshes* pmsMeshs;
	DxFrame *pframeCur = m_pDxAlphaMapFrameHead;
	while ( pframeCur )
	{
		// Note : 프레임이 Piece일 경우 뿌리지 않음.
		if( pframeCur->pRePlacePiece || pframeCur->bPieceUSE )
		{
			pframeCur = pframeCur->pNextAlphaMapFrm;
			continue;
		}

		//// Note : 프레임 이름의  [Mesh]를 뺀 이름으로 프레임을 찾은 후 Piece일 경우 뿌리지 않음.
		//std::string strFrameName = pframeCur->szName;
		//UINT nSize = strFrameName.size();
		//nSize = nSize - 6;
		//strFrameName.resize( nSize );
		//DxFrame *pFindFrame = FindFrame( strFrameName.c_str() );
		//if( pFindFrame )
		//{
		//	if( pFindFrame->pRePlacePiece || pFindFrame->bPieceUSE )
		//	{
		//		pframeCur = pframeCur->pNextAlphaMapFrm;
		//		continue;
		//	}
		//}

		//	Note : 문제점 노출... Render ( , D3DXMATRIX *pWorld, ) 문제 때문인듯.
		//
		if ( !pframeCur->BoxIsInTheFrustum ( &m_pDxFrame->matRot, *pCV, pd3dDevice ) )
		{
			pframeCur = pframeCur->pNextAlphaMapFrm;
			continue;
		}

		//	Note : 원본 Render
		//
		if ( pframeCur->pEffect )	{}
		else if ( pframeCur->pmsMeshs != NULL )
		{
			DWORD	dwCullMode;
			D3DXMATRIX matWorld;
			if ( bRefelct )
			{
				D3DXVECTOR3 vCenter = DxEnvironment::GetInstance().GetCenter();

				D3DXMATRIX matScale;
				matWorld = pframeCur->matCombined;
				matWorld._42 -= (vCenter.y*2.f);
				D3DXMatrixMultiply_MIRROR( matWorld );
				
				pd3dDevice->GetRenderState ( D3DRS_CULLMODE, &dwCullMode );
				pd3dDevice->SetRenderState ( D3DRS_CULLMODE, D3DCULL_CW );
			}
			else
			{
				matWorld = pframeCur->matCombined;
			}

			hr = pd3dDevice->SetTransform ( D3DTS_WORLD, &pframeCur->matCombined );
			if (FAILED(hr))
				return hr;

			//	Note : 메쉬 그리기.
			//
			pmsMeshs = pframeCur->pmsMeshs;
			while ( pmsMeshs != NULL )
			{
				if ( pmsMeshs->m_pLocalMesh )
				{
					//pmsMeshs->pmsMeshs->Render( pd3dDevice ); ( pd3dDevice );
					if( m_pTextureEff )	m_pTextureEff->RenderAlpha( pd3dDevice, pframeCur->pmsMeshs, FALSE );
					else				pmsMeshs->RenderAlpha( pd3dDevice, FALSE );
				}

				pmsMeshs = pmsMeshs->pMeshNext;
			}

			if ( bRefelct )
			{
				pd3dDevice->SetRenderState ( D3DRS_CULLMODE, dwCullMode );
			}
		}

		pframeCur = pframeCur->pNextAlphaMapFrm;
	}

	//	Note : 자식프레임은 그리지 않는다.
	//		- 프레임 안의 DxMesh 에 Alpha 설정이 들어 있으므로...

	return S_OK;
}

HRESULT DxFrameMesh::DrawSelectedFrame ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV )
{
	HRESULT hr;
	if ( !m_pCollDetectFrame )	return S_OK;

	DWORD dwZFunc;
	pd3dDevice->GetRenderState( D3DRS_ZFUNC,	&dwZFunc );

	//	Note : 문제점 노출... Render ( , D3DXMATRIX *pWorld, ) 문제 때문인듯.
	//
	if ( !m_pCollDetectFrame->BoxIsInTheFrustum ( &m_pDxFrame->matRot, *pCV, pd3dDevice ) )	return S_OK;

	//	Note : 원본 Render
	//
	if ( m_pCollDetectFrame->pmsMeshs != NULL )
	{
		//	Note : 트렌스폼 설정.
		//
		hr = pd3dDevice->SetTransform ( D3DTS_WORLD, &m_pCollDetectFrame->matCombined );
		if (FAILED(hr))
			return hr;

		//	Note : 메쉬 그리기.
		//
		DxMeshes *pmsMeshs = m_pCollDetectFrame->pmsMeshs;
		while ( pmsMeshs != NULL )
		{
			if ( pmsMeshs->m_pLocalMesh )
			{
				pmsMeshs->RenderTriangle ( pd3dDevice );
			}

			pmsMeshs = pmsMeshs->pMeshNext;
		}
	}

	pd3dDevice->SetRenderState( D3DRS_ZFUNC,	dwZFunc );

	return S_OK;
}

HRESULT DxFrameMesh::DrawSelectedPoint ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 vVert )
{
	//	-----------------------------[테스트용]--------------------------------
	//
	static DWORD BOXVERTEX = D3DFVF_XYZ;

	DWORD OldFillMode;
	pd3dDevice->GetRenderState(D3DRS_FILLMODE, &OldFillMode );
	pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME );

	DWORD OldLighting;
	pd3dDevice->GetRenderState ( D3DRS_LIGHTING, &OldLighting );
	pd3dDevice->SetRenderState ( D3DRS_LIGHTING, FALSE );

	//	Note : 메터리얼, 텍스쳐 지정.
	//
	D3DMATERIALQ WhiteMaterial;
	memset(&WhiteMaterial, 0, sizeof(D3DMATERIALQ));
	WhiteMaterial.Diffuse.r = 1.0f;
	WhiteMaterial.Diffuse.g = 1.0f;
	WhiteMaterial.Diffuse.b = 1.0f;
	WhiteMaterial.Specular = WhiteMaterial.Diffuse;

	pd3dDevice->SetMaterial( &WhiteMaterial );
	pd3dDevice->SetTexture( 0, NULL );

	//	Note : Set the world matrix
	//
	D3DXMATRIX matWorld;
	D3DXMatrixTranslation ( &matWorld, vVert.x, vVert.y, vVert.z );

	pd3dDevice->SetTransform( D3DTS_WORLD,  &matWorld );

	pd3dDevice->SetFVF(BOXVERTEX);
	pd3dDevice->DrawIndexedPrimitiveUP ( D3DPT_TRIANGLELIST, 0, EDITMESHS::dwNumSphereVertex, EDITMESHS::dwNumSphereFaces,
		EDITMESHS::SphereIndex, D3DFMT_INDEX16, EDITMESHS::SphereVertex, D3DXGetFVFVertexSize(BOXVERTEX) );

	pd3dDevice->SetRenderState(D3DRS_FILLMODE, OldFillMode );
	pd3dDevice->SetRenderState ( D3DRS_LIGHTING, OldLighting );

	return S_OK;
}

HRESULT	DxMeshes::RenderOctree( LPDIRECT3DDEVICEQ pd3dDevice, const float fAlpha )
{
	OctreeDebugInfo::FaceViewCount += m_pDxOctreeMesh->m_dwNumFaces;
	OctreeDebugInfo::VertexViewCount += m_pDxOctreeMesh->m_dwNumVertices;

	HRESULT hr;
	hr = pd3dDevice->SetStreamSource ( 0, m_pDxOctreeMesh->m_pVB, 0, D3DXGetFVFVertexSize(m_pDxOctreeMesh->m_dwFVF) );
	if ( FAILED ( hr ) ) return E_FAIL;
	
	hr = pd3dDevice->SetFVF ( m_pDxOctreeMesh->m_dwFVF );
	if ( FAILED ( hr ) ) return E_FAIL;

	hr = pd3dDevice->SetIndices ( m_pDxOctreeMesh->m_pIB );
	if ( FAILED ( hr ) ) return E_FAIL;

	D3DMATERIALQ sMaterial;
	for ( DWORD i=0; i<cMaterials; i++ )
	{
		if ( exMaterials[i].MtrlType != MATERIAL_NORMAL )		continue;

		//	Note : 메터리얼, 텍스쳐 지정.
		pd3dDevice->SetTexture( 0, pTextures[i] );

		switch( g_emMeshMeterial )
		{
		case EM_MESH_MATERIAL:
			if( fAlpha == 1.f )
			{
				pd3dDevice->SetMaterial( &rgMaterials[i] );
			}
			else
			{
				sMaterial = rgMaterials[i];
				sMaterial.Diffuse.a = fAlpha;
				sMaterial.Ambient.a = fAlpha;
				sMaterial.Specular.a = fAlpha;
				sMaterial.Emissive.a = fAlpha;
				pd3dDevice->SetMaterial( &sMaterial );
			}
			break;
		case EM_MESH_MATERIAL_RED:
			sMaterial = rgMaterials[i];
			sMaterial.Ambient.r = sMaterial.Diffuse.r = 1.f;
			sMaterial.Ambient.g = sMaterial.Diffuse.g = DxMeshes::g_fAlpha;
			sMaterial.Ambient.b = sMaterial.Diffuse.b = DxMeshes::g_fAlpha;
			pd3dDevice->SetMaterial( &sMaterial );
			break;
		};

		m_pDxOctreeMesh->DrawSubset( i, pd3dDevice );
	}

	return S_OK;
}

HRESULT	DxMeshes::RenderOctreeAlphaMap ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bShadow, const float fAlpha )
{
	OctreeDebugInfo::FaceViewCount += m_pDxOctreeMesh->m_dwNumFaces;
	OctreeDebugInfo::VertexViewCount += m_pDxOctreeMesh->m_dwNumVertices;

	HRESULT hr;
	hr = pd3dDevice->SetStreamSource ( 0, m_pDxOctreeMesh->m_pVB, 0, D3DXGetFVFVertexSize(m_pDxOctreeMesh->m_dwFVF) );
	if ( FAILED ( hr ) ) return E_FAIL;
	
	hr = pd3dDevice->SetFVF ( m_pDxOctreeMesh->m_dwFVF );
	if ( FAILED ( hr ) ) return E_FAIL;

	hr = pd3dDevice->SetIndices ( m_pDxOctreeMesh->m_pIB );
	if ( FAILED ( hr ) ) return E_FAIL;


	D3DMATERIALQ sMaterial;
	for ( DWORD i=0; i<cMaterials; i++ )
	{
		DXMTRLTYPE emMETR = exMaterials[i].MtrlType;
		if ( emMETR == MATERIAL_NORMAL )		continue;

		if ( !bShadow )
		{
			switch ( emMETR )
			{
			case MATERIAL_ALPHA:
				DxRenderStates::GetInstance().SetAlphaLand( pd3dDevice );
				break;
			case MATERIAL_ALPHA_NOZWRI:
				DxRenderStates::GetInstance().SetAlphaNoZWriLand( pd3dDevice );
				break;
			case MATERIAL_ALPHA_NOTEST:
				DxRenderStates::GetInstance().SetAlphaNoTestLand( pd3dDevice );
				break;
			case MATERIAL_ALPHA_HARD:
				DxRenderStates::GetInstance().SetOnAlphaMap( pd3dDevice );
				break;
			case MATERIAL_ALPHA_SOFT:
				DxRenderStates::GetInstance().SetOnSoftAlphaMap( pd3dDevice );
				break;
			default:
				DxRenderStates::GetInstance().SetOnAlphaMap ( pd3dDevice );
			};
		}

		//	Note : 메터리얼, 텍스쳐 지정.
		pd3dDevice->SetTexture( 0, pTextures[i] );

		switch( g_emMeshMeterial )
		{
		case EM_MESH_MATERIAL:
			if( fAlpha == 1.f )
			{
				pd3dDevice->SetMaterial( &rgMaterials[i] );
			}
			else
			{
				sMaterial = rgMaterials[i];
				sMaterial.Diffuse.a = fAlpha;
				sMaterial.Ambient.a = fAlpha;
				sMaterial.Specular.a = fAlpha;
				sMaterial.Emissive.a = fAlpha;
				pd3dDevice->SetMaterial( &sMaterial );
			}
			break;
		case EM_MESH_MATERIAL_RED:
			sMaterial = rgMaterials[i];
			sMaterial.Ambient.r = sMaterial.Diffuse.r = 1.f;
			sMaterial.Ambient.g = sMaterial.Diffuse.g = DxMeshes::g_fAlpha;
			sMaterial.Ambient.b = sMaterial.Diffuse.b = DxMeshes::g_fAlpha;
			pd3dDevice->SetMaterial( &sMaterial );
			break;
		};

		m_pDxOctreeMesh->DrawSubset( i, pd3dDevice );

		if ( !bShadow )
		{
			switch ( emMETR )
			{
			case MATERIAL_ALPHA:
				DxRenderStates::GetInstance().ReSetAlphaLand( pd3dDevice );
				break;
			case MATERIAL_ALPHA_NOZWRI:
				DxRenderStates::GetInstance().ReSetAlphaNoZWriLand( pd3dDevice );
				break;
			case MATERIAL_ALPHA_NOTEST:
				DxRenderStates::GetInstance().ReSetAlphaNoTestLand( pd3dDevice );
				break;
			case MATERIAL_ALPHA_HARD:
				DxRenderStates::GetInstance().ReSetOnAlphaMap( pd3dDevice );
				break;
			case MATERIAL_ALPHA_SOFT:
				DxRenderStates::GetInstance().ReSetOnSoftAlphaMap( pd3dDevice );
				break;
			default:
				DxRenderStates::GetInstance().ReSetOnAlphaMap ( pd3dDevice );
			};
		}
	}

	return S_OK;
}

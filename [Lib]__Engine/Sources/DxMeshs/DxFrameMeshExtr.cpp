#include "pch.h"

#include "./TextureManager.h"
#include "DxReplaceContainer.h"
#include "DxAnimationManager.h"

#include "./DxEffectFrame.h"
#include "DxFrameMesh.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

char D3DEXMATERIAL::szMtrlTypes[MATERIAL_NSIZE][24] = 
{
	"NORMAL",
	"ALPHA",
	"ALPHA-NOZWRI",
	"ALPHA-NOTEST"
};

DWORD DxFrame::dwFrameTypeNum = 2;
char DxFrame::szFrameType[2][24] = 
{
	"COMMON",
	"DOOR"
};

DWORD D3DEXMATERIAL::VERSION = 0x00000100;



BOOL DxFrame::IsAlphaMapMesh ()
{
	DxMeshes *pmsMeshCur;
	pmsMeshCur = pmsMeshs;
	while ( pmsMeshCur )
	{
		if ( pmsMeshCur->exMaterials )
		{
			for ( DWORD i=0; i<pmsMeshCur->cMaterials; i++ )
			{
				if ( pmsMeshCur->exMaterials[i].MtrlType!=MATERIAL_NORMAL )		return TRUE;
			}
		}

		pmsMeshCur = pmsMeshCur->pMeshNext;
	}

	return FALSE;
}

//	Note : 효과 추가.
//
void DxFrameMesh::AddEffect ( DxEffectBase *pEffectAdd )
{
	pEffectAdd->pEffectNext = m_pEffectHead;
	m_pEffectHead = pEffectAdd;
}

//	Note : 효과 삭제.
//
BOOL DxFrameMesh::DelEffect ( DxFrame *pDxFrame, DxEffectBase *pEffectDel )
{
	DxEffectBase *pEffectCur, *pEffectPrev;

	//	Note : 프래임의 이팩트 리스트에서 삭제.
	//

	//	Note : 전, 이팩트
	//
	if ( pDxFrame->pEffectPrev == pEffectDel )
	{
		pDxFrame->pEffectPrev =  pEffectDel->pLocalFrameEffNext;
	}
	else
	{
		pEffectCur = pDxFrame->pEffectPrev;
		while ( pEffectCur )
		{
			if ( pEffectCur == pEffectDel )
			{
				pEffectPrev->pLocalFrameEffNext = pEffectDel->pLocalFrameEffNext;
				break;
			}

			pEffectPrev = pEffectCur;
			pEffectCur = pEffectCur->pLocalFrameEffNext;
		}
	}

	//	Note : 중, 이팩트
	//
	if ( pDxFrame->pEffect == pEffectDel )
	{
		pDxFrame->pEffect =  pEffectDel->pLocalFrameEffNext;
	}
	else
	{
		pEffectCur = pDxFrame->pEffect;
		while ( pEffectCur )
		{
			if ( pEffectCur == pEffectDel )
			{
				pEffectPrev->pLocalFrameEffNext = pEffectDel->pLocalFrameEffNext;
				break;
			}

			pEffectPrev = pEffectCur;
			pEffectCur = pEffectCur->pLocalFrameEffNext;
		}
	}

	//	Note : 후, 이팩트
	//
	if ( pDxFrame->pEffectNext == pEffectDel )
	{
		pDxFrame->pEffectNext =  pEffectDel->pLocalFrameEffNext;
	}
	else
	{
		pEffectCur = pDxFrame->pEffectNext;
		while ( pEffectCur )
		{
			if ( pEffectCur == pEffectDel )
			{
				pEffectPrev->pLocalFrameEffNext = pEffectDel->pLocalFrameEffNext;
				break;
			}

			pEffectPrev = pEffectCur;
			pEffectCur = pEffectCur->pLocalFrameEffNext;
		}
	}

	//	Note : 전체의 이팩트 리스트에서 삭제.
	//
	if ( pEffectDel == m_pEffectHead )
	{
		m_pEffectHead = pEffectDel->pEffectNext;
		pEffectDel->pEffectNext = NULL;
		
		pEffectDel->CleanUp();
		delete pEffectDel;
	}
	else
	{
		pEffectCur = m_pEffectHead;
		while ( pEffectCur )
		{
			if ( pEffectCur == pEffectDel )
			{
				pEffectPrev->pEffectNext = pEffectCur->pEffectNext;
				pEffectCur->pEffectNext = NULL;

				pEffectDel->CleanUp();
				delete pEffectDel;

				return TRUE;
			}

			pEffectPrev = pEffectCur;
			pEffectCur = pEffectCur->pEffectNext;
		}
	}

	//	Note : 만약 효과가 제거되면서 Replace 속정이 제거되면 
	//		알파맵이 존재하는 프레임의 경우 알파맵 리스트에 삽입.
	//
	if ( !pDxFrame->IsReplace () && pDxFrame->IsAlphaMapMesh () )
	{
		AddAlphaMapFrame ( pDxFrame );
	}

	return FALSE;
}

void DxFrameMesh::AddAlphaMapFrameAll ( DxFrame *pDxFrame )
{
	if ( pDxFrame->IsAlphaMapMesh () )		AddAlphaMapFrame(pDxFrame);

	DxFrame *pFrameChildCur = pDxFrame->pframeFirstChild;
	for ( ; pFrameChildCur; pFrameChildCur=pFrameChildCur->pframeSibling )
	{
		AddAlphaMapFrameAll ( pFrameChildCur );
	}
}

void DxFrameMesh::DelAlphaMapFrameAll ( DxFrame *pDxFrame )
{
	if ( pDxFrame->IsAlphaMapMesh () )		DelAlphaMapFrame(pDxFrame);

	DxFrame *pFrameChildCur = pDxFrame->pframeFirstChild;
	for ( ; pFrameChildCur; pFrameChildCur=pFrameChildCur->pframeSibling )
	{
		DelAlphaMapFrameAll ( pFrameChildCur );
	}
}

void DxFrameMesh::AddAlphaMapFrame ( DxFrame *pDxFrameAdd )
{
	GASSERT(pDxFrameAdd);

	//	Note :등록 리스트를 검색후 있으면 무시 / 없으면 등록.
	//
	DxFrame* pDxFrameCur = m_pDxAlphaMapFrameHead;
	while ( pDxFrameCur )
	{
		if ( pDxFrameCur == pDxFrameAdd )	return;
		pDxFrameCur = pDxFrameCur->pNextAlphaMapFrm;
	}

	pDxFrameAdd->pNextAlphaMapFrm = m_pDxAlphaMapFrameHead;
	m_pDxAlphaMapFrameHead = pDxFrameAdd;
}

void DxFrameMesh::DelAlphaMapFrame ( DxFrame *pDxFrameDel )
{
	GASSERT(pDxFrameDel);

	if ( m_pDxAlphaMapFrameHead == pDxFrameDel )
	{
		m_pDxAlphaMapFrameHead =  pDxFrameDel->pNextAlphaMapFrm;
	}
	else
	{
		DxFrame* pDxFrameCur = m_pDxAlphaMapFrameHead, *pDxFramePrev;
		while ( pDxFrameCur )
		{
			if ( pDxFrameCur == pDxFrameDel )
			{
				pDxFramePrev->pNextAlphaMapFrm = pDxFrameCur->pNextAlphaMapFrm;
				return;
			}

			pDxFramePrev = pDxFrameCur;
			pDxFrameCur = pDxFrameCur->pNextAlphaMapFrm;
		}
	}
}

BOOL DxFrameMesh::AddPieceObj ( char *szFrameName, const char *szPieceName )
{
	GASSERT(szFrameName);
	GASSERT(szPieceName);

	//	Note : Frame 찾기.
	//
	DxFrame *pDxFrame = FindFrame ( szFrameName );
	if ( !pDxFrame )	return FALSE;

	//	Note : 이전 치환 조각 설정되어 있을때.
	//
	if ( pDxFrame->pRePlacePiece )
		DelPieceObj ( szFrameName );

	// Note : 이름이 없으면 취소	
	if( !pDxFrame->szName )		return FALSE;

	//	Note : 치환 조각 가져오기.
	DXREPLACEPIECE* pPiece = DxReplaceContainer::GetInstance().LoadPiece ( szPieceName );
	if ( !pPiece )		return FALSE;

	DXPIECEOBJ *pPieceObj = new DXPIECEOBJ;
	pPieceObj->SetPiece ( pPiece );


	int nStrLen = strlen(pDxFrame->szName)+1;
	pPieceObj->szFrameName = new char[nStrLen];
	StringCchCopy( pPieceObj->szFrameName, nStrLen, pDxFrame->szName );

	//	Note : 프레임의 메트릭스로 업데이트.
	//
	pPieceObj->matComb = pDxFrame->matCombined;

	if ( pPiece->bRendAni )
	{
		srand(GetTickCount());
		pPieceObj->fCurTime = (rand()%1000) * 0.01f;
	}

	//	Note : 프레임에 치환 프레임 등록.
	//
	pDxFrame->pRePlacePiece = pPieceObj;

	//	Note : 치환 조각 등록시 알파맵 프래임 리스트에서 제거 해야함.
	//
	DelAlphaMapFrameAll ( pDxFrame );

	//	Note : 리스트에 등록.
	//
	pPieceObj->pNext = m_pPieceObjHead;
	m_pPieceObjHead = pPieceObj;

	return TRUE;
}

void DxFrameMesh::DelPieceObj ( char *szFrameName )
{
	GASSERT(szFrameName);
	if ( !m_pPieceObjHead )		return;

	DXPIECEOBJ* pPieceObjCur = m_pPieceObjHead, *pPieceObjPrev;

	//	Note : Frame 찾기.
	//
	DxFrame *pDxFrame = FindFrame ( szFrameName );
	if ( !pDxFrame )	return;

	//	Note : 치환 조각 제거시 알파맵 프래임 리스트에 추가 해야함.
	//
	AddAlphaMapFrameAll ( pDxFrame );

	if ( m_pPieceObjHead == pDxFrame->pRePlacePiece )
	{
		m_pPieceObjHead = pPieceObjCur->pNext;

		pDxFrame->pRePlacePiece = NULL;
		pPieceObjCur->pNext = NULL;
		SAFE_DELETE(pPieceObjCur);
	}
	else
	{
		while ( pPieceObjCur )
		{
			if ( pPieceObjCur == pDxFrame->pRePlacePiece )
			{
				pPieceObjPrev->pNext = pPieceObjCur->pNext;

				pDxFrame->pRePlacePiece = NULL;
				pPieceObjCur->pNext = NULL;
				SAFE_DELETE(pPieceObjCur);
				break;
			}

			pPieceObjPrev = pPieceObjCur;
			pPieceObjCur = pPieceObjCur->pNext;
		}
	}
}

void DxFrameMesh::MakePieceAABBTree ()
{
	DxReplaceContainer::MakeAABBTree ( m_pPieceObjTree, m_pPieceObjHead );
}

void DxFrameMesh::CleanPieceAABBTree ()
{
	SAFE_DELETE(m_pPieceObjTree);
}

void DxFrameMesh::MakeAnimationList ()
{
	DxAnimationMan *pAnimManCur = pAnimManHead;
	while ( pAnimManCur )
	{
		pAnimManCur->SetDxFrameRoot(NULL);
		pAnimManCur->SetAnimationHead(NULL);

		pAnimManCur = pAnimManCur->m_pNext;
	}

	SAFE_DELETE(pAnimManHead);


	MakeAnimationList ( m_pDxFrame );
}

void DxFrameMesh::MakeAnimationList ( DxFrame * pDxFrame )
{
	DxFrame		*pframeCur = pDxFrame->pframeFirstChild;
	while ( pframeCur )
	{
		if ( pframeCur->pframeFromAnimate )
		{
			DxAnimationMan* pNewAniMan = new DxAnimationMan;
			pNewAniMan->SetDxFrameRoot ( pframeCur );

			//	Note : 리스트에 삽입.
			//
			pNewAniMan->m_pNext = pAnimManHead;
			pAnimManHead = pNewAniMan;
		}
		else
		{
			MakeAnimationList ( pframeCur );
		}

		pframeCur = pframeCur->pframeSibling;
	}
}

DxAnimationMan* DxFrameMesh::FindAniRootFrame ( DxFrame *pDxFrame )
{
	DxAnimationMan *pAniManCur = pAnimManHead;
	while ( pAniManCur )
	{
		if ( pAniManCur->GetDxFrameRoot() == pDxFrame )
			return pAniManCur;

		pAniManCur = pAniManCur->m_pNext;
	}

	return NULL;
}

void	DxFrame::DxFrameNodeCount ( DWORD &nNodeCount )
{	
	nNodeCount++;

	DxFrame		*pframeChild = pframeFirstChild;
	while ( pframeChild )
	{
		pframeChild->DxFrameNodeCount ( nNodeCount );
		pframeChild = pframeChild->pframeSibling;
	}
}

//	Note : 효과 대체나 치환 조각 같은 대체물이 있을 경우 고려 해서
//		옥트리의 분할에서 재외하게 만듬.
//
BOOL DxFrame::IsReplace ()
{
	if ( bFlag&DXFRM_NAVIFRAME )				return TRUE;

	DxEffectBase* pEffCur = pEffect;
	while ( pEffCur )
	{
		if ( pEffCur->GetFlag()&(_EFF_REPLACE|_EFF_REPLACE_AFTER) )	return TRUE;
		pEffCur = pEffCur->pLocalFrameEffNext;
	}

	return pRePlacePiece!=NULL;
}	

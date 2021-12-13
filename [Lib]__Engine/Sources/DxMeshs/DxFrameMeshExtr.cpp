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

//	Note : ȿ�� �߰�.
//
void DxFrameMesh::AddEffect ( DxEffectBase *pEffectAdd )
{
	pEffectAdd->pEffectNext = m_pEffectHead;
	m_pEffectHead = pEffectAdd;
}

//	Note : ȿ�� ����.
//
BOOL DxFrameMesh::DelEffect ( DxFrame *pDxFrame, DxEffectBase *pEffectDel )
{
	DxEffectBase *pEffectCur, *pEffectPrev;

	//	Note : �������� ����Ʈ ����Ʈ���� ����.
	//

	//	Note : ��, ����Ʈ
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

	//	Note : ��, ����Ʈ
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

	//	Note : ��, ����Ʈ
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

	//	Note : ��ü�� ����Ʈ ����Ʈ���� ����.
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

	//	Note : ���� ȿ���� ���ŵǸ鼭 Replace ������ ���ŵǸ� 
	//		���ĸ��� �����ϴ� �������� ��� ���ĸ� ����Ʈ�� ����.
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

	//	Note :��� ����Ʈ�� �˻��� ������ ���� / ������ ���.
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

	//	Note : Frame ã��.
	//
	DxFrame *pDxFrame = FindFrame ( szFrameName );
	if ( !pDxFrame )	return FALSE;

	//	Note : ���� ġȯ ���� �����Ǿ� ������.
	//
	if ( pDxFrame->pRePlacePiece )
		DelPieceObj ( szFrameName );

	// Note : �̸��� ������ ���	
	if( !pDxFrame->szName )		return FALSE;

	//	Note : ġȯ ���� ��������.
	DXREPLACEPIECE* pPiece = DxReplaceContainer::GetInstance().LoadPiece ( szPieceName );
	if ( !pPiece )		return FALSE;

	DXPIECEOBJ *pPieceObj = new DXPIECEOBJ;
	pPieceObj->SetPiece ( pPiece );


	int nStrLen = strlen(pDxFrame->szName)+1;
	pPieceObj->szFrameName = new char[nStrLen];
	StringCchCopy( pPieceObj->szFrameName, nStrLen, pDxFrame->szName );

	//	Note : �������� ��Ʈ������ ������Ʈ.
	//
	pPieceObj->matComb = pDxFrame->matCombined;

	if ( pPiece->bRendAni )
	{
		srand(GetTickCount());
		pPieceObj->fCurTime = (rand()%1000) * 0.01f;
	}

	//	Note : �����ӿ� ġȯ ������ ���.
	//
	pDxFrame->pRePlacePiece = pPieceObj;

	//	Note : ġȯ ���� ��Ͻ� ���ĸ� ������ ����Ʈ���� ���� �ؾ���.
	//
	DelAlphaMapFrameAll ( pDxFrame );

	//	Note : ����Ʈ�� ���.
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

	//	Note : Frame ã��.
	//
	DxFrame *pDxFrame = FindFrame ( szFrameName );
	if ( !pDxFrame )	return;

	//	Note : ġȯ ���� ���Ž� ���ĸ� ������ ����Ʈ�� �߰� �ؾ���.
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

			//	Note : ����Ʈ�� ����.
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

//	Note : ȿ�� ��ü�� ġȯ ���� ���� ��ü���� ���� ��� ��� �ؼ�
//		��Ʈ���� ���ҿ��� ����ϰ� ����.
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

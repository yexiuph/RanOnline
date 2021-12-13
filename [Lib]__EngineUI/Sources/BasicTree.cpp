#include "pch.h"

#include "BasicTree.h"
#include "DxFontMan.h" //../[Lib]__Engine/Sources/DxCommon

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const float CBasicTree::fX_GAP = 13.0f;
const float	CBasicTree::fY_GAP = 17.0f;

using std::string;

CBasicTree::CBasicTree () :
	m_pSelectedNode ( NULL ),
	m_nStartIndex ( 0 ),
	m_nRenderCount ( 0 ),
	m_pFont ( NULL ),
	m_bUseLabelOverColor ( FALSE ),
	m_dwLabelOverColor ( UINT_MAX )
{
}

CBasicTree::~CBasicTree ()
{
}

void CBasicTree::InsertNewNode ( CBasicTreeNode* pParent, CBasicTreeNode* pNode )
{
	if ( !pNode )
	{
		GASSERT ( 0 && "CBasicTreeNode.cpp : 노드가 널입니다." );
		return ;
	}

	if ( pParent )
	{
		pParent->InsertChildNode ( pNode );
	}
	else
	{
		m_ChildList.push_back ( pNode );
	}

	CalcMaxLine ();
}

CBasicTreeNode*	CBasicTree::GetRootNode ()
{	
	if ( m_ChildList.empty () ) return NULL;	
	return m_ChildList.front ();
}

CBasicTreeNode*	CBasicTree::GetPrevSiblingNode ( CBasicTreeNode* pParent, CBasicTreeNode* pNode )
{
	if ( !pNode )
	{
		GASSERT ( 0 && "CBasicTreeNode.cpp : 노드가 널입니다." );
		return NULL;
	}

	if ( pParent )
	{
		CBasicTreeNode::TREENODE_LIST& ChildList = pParent->GetChildList ();
		CBasicTreeNode::TREENODE_LIST_ITER iter = ChildList.begin ();
		CBasicTreeNode::TREENODE_LIST_ITER iter_end = ChildList.end ();

		CBasicTreeNode* pPrevSibling = NULL;
		for ( ; iter != iter_end; ++iter )
		{
			CBasicTreeNode* pCur = (*iter);
			if ( pNode == pCur ) return pPrevSibling;
			pPrevSibling = pCur;
		}

		return NULL;
	}
	else
	{		
		CBasicTreeNode::TREENODE_LIST_ITER iter = m_ChildList.begin ();
		CBasicTreeNode::TREENODE_LIST_ITER iter_end = m_ChildList.end ();

		CBasicTreeNode* pPrevSibling = NULL;
		for ( ; iter != iter_end; ++iter )
		{
			CBasicTreeNode* pCur = (*iter);
			if ( pNode == pCur ) return pPrevSibling;
			pPrevSibling = pCur;
		}

		return NULL;
	}
}

CBasicTreeNode*	CBasicTree::GetNextSiblingNode ( CBasicTreeNode* pParent, CBasicTreeNode* pNode )
{
	if ( !pNode )
	{
		GASSERT ( 0 && "CBasicTreeNode.cpp : 노드가 널입니다." );
		return NULL;
	}

	if ( pParent )
	{
		CBasicTreeNode::TREENODE_LIST& ChildList = pParent->GetChildList ();
		CBasicTreeNode::TREENODE_LIST_ITER iter = ChildList.begin ();
		CBasicTreeNode::TREENODE_LIST_ITER iter_end = ChildList.end ();	
		for ( ; iter != iter_end; ++iter )
		{
			CBasicTreeNode* pCur = (*iter);
			if ( pNode == pCur )
			{
				++iter;
				return (*iter);
			}
		}

		return NULL;
	}
	else
	{		
		CBasicTreeNode::TREENODE_LIST_ITER iter = m_ChildList.begin ();
		CBasicTreeNode::TREENODE_LIST_ITER iter_end = m_ChildList.end ();

		CBasicTreeNode* pPrevSibling = NULL;
		for ( ; iter != iter_end; ++iter )
		{
			CBasicTreeNode* pCur = (*iter);
			if ( pNode == pCur )
			{
				++iter;
				return (*iter);
			}
		}

		return NULL;
	}
}                                          

void	CBasicTree::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage( ControlID, dwMsg );

	if ( TREENODE_BASE <= ControlID && ControlID <= TREENODE_END )
	{
		if ( CHECK_MOUSE_IN ( dwMsg ) )
		{
			if ( dwMsg & UIMSG_LB_DOWN )
			{
				m_pSelectedNode = (CBasicTreeNode*) FindControl ( ControlID );
			}
		}
	}
}

void CBasicTree::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
	
	UpdateNodePos ();

	if ( m_bUseLabelOverColor )
	{
		if ( m_pSelectedNode )
		{
			m_pSelectedNode->SetLabelUseOverColor ( TRUE );
			m_pSelectedNode->SetLabelOverColor ( m_dwLabelOverColor );
		}
	}
}

void CBasicTree::UpdateNodePos ( )
{
	if ( m_ChildList.empty () ) return ;

	CBasicTreeNode* pRootNode = GetRootNode ();
	if( !pRootNode ) return ;
	
	const UIRECT& rcGlobalPos = GetGlobalPos ();
	D3DXVECTOR2 vPos = D3DXVECTOR2 ( rcGlobalPos.left, rcGlobalPos.top );
	pRootNode->SetGlobalPos( vPos );

	const UIRECT& rcRootGlobalPos = pRootNode->GetGlobalPos ();
	D3DXVECTOR2 vRenderPos = D3DXVECTOR2 ( rcRootGlobalPos.left, rcRootGlobalPos.top );

	m_bRender = FALSE;
	m_nRenderCount = 0;
	UpdateNodePos ( m_ChildList, vRenderPos );
}

void CBasicTree::UpdateNodePos ( const CBasicTreeNode::TREENODE_LIST& list, D3DXVECTOR2& ref_vRenderPos )
{		
	D3DXVECTOR2 vRenderPosBack = ref_vRenderPos;

	CBasicTreeNode::TREENODE_LIST_CITER iter = list.begin ();
	CBasicTreeNode::TREENODE_LIST_CITER iter_end = list.end ();
	for ( ; iter != iter_end; ++iter )
	{
		CBasicTreeNode* pCur = (*iter);
		pCur->SetGlobalPos ( ref_vRenderPos );

		if ( m_nRenderCount == m_nStartIndex )
		{
			m_bRender = TRUE;
		}
		//	NOTE :
		//		최대로 뿌릴수 있는 라인을 체크한다.
		int nRenderCount = m_nRenderCount - m_nStartIndex;
		if ( m_nLinePerOneView < nRenderCount )
		{	
			m_bRender = FALSE;
		}

		pCur->SetVisibleSingle ( m_bRender );

		//	NOTE :
		//		노드의 개수를 카운트
		m_nRenderCount++;

		CUIControl* pParent = pCur->GetParent ();
		if ( pParent )
		{
			const UIRECT rcParentPos = pParent->GetGlobalPos ();
			const UIRECT rcGlobalPos = pCur->GetGlobalPos ();
			D3DXVECTOR2 vLocalPos =
				D3DXVECTOR2 ( rcGlobalPos.left - rcParentPos.left, rcGlobalPos.top - rcParentPos.top );
			pCur->SetLocalPos ( vLocalPos );
		}

		if ( pCur->IsBranchVisible () )
		{
			//	NOTE :
			//		보이지 않는 노드들에 대해서 계산은 불필요하다.
			ref_vRenderPos.x += fX_GAP;

			if ( m_bRender )
			{				
				ref_vRenderPos.y += fY_GAP;
			}

			UpdateNodePos ( pCur->GetChildList (), ref_vRenderPos );
		}

		//	NOTE :
		//		보이지 않는 노드들에 대해서 계산은 불필요하다.
		ref_vRenderPos.x = vRenderPosBack.x;
		if ( m_bRender )
		{			
			ref_vRenderPos.y += fY_GAP;
		}
	}

	//	NOTE :
	//		보이지 않는 노드들에 대해서 계산은 불필요하다.
	if ( m_bRender ) ref_vRenderPos.y -= fY_GAP;	
}

void	CBasicTree::SetFont ( CD3DFontPar* pFont )
{
	m_pFont = pFont;
	CalcMaxLine ();
}

void	CBasicTree::CalcMaxLine ()
{
	TCHAR szTest[] = _T("테스트1234ABC");

	SIZE Size;
	m_pFont->GetTextExtent ( szTest, Size );

	const float fHeight = GetGlobalPos ().sizeY;
	const float fOneLine = fY_GAP;

	m_nLinePerOneView = (int)floor(fHeight / fOneLine);

	if ( m_nLinePerOneView < 1 )
	{
		GASSERT ( 0 && "원하는 만큼의 라인을 처리할 수 없습니다. 다시 설정하십시오." );
	}
}

//HRESULT CBasicTree::InvalidateDeviceObjects ()
//{
//	CUIGroup::InvalidateDeviceObjects();
//
//	UpdateNodePos();
//
//	return S_OK;
//}
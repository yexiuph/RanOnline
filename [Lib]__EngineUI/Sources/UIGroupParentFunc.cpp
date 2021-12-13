#include "pch.h"
#include "UIGroup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void CUIGroup::ResetMessageEx()
{
	CUIControl::ResetMessageEx();

	const CUIControlContainer::UICONTROL_LIST& list = m_ControlContainer.GetControl();
	CUIControlContainer::UICONTROL_LIST_CITER citer = list.begin ();
	CUIControlContainer::UICONTROL_LIST_CITER citer_end = list.end ();
	CUIControl * pControl = NULL;
	for ( ; citer != citer_end; ++citer )
	{
		pControl = *citer;
		if( !pControl ) continue;

		pControl->ResetMessageEx ();		
	}
}

void CUIGroup::AlignSubControl ( const UIRECT& rcParentOldPos, const UIRECT& rcParentNewPos )
{
	const UIRECT rcOldPos = GetLocalPos ();
	CUIControl::AlignSubControl ( rcParentOldPos, rcParentNewPos ); // 부모를 정렬하고

	const UIRECT rcNewPos = GetLocalPos ();

	const CUIControlContainer::UICONTROL_LIST& list = m_ControlContainer.GetControl ();
	CUIControlContainer::UICONTROL_LIST_CITER citer = list.begin ();
	CUIControlContainer::UICONTROL_LIST_CITER citer_end = list.end ();
	CUIControl * pControl = NULL;
	for ( ; citer != citer_end; ++citer )
	{
		pControl = *citer;
		if( !pControl ) continue;

		pControl->AlignSubControl( rcOldPos, rcNewPos ); // 자식들을 정렬한다.
	}
}

void CUIGroup::ReSizeControl( const UIRECT& rcParentOldPos, const UIRECT& rcParentNewPos )
{
//	const UIRECT rcOldPos = GetLocalPos ();
	const UIRECT rcOldPos = GetGlobalPos ();
	CUIControl::ReSizeControl ( rcParentOldPos, rcParentNewPos ); // 부모를 정렬하고

//	const UIRECT rcNewPos = GetLocalPos ();
	const UIRECT rcNewPos = GetGlobalPos ();

	const CUIControlContainer::UICONTROL_LIST& list = m_ControlContainer.GetControl ();
	CUIControlContainer::UICONTROL_LIST_CITER citer = list.begin ();
	CUIControlContainer::UICONTROL_LIST_CITER citer_end = list.end ();
	CUIControl * pControl = NULL;
	for ( ; citer != citer_end; ++citer )
	{	
		pControl = *citer;
		if( !pControl ) continue;

		pControl->ReSizeControl( rcOldPos, rcNewPos ); // 자식들을 정렬한다.
	}
}


void CUIGroup::SetTransparentOption ( BOOL bTransparency )
{
	CUIControl::SetTransparentOption ( bTransparency );

	const CUIControlContainer::UICONTROL_LIST& list = m_ControlContainer.GetControl ();
	CUIControlContainer::UICONTROL_LIST_CITER citer = list.begin ();
	CUIControlContainer::UICONTROL_LIST_CITER citer_end = list.end ();
	CUIControl * pControl = NULL;
	for ( ; citer != citer_end; ++citer )
	{
		pControl = *citer;
		if( !pControl ) continue;

		pControl->SetTransparentOption ( bTransparency );
	}
}

BOOL CUIGroup::SetFadeIn ()
{
	if ( !CUIControl::SetFadeIn () ) return FALSE;

	const CUIControlContainer::UICONTROL_LIST& list = m_ControlContainer.GetControl ();
	CUIControlContainer::UICONTROL_LIST_CITER citer = list.begin ();
	CUIControlContainer::UICONTROL_LIST_CITER citer_end = list.end ();
	CUIControl * pControl = NULL;
	for ( ; citer != citer_end; ++citer )
	{
		pControl = *citer;
		if( !pControl ) continue;

		if( !pControl->SetFadeIn() ) return FALSE;
	}

	return TRUE;
}

BOOL CUIGroup::SetFadeOut()
{
	if ( !CUIControl::SetFadeOut () ) return FALSE;

	const CUIControlContainer::UICONTROL_LIST& list = m_ControlContainer.GetControl ();
	CUIControlContainer::UICONTROL_LIST_CITER citer = list.begin ();
	CUIControlContainer::UICONTROL_LIST_CITER citer_end = list.end ();
	CUIControl * pControl = NULL;
	for ( ; citer != citer_end; ++citer )
	{
		pControl = *citer;
		if( !pControl ) continue;

		if( !pControl->SetFadeOut() ) return FALSE;
	}

	return TRUE;
}

void CUIGroup::SetDiffuse(D3DCOLOR _diffuse)
{
	CUIControl::SetDiffuse( _diffuse );

	const CUIControlContainer::UICONTROL_LIST& list = m_ControlContainer.GetControl ();
	CUIControlContainer::UICONTROL_LIST_CITER citer = list.begin ();
	CUIControlContainer::UICONTROL_LIST_CITER citer_end = list.end ();
	CUIControl * pControl = NULL;
	for ( ; citer != citer_end; ++citer )
	{
		pControl = *citer;
		if( !pControl ) continue;

		pControl->SetDiffuse ( _diffuse );
	}
}

void CUIGroup::SetGlobalPos(const UIRECT& Pos)
{
	CUIControl::SetGlobalPos ( Pos );

	const UIRECT& rcOriginPos = GetGlobalPos ();	

	const CUIControlContainer::UICONTROL_LIST& list = m_ControlContainer.GetControl ();
	CUIControlContainer::UICONTROL_LIST_CITER citer = list.begin ();
	CUIControlContainer::UICONTROL_LIST_CITER citer_end = list.end ();
	CUIControl * pControl = NULL;
	for ( ; citer != citer_end; ++citer )
	{
		pControl = *citer;
		if( !pControl ) continue;

		const UIRECT& rcChildPos = pControl->GetLocalPos ();

		const UIRECT rcNewChildPos( rcOriginPos.left + rcChildPos.left, rcOriginPos.top + rcChildPos.top,
									rcChildPos.sizeX, rcChildPos.sizeY );

		pControl->SetGlobalPos ( rcNewChildPos );
	}
}

void CUIGroup::SetGlobalPos(const D3DXVECTOR2& vPos)
{	
	CUIControl::SetGlobalPos ( vPos );

	const UIRECT& rcPos = GetGlobalPos ();
	const D3DXVECTOR2 vParentPos ( rcPos.left, rcPos.top );

	const CUIControlContainer::UICONTROL_LIST& list = m_ControlContainer.GetControl ();
	CUIControlContainer::UICONTROL_LIST_CITER citer = list.begin ();
	CUIControlContainer::UICONTROL_LIST_CITER citer_end = list.end ();
	CUIControl * pControl = NULL;
	for ( ; citer != citer_end; ++citer )
	{
		pControl = *citer;
		if( !pControl ) continue;

		const UIRECT& rcChildPos = pControl->GetLocalPos ();
		const D3DXVECTOR2 vChildPos( vParentPos.x + rcChildPos.left, vParentPos.y + rcChildPos.top );
		pControl->SetGlobalPos( vChildPos );
	}
}

BOOL CUIGroup::IsExclusiveControl()
{
	return m_bAllExclusiveControl;

	if( IsExclusiveSelfControl() ) return TRUE;

	const CUIControlContainer::UICONTROL_LIST& list = m_ControlContainer.GetControl ();
	CUIControlContainer::UICONTROL_LIST_CITER citer = list.begin ();
	CUIControlContainer::UICONTROL_LIST_CITER citer_end = list.end ();
	CUIControl * pControl = NULL;
	for ( ; citer != citer_end; ++citer )
	{
		pControl = *citer;
		if( !pControl ) continue;

		if( pControl->IsExclusiveControl() ) return TRUE;
	}

	return FALSE;
}

void CUIGroup::SetExclusiveControl()
{
	m_bExclusiveControl = TRUE;
	m_bAllExclusiveControl = TRUE;
}

void CUIGroup::ResetExclusiveControl()
{
	if ( FALSE == m_bAllExclusiveControl ) return;

	CUIControl::ResetExclusiveControl();

	const CUIControlContainer::UICONTROL_LIST& list = m_ControlContainer.GetControl ();
	CUIControlContainer::UICONTROL_LIST_CITER citer = list.begin ();
	CUIControlContainer::UICONTROL_LIST_CITER citer_end = list.end ();
	CUIControl * pControl = NULL;
	for ( ; citer != citer_end; ++citer )
	{
		pControl = *citer;
		if( !pControl ) continue;

		pControl->ResetExclusiveControl();
	}

	m_bAllExclusiveControl = FALSE;
}
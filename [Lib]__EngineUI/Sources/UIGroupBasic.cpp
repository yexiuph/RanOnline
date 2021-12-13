#include "pch.h"
#include "UIGroup.h"
#include "DxInputDevice.h"
#include "UIMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HRESULT CUIGroup::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT( pd3dDevice );

	HRESULT hr = S_OK;
	hr = CUIControl::InitDeviceObjects ( pd3dDevice );
	if ( FAILED ( hr ) ) return hr;

	const CUIControlContainer::UICONTROL_LIST& list = m_ControlContainer.GetControl ();
	CUIControlContainer::UICONTROL_LIST_CITER citer = list.begin ();
	CUIControlContainer::UICONTROL_LIST_CITER citer_end = list.end ();
	CUIControl * pControl = NULL;
	for ( ; citer != citer_end; ++citer )
	{
		pControl = *citer;
		if( !pControl ) continue;

		hr = pControl->InitDeviceObjects ( pd3dDevice );
		if ( FAILED ( hr ) ) return hr;
	}

	return S_OK;
}

HRESULT CUIGroup::InvalidateDeviceObjects ()
{
	HRESULT hr = S_OK;
	hr = CUIControl::InvalidateDeviceObjects();
	if ( FAILED ( hr ) ) return hr;

	const CUIControlContainer::UICONTROL_LIST& list = m_ControlContainer.GetControl ();
	CUIControlContainer::UICONTROL_LIST_CITER citer = list.begin ();
	CUIControlContainer::UICONTROL_LIST_CITER citer_end = list.end ();
	CUIControl * pControl = NULL;
	for ( ; citer != citer_end; ++citer )
	{
		pControl = *citer;
		if( !pControl ) continue;

		hr = pControl->InvalidateDeviceObjects ();
		if ( FAILED ( hr ) ) return hr;
	}

	return S_OK;
}

HRESULT CUIGroup::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT( pd3dDevice );

	HRESULT hr = S_OK;
	hr = CUIControl::RestoreDeviceObjects ( pd3dDevice );
	if ( FAILED ( hr ) ) return hr;

	UIRECT rcOriginPos = GetGlobalPos ();

	const CUIControlContainer::UICONTROL_LIST& list = m_ControlContainer.GetControl ();
	CUIControlContainer::UICONTROL_LIST_CITER citer = list.begin ();
	CUIControlContainer::UICONTROL_LIST_CITER citer_end = list.end ();
	CUIControl * pControl = NULL;
	for ( ; citer != citer_end; ++citer )
	{
		pControl = *citer;
		if( !pControl ) continue;

		//	부모가 있다는 이야기는 상대포지션을 이용한다는 이야기고,
		//	없다는 것은 절대 포지션을 이용한다는 말이다.
		UIRECT rcChildPosBack;

		if ( pControl->GetParent () )
		{
			rcChildPosBack = pControl->GetLocalPos ();
		}
		else
		{
			GASSERT ( 0 && "부모가 없는 컨트롤은 존재할 수 없습니다." );
			//rcChildPosBack = pControl->GetGlobalPos ();
		}

		UIRECT rcChildPos(	rcOriginPos.left + rcChildPosBack.left, rcOriginPos.top + rcChildPosBack.top,
							rcChildPosBack.sizeX, rcChildPosBack.sizeY );

		BOOL bCheckProtectSize = m_bCheckProtectSize;
		m_bCheckProtectSize = FALSE;

		pControl->SetGlobalPos ( rcChildPos );
		pControl->RestoreDeviceObjects ( pd3dDevice );

		m_bCheckProtectSize = bCheckProtectSize;
	}

	return S_OK;
}

HRESULT CUIGroup::DeleteDeviceObjects ()
{
	HRESULT hr = S_OK;
	hr = CUIControl::DeleteDeviceObjects();
	if ( FAILED ( hr ) ) return hr;

	const CUIControlContainer::UICONTROL_LIST& list = m_ControlContainer.GetControl ();
	CUIControlContainer::UICONTROL_LIST_CITER citer = list.begin ();
	CUIControlContainer::UICONTROL_LIST_CITER citer_end = list.end ();
	CUIControl * pControl = NULL;
	for ( ; citer != citer_end; ++citer )
	{
		pControl = *citer;
		if( !pControl ) continue;

		hr = pControl->DeleteDeviceObjects ();
		if ( FAILED ( hr ) ) return hr;
	}

	return S_OK;
}

HRESULT CUIGroup::Render ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if( !IsVisible() ) return S_OK;
	
	HRESULT hr = S_OK;
    hr = CUIControl::Render ( pd3dDevice );
	if( FAILED( hr ) ) return hr;

	const CUIControlContainer::UICONTROL_LIST& list = m_ControlContainer.GetControl ();
	CUIControlContainer::UICONTROL_LIST_CITER citer = list.begin ();
	CUIControlContainer::UICONTROL_LIST_CITER citer_end = list.end ();
	CUIControl * pControl = NULL;
	for ( ; citer != citer_end; ++citer )
	{
		pControl = *citer;
		if( !pControl ) continue;

		if ( !pControl->IsVisible () ) continue;

		hr = pControl->Render( pd3dDevice );
		if ( FAILED ( hr ) ) return hr;
	}

	return S_OK;
}
#include "pch.h"
#include "UIMan.h"
#include "UIControl.h"
#include "DxRenderStates.h"
#include "DxInputDevice.h"
#include "UIRenderQueue.h"
#include "UIKeyCheck.h"

#include "../TextTexture/TextUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HRESULT CUIMan::OneTimeSceneInit ()
{
	HRESULT hr = S_OK;

	const CUIControlContainer::UICONTROL_LIST& list = m_ControlContainer.GetControl ();
	CUIControlContainer::UICONTROL_LIST_CITER citer = list.begin ();
	CUIControlContainer::UICONTROL_LIST_CITER citer_end = list.end ();
	for ( ; citer != citer_end; ++citer )
	{
		CUIControl * pControl = *citer;
		if( !pControl ) continue;

		hr = pControl->OneTimeSceneInit ();
		if ( FAILED ( hr ) ) return hr;
	}

	return S_OK;
}

HRESULT CUIMan::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT( pd3dDevice );
	HRESULT hr = S_OK;

	m_pd3dDevice = pd3dDevice;

	const CUIControlContainer::UICONTROL_LIST& list = m_ControlContainer.GetControl ();
	CUIControlContainer::UICONTROL_LIST_CITER citer = list.begin ();
	CUIControlContainer::UICONTROL_LIST_CITER citer_end = list.end ();
	for ( ; citer != citer_end; ++citer )
	{
		CUIControl * pControl = *citer;
		if( !pControl ) continue;

		hr = pControl->InitDeviceObjects ( pd3dDevice );
		if ( FAILED ( hr ) ) return hr;
	}

	return S_OK;
}

HRESULT CUIMan::InvalidateDeviceObjects ()
{
	HRESULT hr = S_OK;

	const CUIControlContainer::UICONTROL_LIST& list = m_ControlContainer.GetControl ();
	CUIControlContainer::UICONTROL_LIST_CITER citer = list.begin ();
	CUIControlContainer::UICONTROL_LIST_CITER citer_end = list.end ();
	for ( ; citer != citer_end; ++citer )
	{
		CUIControl * pControl = *citer;
		if( !pControl ) continue;

		hr = pControl->InvalidateDeviceObjects ();
		if ( FAILED ( hr ) ) return hr;
	}

	return S_OK;
}

HRESULT CUIMan::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT( pd3dDevice );
	HRESULT hr = S_OK;

	const CUIControlContainer::UICONTROL_LIST& list = m_ControlContainer.GetControl ();
	CUIControlContainer::UICONTROL_LIST_CITER citer = list.begin ();
	CUIControlContainer::UICONTROL_LIST_CITER citer_end = list.end ();
	for ( ; citer != citer_end; ++citer )
	{
		CUIControl * pControl = *citer;
		if( !pControl ) continue;

		hr = (*citer)->RestoreDeviceObjects ( pd3dDevice );
		if ( FAILED ( hr ) ) return hr;
	}

	MaintainControlPosition(); // MEMO : 포지션 유지	

	return S_OK;
}

HRESULT CUIMan::DeleteDeviceObjects ()
{
	HRESULT hr = S_OK;

	const CUIControlContainer::UICONTROL_LIST& list = m_ControlContainer.GetControl ();
	CUIControlContainer::UICONTROL_LIST_CITER citer = list.begin ();
	CUIControlContainer::UICONTROL_LIST_CITER citer_end = list.end ();
	for ( ; citer != citer_end; ++citer )
	{
		CUIControl * pControl = *citer;
		if( !pControl ) continue;

		hr = pControl->DeleteDeviceObjects (); // MEMO
		if( FAILED( hr ) ) return hr;
	}

	return S_OK;
}

HRESULT CUIMan::RenderList ( CUIFocusContainer* pList, LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT( pList && pd3dDevice );
	HRESULT hr = S_OK;

	const CUIFocusContainer::UICONTROL_FOCUSLIST& focus_list =  pList->GetFocusList ();
	CUIFocusContainer::UICONTROL_FOCUSLIST_CITER citer = focus_list.begin ();
	CUIFocusContainer::UICONTROL_FOCUSLIST_CITER citer_end = focus_list.end ();

	//	렌더는 포커스가 맞춰진것의 반대로 출력해야
	//	화면에 보이는 순서가 맞다.
	for ( ; citer != citer_end; ++citer )
	{
		CUIControl * pUIControl = citer->pUIControl;
		if( !pUIControl) continue;

		hr = pUIControl->Render( pd3dDevice );
		if ( FAILED ( hr ) ) return hr;
	}

	return S_OK;
}

HRESULT CUIMan::Render ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT( pd3dDevice );

	CTextUtil::Get()->FrameMove();

	DWORD dwAlphaBlendEnable, dwSrcBlend, dwDestBlend, dwZWriteEnable, dwZEnable, dwZFunc, dwClipPlaneEnable, dwFogEnable;
	DWORD dwColorOP, dwAlphaOP, dwMinFilter, dwMagFilter, dwMipFilter;

	pd3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE,	&dwAlphaBlendEnable );
	pd3dDevice->GetRenderState( D3DRS_SRCBLEND,			&dwSrcBlend );
	pd3dDevice->GetRenderState( D3DRS_DESTBLEND,		&dwDestBlend );

	pd3dDevice->GetRenderState( D3DRS_ZWRITEENABLE,		&dwZWriteEnable );
	pd3dDevice->GetRenderState( D3DRS_ZENABLE,			&dwZEnable );
	pd3dDevice->GetRenderState( D3DRS_ZFUNC,			&dwZFunc);
	pd3dDevice->GetRenderState( D3DRS_CLIPPLANEENABLE,	&dwClipPlaneEnable );
	pd3dDevice->GetRenderState( D3DRS_FOGENABLE,		&dwFogEnable );

	pd3dDevice->GetTextureStageState( 0, D3DTSS_COLOROP,	&dwColorOP );
	pd3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAOP,	&dwAlphaOP );

	pd3dDevice->GetSamplerState( 0, D3DSAMP_MINFILTER,	&dwMinFilter );
	pd3dDevice->GetSamplerState( 0, D3DSAMP_MAGFILTER,	&dwMagFilter );
	pd3dDevice->GetSamplerState( 0, D3DSAMP_MIPFILTER,	&dwMipFilter );

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_INVSRCALPHA );

	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		FALSE );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE,			FALSE );
	pd3dDevice->SetRenderState( D3DRS_ZFUNC,			D3DCMP_ALWAYS);
	pd3dDevice->SetRenderState( D3DRS_CLIPPLANEENABLE,	FALSE );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,		FALSE );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );

	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER,	D3DTEXF_POINT );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER,	D3DTEXF_POINT );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER,	D3DTEXF_NONE );

	HRESULT hr = S_OK;

	hr = RenderList ( &m_BottomList, pd3dDevice );
	if ( FAILED ( hr ) )	goto _RETURN;

	hr = RenderList ( &m_FocusList, pd3dDevice );
	if ( FAILED ( hr ) )	goto _RETURN;

	hr = RenderList ( &m_TopList, pd3dDevice );
	if ( FAILED ( hr ) )	goto _RETURN;

_RETURN:
	CUIRenderQueue::Get()->RenderImmediately();

	pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	dwAlphaBlendEnable );
	pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			dwSrcBlend );
	pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		dwDestBlend );

	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		dwZWriteEnable );
	pd3dDevice->SetRenderState( D3DRS_ZENABLE,			dwZEnable );
	pd3dDevice->SetRenderState( D3DRS_ZFUNC,			dwZFunc);
	pd3dDevice->SetRenderState( D3DRS_CLIPPLANEENABLE,	dwClipPlaneEnable );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE,		dwFogEnable );

	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	dwColorOP );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	dwAlphaOP );

	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER,	dwMinFilter );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER,	dwMagFilter );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER,	dwMipFilter );

	return S_OK;
}

BOOL CUIMan::UpdateList ( CUIFocusContainer* pList, BOOL bFocusList, float fElapsedTime )
{
	GASSERT( pList );

	UIGUID cID(NO_ID);
	CUIControl * pControl(NULL);
	int PosX(0), PosY(0), PosZ(0);
	static BOOL bFirstControl(TRUE);

	if( bFocusList )	bFirstControl = TRUE;

	DxInputDevice::GetInstance().GetMouseLocate ( PosX, PosY, PosZ );
	BYTE LB = (BYTE)DxInputDevice::GetInstance().GetMouseState(DXMOUSE_LEFT);
	BYTE MB = (BYTE)DxInputDevice::GetInstance().GetMouseState(DXMOUSE_MIDDLE);
	BYTE RB = (BYTE)DxInputDevice::GetInstance().GetMouseState(DXMOUSE_RIGHT);
	int nScroll = DxInputDevice::GetInstance().GetMouseMoveZ ();

	CUIFocusContainer::UICONTROL_FOCUSLIST focus_list = pList->GetFocusList ();
	CUIFocusContainer::UICONTROL_FOCUSLIST_RITER riter = focus_list.rbegin ();
	CUIFocusContainer::UICONTROL_FOCUSLIST_RITER riter_end = focus_list.rend ();

	for ( ; riter != riter_end; ++riter )
	{
		cID = riter->cID;
		pControl = riter->pUIControl;
		if( !pControl ) continue;

		if ( !pControl->IsVisible () ) continue;

		// MEMO : 독점 컨트롤이 없다면...
		if( !m_bExclusiveControl )
		{
			if( pControl->IsExclusiveControl() )
			{
				m_bExclusiveControl = TRUE;
				m_pExclusiveControl = pControl;
			}
		}

		pControl->Update ( PosX, PosY, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );		

		if ( bFirstControl )
		{
			DWORD dwMsg = pControl->GetMessageEx ();
			// MEMO : 상속받는 객체에서 호출된다.
			TranslateUIMessage ( cID, dwMsg );

			//	모달일 때
			if ( CHECK_MODAL_ON ( dwMsg ) )
			{
				bool bResult = pList->InsertTail( cID, pControl );
				if ( !bResult )
				{
					GASSERT ( 0 && "포커스 설정에 오류가 발생했습니다." );
					return FALSE;
				}
				SetMouseInControl ( true );
				bFirstControl = IsMouseInControl() ? FALSE : TRUE;
			}

			if ( bFocusList ) // MEMO : 포커스 리스트라면...
			{
				if ( bFirstControl && ( CHECK_MOUSE_IN ( dwMsg ) || CHECK_KEYFOCUSED ( dwMsg ) ) )
				{
					//bool bFocus = false;
					//if ( dwMsg & UIMSG_LB_DOWN ) bFocus = true;
					//if ( dwMsg & UIMSG_MB_DOWN ) bFocus = true;
					//if ( dwMsg & UIMSG_RB_DOWN ) bFocus = true;
					//if ( CHECK_KEYFOCUSED ( dwMsg ) ) bFocus = true;

					//if ( bFocus ) // 포커스 등록하기

					//{
					//	if( IsVisibleGroup( cID ) )
					//	{						
					//		pList->InsertTail( cID, pControl ); // MEMO : 포커스를 위해 리스크 마지막에 등록
					//		pControl->SetFocusControl();
					//		//m_pFocusControl = pControl;
					//	}
					//	else
					//	{
					//		if( pList->IsInserted( cID ) ) pList->InsertHead( cID, pControl );
					//		pControl->ResetFocusControl();
					//	}
					//}

					//bool bMouseClick = false;
					//if ( CHECK_LB_UP_LIKE ( dwMsg ) )	bMouseClick = true;
					//if ( CHECK_LB_DOWN_LIKE ( dwMsg ) ) bMouseClick = true;
					//if ( CHECK_MB_UP_LIKE ( dwMsg ) )	bMouseClick = true;
					//if ( CHECK_MB_DOWN_LIKE ( dwMsg ) ) bMouseClick = true;
					//if ( CHECK_RB_UP_LIKE ( dwMsg ) )	bMouseClick = true;
					//if ( CHECK_RB_DOWN_LIKE ( dwMsg ) ) bMouseClick = true;

					////	마우스가 클릭되면...
					////	마우스가 클릭(다운 또는 업 또는 눌러진 상태) 이벤트를 발생시켰다는 것은,
					////	어디든 첫번째 컨트롤로 처리되었다는 의미이다.
					//if ( bMouseClick ) 
					//{
					//	m_bFirstControl = FALSE;
					//}

					//if ( UIMSG_MOUSEIN & dwMsg ) SetMouseInControl ( true );	

					if( ( dwMsg & UIMSG_LB_DOWN ) || ( dwMsg & UIMSG_MB_DOWN ) ||
						( dwMsg & UIMSG_RB_DOWN ) || ( CHECK_KEYFOCUSED( dwMsg ) ) )
					{
						//	Memo :	이 부분에서 체크를 안하면
						//			보이지 않는 컨트롤이 포커스로 등록된다.
						if( IsVisibleGroup( cID ) )
						{
							if( m_pFocusControl != pControl )
							{
								if( m_pFocusControl )
									m_pFocusControl->ResetFocusControl();

								m_pFocusControl = pControl;

								// MEMO : 포커스를 위해 리스크 마지막에 등록
								pList->InsertTail( cID, pControl );
								pControl->SetFocusControl();
							}
						}
					}

					if( CHECK_LB_UP_LIKE ( dwMsg ) || CHECK_LB_DOWN_LIKE ( dwMsg ) ||
						CHECK_MB_UP_LIKE ( dwMsg ) || CHECK_MB_DOWN_LIKE ( dwMsg ) ||
						CHECK_RB_UP_LIKE ( dwMsg ) || CHECK_RB_DOWN_LIKE ( dwMsg ) || 
						CHECK_MOUSE_IN ( dwMsg ) ) 
					{
						//	마우스가 클릭되면...
						//	마우스가 클릭(다운 또는 업 또는 눌러진 상태) 이벤트를 발생시켰다는 것은,
						//	어디든 첫번째 컨트롤로 처리되었다는 의미이다.
						bFirstControl = FALSE;
					}

					if ( UIMSG_MOUSEIN & dwMsg ) 
						SetMouseInControl ( true );					
				}
			}
		}
	}

	return TRUE;
}

HRESULT CUIMan::FrameMove ( LPDIRECT3DDEVICEQ pd3dDevice, float fElapsedTime )
{
	GASSERT( pd3dDevice );

	//	Render 역순으로 UpdateList () 호출
	//
	//m_bFirstControl = TRUE;		// MEMO : 최초컨트롤 리셋

	m_bExclusiveControl = FALSE;	// MEMO : 독점 컨트롤 존재 여부 리셋
	m_pExclusiveControl = NULL;		// MEMO : 독점 컨트롤 포인터 리셋
	//m_bFocusControl = FALSE;		// MEMO : 포커스 컨트롤 존재 여부 리셋
	//m_pFocusControl = NULL;		// MEMO : 포커스 컨트롤 포인터 리셋

	UIKeyCheck::GetInstance()->Clear();	//	단축키 리셋
	SetMouseInControl( FALSE );

	if( !UpdateList( &m_FocusList, TRUE, fElapsedTime ) )	return E_FAIL;
	if( !UpdateList( &m_BottomList, FALSE, fElapsedTime ) )	return E_FAIL;
	if( !UpdateList( &m_TopList, FALSE, fElapsedTime ) )	return E_FAIL;

	//UpdateActionMsgQ();

	// MEMO : 포지션 유지를 위한 셋팅
	SetControlPosition();

	return S_OK;
}

HRESULT CUIMan::FinalCleanup ()
{
	HRESULT hr = S_OK;

	const CUIControlContainer::UICONTROL_LIST& list = m_ControlContainer.GetControl ();
	CUIControlContainer::UICONTROL_LIST_CITER citer = list.begin ();
	CUIControlContainer::UICONTROL_LIST_CITER citer_end = list.end ();
	for ( ; citer != citer_end; ++citer )
	{
		CUIControl * pControl = *citer;
		if( !pControl ) continue;

		hr = pControl->FinalCleanup ();
		if ( FAILED ( hr ) ) return hr;
	}

	DeleteAllControl();

	return S_OK;
}
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

	MaintainControlPosition(); // MEMO : ������ ����	

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

	//	������ ��Ŀ���� ���������� �ݴ�� ����ؾ�
	//	ȭ�鿡 ���̴� ������ �´�.
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

		// MEMO : ���� ��Ʈ���� ���ٸ�...
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
			// MEMO : ��ӹ޴� ��ü���� ȣ��ȴ�.
			TranslateUIMessage ( cID, dwMsg );

			//	����� ��
			if ( CHECK_MODAL_ON ( dwMsg ) )
			{
				bool bResult = pList->InsertTail( cID, pControl );
				if ( !bResult )
				{
					GASSERT ( 0 && "��Ŀ�� ������ ������ �߻��߽��ϴ�." );
					return FALSE;
				}
				SetMouseInControl ( true );
				bFirstControl = IsMouseInControl() ? FALSE : TRUE;
			}

			if ( bFocusList ) // MEMO : ��Ŀ�� ����Ʈ���...
			{
				if ( bFirstControl && ( CHECK_MOUSE_IN ( dwMsg ) || CHECK_KEYFOCUSED ( dwMsg ) ) )
				{
					//bool bFocus = false;
					//if ( dwMsg & UIMSG_LB_DOWN ) bFocus = true;
					//if ( dwMsg & UIMSG_MB_DOWN ) bFocus = true;
					//if ( dwMsg & UIMSG_RB_DOWN ) bFocus = true;
					//if ( CHECK_KEYFOCUSED ( dwMsg ) ) bFocus = true;

					//if ( bFocus ) // ��Ŀ�� ����ϱ�

					//{
					//	if( IsVisibleGroup( cID ) )
					//	{						
					//		pList->InsertTail( cID, pControl ); // MEMO : ��Ŀ���� ���� ����ũ �������� ���
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

					////	���콺�� Ŭ���Ǹ�...
					////	���콺�� Ŭ��(�ٿ� �Ǵ� �� �Ǵ� ������ ����) �̺�Ʈ�� �߻����״ٴ� ����,
					////	���� ù��° ��Ʈ�ѷ� ó���Ǿ��ٴ� �ǹ��̴�.
					//if ( bMouseClick ) 
					//{
					//	m_bFirstControl = FALSE;
					//}

					//if ( UIMSG_MOUSEIN & dwMsg ) SetMouseInControl ( true );	

					if( ( dwMsg & UIMSG_LB_DOWN ) || ( dwMsg & UIMSG_MB_DOWN ) ||
						( dwMsg & UIMSG_RB_DOWN ) || ( CHECK_KEYFOCUSED( dwMsg ) ) )
					{
						//	Memo :	�� �κп��� üũ�� ���ϸ�
						//			������ �ʴ� ��Ʈ���� ��Ŀ���� ��ϵȴ�.
						if( IsVisibleGroup( cID ) )
						{
							if( m_pFocusControl != pControl )
							{
								if( m_pFocusControl )
									m_pFocusControl->ResetFocusControl();

								m_pFocusControl = pControl;

								// MEMO : ��Ŀ���� ���� ����ũ �������� ���
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
						//	���콺�� Ŭ���Ǹ�...
						//	���콺�� Ŭ��(�ٿ� �Ǵ� �� �Ǵ� ������ ����) �̺�Ʈ�� �߻����״ٴ� ����,
						//	���� ù��° ��Ʈ�ѷ� ó���Ǿ��ٴ� �ǹ��̴�.
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

	//	Render �������� UpdateList () ȣ��
	//
	//m_bFirstControl = TRUE;		// MEMO : ������Ʈ�� ����

	m_bExclusiveControl = FALSE;	// MEMO : ���� ��Ʈ�� ���� ���� ����
	m_pExclusiveControl = NULL;		// MEMO : ���� ��Ʈ�� ������ ����
	//m_bFocusControl = FALSE;		// MEMO : ��Ŀ�� ��Ʈ�� ���� ���� ����
	//m_pFocusControl = NULL;		// MEMO : ��Ŀ�� ��Ʈ�� ������ ����

	UIKeyCheck::GetInstance()->Clear();	//	����Ű ����
	SetMouseInControl( FALSE );

	if( !UpdateList( &m_FocusList, TRUE, fElapsedTime ) )	return E_FAIL;
	if( !UpdateList( &m_BottomList, FALSE, fElapsedTime ) )	return E_FAIL;
	if( !UpdateList( &m_TopList, FALSE, fElapsedTime ) )	return E_FAIL;

	//UpdateActionMsgQ();

	// MEMO : ������ ������ ���� ����
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
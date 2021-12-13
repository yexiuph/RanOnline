#include "pch.h"
#include "HeadChatDisplayMan.h"
#include "HeadChatDisplay.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const float	CHeadChatDisplayMan::fVISIBLE_TIME = 8.0f;

CHeadChatDisplayMan::CHeadChatDisplayMan()
	: m_CurID( 0 )
{
}

CHeadChatDisplayMan::~CHeadChatDisplayMan()
{
	m_mapChat.clear();
}

void CHeadChatDisplayMan::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIControl::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( !IsVisible () ) return ;

	std::vector<UIGUID> vecDELETE_CTRL;
	const CUIControlContainer::UICONTROL_LIST& vec = m_ControlContainer.GetControl ();
	CUIControlContainer::UICONTROL_LIST_CITER citer = vec.begin ();
	CUIControlContainer::UICONTROL_LIST_CITER citer_end = vec.end ();

	for ( ; citer != citer_end; ++citer )
	{
		CUIControl* pControl = (*citer);

		//	보이지 않는 컨트롤은 업데이트 하지 않는다.
		if ( !pControl->IsVisible () )	continue;

		pControl->Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
		UIGUID ControlID = pControl->GetWndID ();	
		DWORD dwMsg = pControl->GetMessageEx ();

		if ( CHECK_KEYFOCUSED ( dwMsg ) )	CUIControl::AddMessageEx ( UIMSG_KEY_FOCUSED );
		if ( CHECK_TOPARENT ( dwMsg ) )		CUIControl::AddMessageEx ( CUT_LOW24BIT ( dwMsg ) );

		if ( bFirstControl )
		{
			NS_UIDEBUGSET::BlockBegin ();
			if ( dwMsg && ( ControlID != NO_ID ) ) TranslateUIMessage ( ControlID, dwMsg );
			NS_UIDEBUGSET::BlockEnd ();
		}

		CHeadChatDisplay* pHeadChatDisplay = (CHeadChatDisplay*)pControl;
		float fLifeTime = pHeadChatDisplay->GetLifeTime ();
		if ( fLifeTime < 0.0f )
		{
			m_HeadChatPool.ReleaseNonInit( pHeadChatDisplay );
			
			CHATMAP_ITER iter = m_mapChat.find ( pHeadChatDisplay->GetName() );
			if ( iter != m_mapChat.end () )
			{
				m_mapChat.erase ( iter );
			}

			vecDELETE_CTRL.push_back ( pControl->GetWndID() );
		}
	}

	for ( size_t i=0; i<vecDELETE_CTRL.size(); ++i )
	{
        DeleteControl ( vecDELETE_CTRL[i], 0 );
	}
}


HRESULT CHeadChatDisplayMan::InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;
	return CUIGroup::InitDeviceObjects( pd3dDevice );
}

void CHeadChatDisplayMan::AddChat ( const CString& strName, const D3DCOLOR& dwIDColor, const CString& strChat, const D3DCOLOR& dwChatColor, const D3DXVECTOR2& vPos )
{
	CHATMAP_ITER iter = m_mapChat.find ( strName );
	if ( iter != m_mapChat.end () )
	{
		m_HeadChatPool.ReleaseNonInit( (*iter).second );

		UIGUID ControlID = ((*iter).second)->GetWndID ();
		DeleteControl ( ControlID, 0 );
		m_mapChat.erase ( iter );
	}

	CHeadChatDisplay* pHeadChatDisplay = MAKE_HEADCHATDISPLAY();

	pHeadChatDisplay->SetLifeTime( fVISIBLE_TIME );
	pHeadChatDisplay->SetChat( strName, dwIDColor, strChat, dwChatColor );
	pHeadChatDisplay->SetGlobalPos( vPos );

	m_mapChat.insert ( std::make_pair(strName,pHeadChatDisplay) );
}

CHeadChatDisplay* CHeadChatDisplayMan::MAKE_HEADCHATDISPLAY()
{
	CHeadChatDisplay* pHeadChatDisplay = m_HeadChatPool.New();

	if( !pHeadChatDisplay->IsMemPool() )
	{
		if ( BASE_CONTROL_ID_END <= m_CurID ) m_CurID = BASE_CONTROL_ID;
		else m_CurID += BASE_CONTROL_ID;

		pHeadChatDisplay->SetMemPool();
		pHeadChatDisplay->CreateSub( this, "HEAD_CHAT_DISPLAY", UI_FLAG_XSIZE | UI_FLAG_YSIZE, m_CurID );
		pHeadChatDisplay->CreateSubControl();
		pHeadChatDisplay->InitDeviceObjects( m_pd3dDevice );
		pHeadChatDisplay->RestoreDeviceObjects( m_pd3dDevice );
	}

	RegisterControl ( pHeadChatDisplay );
	return pHeadChatDisplay;
}

bool CHeadChatDisplayMan::DeleteControl( UIGUID ControlID, int refactoring )
{
	return m_ControlContainer.EraseControl( ControlID, FALSE );
}
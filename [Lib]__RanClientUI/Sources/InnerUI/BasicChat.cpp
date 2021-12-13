#include "pch.h"
#include "BasicChat.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "BasicChatLeftBar.h"
#include "BasicChatRightBody.h"
#include "../[Lib]__EngineUI/Sources/UIMan.h"
#include "ChatShowFlag.h"
#include "../[Lib]__EngineUI/Sources/BasicButton.h"
#include "InnerInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CBasicChat::CBasicChat()
	: m_bCheckButton( FALSE )
{
}

CBasicChat::~CBasicChat()
{
}

void CBasicChat::CreateSubControl ()
{
	CD3DFontPar* pFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	CBasicChatLeftBar* pBasicChatLeftBar = new CBasicChatLeftBar;
	pBasicChatLeftBar->CreateSub ( this, "CHAT_LEFT_BAR", UI_FLAG_YSIZE, LEFT_BAR );
	pBasicChatLeftBar->CreateSubControl ();
	pBasicChatLeftBar->SetProtectSizePoint ();
	RegisterControl ( pBasicChatLeftBar );
	m_pBasicChatLeftBar = pBasicChatLeftBar;

	CBasicChatRightBody* pBasicChatRightBody = new CBasicChatRightBody;
    pBasicChatRightBody->CreateSub ( this, "CHAT_RIGHT_BODY", UI_FLAG_YSIZE, RIGHT_BODY );
	pBasicChatRightBody->CreateSubControl ( pFont9 );
	pBasicChatRightBody->SetProtectSizePoint ();
	RegisterControl ( pBasicChatRightBody );
	m_pBasicChatRightBody = pBasicChatRightBody;

	CUIChatShowFlag* pUIChatShowFlag = new CUIChatShowFlag;
	pUIChatShowFlag->CreateSub ( this, "CHAT_SHOW_FLAG", UI_FLAG_BOTTOM, CHAT_SHOW_FLAG );
	pUIChatShowFlag->CreateSubControl ();
	pUIChatShowFlag->SetProtectSizePoint ();
	pUIChatShowFlag->SetVisibleSingle ( FALSE );
	RegisterControl ( pUIChatShowFlag );
	m_pUIChatShowFlag = pUIChatShowFlag;

	CBasicButton* pButton = new CBasicButton;
	pButton->CreateSub ( this, "CHAT_FLAG_BUTTON", UI_FLAG_BOTTOM, CHAT_FLAG_BUTTON );
	pButton->CreateFlip ( "CHAT_FLAG_BUTTON_F", CBasicButton::CLICK_FLIP );
	pButton->CreateMouseOver ( "CHAT_FLAG_BUTTON_F" );
	pButton->SetUseGlobalAction ( TRUE );
	RegisterControl ( pButton );
	m_pChatShowFlagButton = pButton;
}

void CBasicChat::AddChat ( const CString& strName, const CString& strMsg, const WORD& wType )
{
	if( m_pBasicChatRightBody )
		m_pBasicChatRightBody->AddChat ( strName, strMsg, wType );
}

void CBasicChat::AddStringToChatEx ( CString strTemp, WORD wType )
{
	if( m_pBasicChatRightBody )
		m_pBasicChatRightBody->AddStringToChatEx ( strTemp, wType );
}

void CBasicChat::AddStringToSystemMessage ( CString strTemp, D3DCOLOR dwColor )
{
	if( m_pBasicChatRightBody )
		m_pBasicChatRightBody->AddStringToSystemMessage ( strTemp, dwColor );
}

void CBasicChat::AddChatMacro (CString strMsg )
{
	if( m_pBasicChatRightBody )
		m_pBasicChatRightBody->AddChatMacro(strMsg);
}

void CBasicChat::ChatLog( bool bChatLog, int nChatLogType )
{
	if( m_pBasicChatRightBody )
		m_pBasicChatRightBody->ChatLog( bChatLog, nChatLogType );
}
CString CBasicChat::GET_RECORD_CHAT()
{
	if( !m_pBasicChatRightBody ) return NULL;

	return m_pBasicChatRightBody->GET_RECORD_CHAT();
}

void CBasicChat::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	m_PosX = x;
	m_PosY = y;

	if ( m_pUIChatShowFlag->IsVisible () )	m_pBasicChatRightBody->SetFreezeControl( true );
	else									m_pBasicChatRightBody->SetFreezeControl( false );

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if ( IsExclusiveSelfControl() )
	{
		const float fTOP = y + m_vGap.y;
		if ( fTOP <= 0.0f ) return ;

		const UIRECT& rcGlobalOldPos = GetGlobalPos();

		UIRECT rcGlobalNewPos = rcGlobalOldPos;
		rcGlobalNewPos.top = fTOP;
		rcGlobalNewPos.sizeY = rcGlobalOldPos.bottom - fTOP;

		const D3DXVECTOR2& vProtectSize = GetProtectSize();
		if( rcGlobalNewPos.sizeY <= vProtectSize.y ) return;

		WORD wFlag = GetAlignFlag();
		SetAlignFlag ( UI_FLAG_YSIZE );

		AlignSubControl( rcGlobalOldPos, rcGlobalNewPos );

		SetGlobalPos( rcGlobalNewPos );

		SetAlignFlag ( wFlag );
	}

	m_pBasicChatRightBody->CLASSIFY_CHATTYPE();
	int nChatType = m_pBasicChatRightBody->GetCHATTYPE ();
	switch ( nChatType )
	{
	case CHAT_NORMAL:	m_pBasicChatLeftBar->DO_CHAT_STATE( 0 ); break;
	case CHAT_PRIVATE:	m_pBasicChatLeftBar->DO_CHAT_STATE( 1 ); break;
	case CHAT_PARTY:	m_pBasicChatLeftBar->DO_CHAT_STATE( 2 ); break;
	case CHAT_TOALL:	m_pBasicChatLeftBar->DO_CHAT_STATE( 3 ); break;
	case CHAT_GUILD:	m_pBasicChatLeftBar->DO_CHAT_STATE( 4 ); break;
	case CHAT_ALLIANCE:	m_pBasicChatLeftBar->DO_CHAT_STATE( 5 ); break;
	}

	int nDisplayType = m_pBasicChatRightBody->GetDISPLAYTYPE ();
	switch ( nDisplayType )
	{
	case CBasicChatRightBody::CHAT_CHANNEL_TOTAL:		m_pChatShowFlagButton->SetVisibleSingle ( TRUE ); break;
	case CBasicChatRightBody::CHAT_CHANNEL_PRIVATE:
	case CBasicChatRightBody::CHAT_CHANNEL_PARTY:
	case CBasicChatRightBody::CHAT_CHANNEL_CLUB:
	case CBasicChatRightBody::CHAT_CHANNEL_ALLIANCE:
	case CBasicChatRightBody::CHAT_CHANNEL_SYSTEM:		m_pChatShowFlagButton->SetVisibleSingle ( FALSE ); break;
	}
}

void CBasicChat::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case CHAT_SHOW_FLAG:
		{
			m_pBasicChatRightBody->SetChatFlagState ( CHAT_TOALL );	//	리셋
			if ( UIMSG_CHAT_NORMAL	 & dwMsg ) m_pBasicChatRightBody->AddChatFlagState ( CHAT_NORMAL );
			if ( UIMSG_CHAT_PRIVATE  & dwMsg ) m_pBasicChatRightBody->AddChatFlagState ( CHAT_PRIVATE );
			if ( UIMSG_CHAT_PARTY    & dwMsg ) m_pBasicChatRightBody->AddChatFlagState ( CHAT_PARTY );
			if ( UIMSG_CHAT_CLUB     & dwMsg ) m_pBasicChatRightBody->AddChatFlagState ( CHAT_GUILD );
			if ( UIMSG_CHAT_ALLIANCE & dwMsg ) m_pBasicChatRightBody->AddChatFlagState ( CHAT_ALLIANCE );
			if ( UIMSG_CHAT_SYSTEM   & dwMsg ) m_pBasicChatRightBody->AddChatFlagState ( CHAT_SYSTEM );

			if ( !CHECK_MOUSE_IN ( dwMsg ) )
			{
				if (UIMSG_LB_DOWN & dwMsg )
				{
					if ( m_pUIChatShowFlag->IsVisible () )
					{
						//m_ActionMsgQ.PostUIMessage ( CHAT_SHOW_FLAG, UIMSG_INVISIBLE  );
						m_pUIChatShowFlag->SetVisibleSingle( FALSE );
						m_pBasicChatRightBody->SetVisibleChatButton( TRUE );
						m_bCheckButton = TRUE;
					}
				}
			}
			else
			{
				//	캐릭터 움직임 막음
				AddMessageEx ( UIMSG_MOUSEIN_BLOCK_CHARACTER_MOVE );
			}
		}
		break;

	case LEFT_BAR:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )		//	컨트롤 내부에 마우스가 위치하고,
			{
				//	캐릭터 움직임 막음
				AddMessageEx ( UIMSG_MOUSEIN_BLOCK_CHARACTER_MOVE );

				if ( (dwMsg & UIMSG_MOUSEIN_LEFTBAR_DRAG) )	//	마우스를 좌클릭으로 눌렀을 때,
				{			
					SetExclusiveControl();	//	단독 컨트롤로 등록하고,
					if ( !m_bFirstGap )				//	최초 포지션 갭을 기록한다.
					{
						UIRECT rcPos = GetGlobalPos ();
						m_vGap.x = rcPos.left - m_PosX;
						m_vGap.y = rcPos.top - m_PosY;
						m_bFirstGap = TRUE;
					}
				}
				else if ( CHECK_LB_UP_LIKE ( dwMsg ) )	//	좌클릭했던것을 놓으면
				{
					ResetExclusiveControl();	//	단독 컨트롤을 해제하고
					m_bFirstGap = FALSE;				//	최초 포지션갭을 해제한다.
				}

				if ( UIMSG_MOUSEIN_NORMAL_CHAT & dwMsg )
				{
					BEGIN_NORMAL_CHAT();
					return ;
				}

				if ( UIMSG_MOUSEIN_PRIVATE_CHAT & dwMsg )
				{
					BEGIN_PRIVATE_CHAT();
					return ;
				}

				if ( UIMSG_MOUSEIN_PARTY_CHAT & dwMsg )
				{
					BEGIN_PARTY_CHAT();
					return ;
				}

				if ( UIMSG_MOUSEIN_TOALL_CHAT & dwMsg )
				{
					BEGIN_TOALL_CHAT();
					return ;
				}

				if ( UIMSG_MOUSEIN_GUILD_CHAT & dwMsg )
				{
					BEGIN_GUILD_CHAT();
					return ;
				}

				if ( UIMSG_MOUSEIN_ALLIANCE_CHAT & dwMsg )
				{
					BEGIN_ALLIANCE_CHAT();
					return ;
				}
			}
			else if ( CHECK_LB_UP_LIKE ( dwMsg ) )		//	컨트롤 영역밖에서 버튼을 떼는 경우가
			{											//	발생하더라도
				ResetExclusiveControl();		//	단독 컨트롤을 해제하고,
				m_bFirstGap = FALSE;					//	최초 포지션갭을 해제한다.
			}
		}
		break;

	case CHAT_FLAG_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( UIMSG_LB_DOWN & dwMsg )
				{
					if ( !m_pUIChatShowFlag->IsVisible () && !m_bCheckButton )
					{
						//m_ActionMsgQ.PostUIMessage ( CHAT_SHOW_FLAG, UIMSG_VISIBLE );
						m_pBasicChatRightBody->SetVisibleChatButton( FALSE );
						m_pUIChatShowFlag->SetVisibleSingle( TRUE );
					}

					m_bCheckButton = FALSE;
				}
			}
		}	
		break;
	case RIGHT_BODY:
		{
			if ( UIMSG_MOUSEIN_RIGHTBODY & dwMsg )
			{
				//	캐릭터 움직임 막음
				AddMessageEx ( UIMSG_MOUSEIN_BLOCK_CHARACTER_MOVE );
			}
		}
		break;
	}

	return ;
}

HRESULT CBasicChat::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	hr = CUIGroup::RestoreDeviceObjects ( pd3dDevice );
	if ( FAILED ( hr ) ) return hr;

	const long lResolution = CUIMan::GetResolution();
	WORD X_RES = HIWORD( lResolution );
	WORD Y_RES = LOWORD( lResolution );

	static INT nY_RES_BACK;

	/*
#ifdef CH_PARAM // 중국 인터페이스 변경

	UIRECT rcOldPos = GetGlobalPos();
	UIRECT rcNewPos = rcOldPos;

	WORD CH_Y_RES = Y_RES - 41;

	if( ( CH_Y_RES < rcOldPos.sizeY ) && ( CH_Y_RES > 60 ) )
	{ 
		WORD wAlignFlag = GetAlignFlag ();
		SetAlignFlag ( UI_FLAG_YSIZE );

		rcNewPos.sizeY = rcOldPos.sizeY - ( nY_RES_BACK - Y_RES );

		AlignSubControl( rcOldPos, rcNewPos );

		SetAlignFlag( wAlignFlag );
	}

	rcNewPos.top = CH_Y_RES - rcNewPos.sizeY;
	rcNewPos.bottom = rcNewPos.top + rcNewPos.sizeY;

	SetGlobalPos( rcNewPos );

#else
*/
	const UIRECT& rcWindow = GetGlobalPos ();

	if( ( Y_RES < rcWindow.sizeY ) && ( Y_RES > 60 ) ) // Note : 리사이즈, 60은 채팅 박스의 최소 사이즈
	{ 
		WORD wAlignFlag = GetAlignFlag ();
		SetAlignFlag ( UI_FLAG_YSIZE );

		const UIRECT& rcOldPos = GetLocalPos();
		UIRECT rcNewPos = rcOldPos;
		rcNewPos.sizeY = rcOldPos.sizeY - ( nY_RES_BACK - Y_RES );

		AlignSubControl( rcOldPos, rcNewPos );
		SetLocalPos( D3DXVECTOR2( rcNewPos.left, rcNewPos.top ) );

		SetGlobalPos( rcNewPos );
		SetAlignFlag( wAlignFlag );
	}
	else
	{
		D3DXVECTOR2 vPos( 0.0f, 0.0f );
		vPos.y = Y_RES - rcWindow.sizeY;
		SetGlobalPos( vPos );
	}

//#endif

	nY_RES_BACK = Y_RES;

	return S_OK;
}

void	CBasicChat::BEGIN_NORMAL_CHAT ( const CString& strMessage )
{	
	m_pBasicChatRightBody->SetIgnoreState ( true );
	m_pBasicChatRightBody->BEGIN_NORMAL_CHAT ( strMessage );
	m_pBasicChatRightBody->SetIgnoreState ( false );
}

void	CBasicChat::BEGIN_PRIVATE_CHAT ( const CString& strName, const CString& strMessage )
{
	m_pBasicChatRightBody->SetIgnoreState ( true );
	m_pBasicChatRightBody->BEGIN_PRIVATE_CHAT ( strName, strMessage );
	m_pBasicChatRightBody->SetIgnoreState ( false );
}

void	CBasicChat::BEGIN_PARTY_CHAT ( const CString& strMessage )
{
	m_pBasicChatRightBody->SetIgnoreState ( true );
	m_pBasicChatRightBody->BEGIN_PARTY_CHAT ( strMessage );
	m_pBasicChatRightBody->SetIgnoreState ( false );
}

void	CBasicChat::BEGIN_TOALL_CHAT ( const CString& strMessage )
{
	m_pBasicChatRightBody->SetIgnoreState ( true );
	m_pBasicChatRightBody->BEGIN_TOALL_CHAT ( strMessage );
}

void	CBasicChat::BEGIN_GUILD_CHAT ( const CString& strMessage )
{
	m_pBasicChatRightBody->SetIgnoreState ( true );
	m_pBasicChatRightBody->BEGIN_GUILD_CHAT ( strMessage );
	m_pBasicChatRightBody->SetIgnoreState ( false );
}

void	CBasicChat::BEGIN_ALLIANCE_CHAT ( const CString& strMessage )
{
	m_pBasicChatRightBody->SetIgnoreState ( true );
	m_pBasicChatRightBody->BEGIN_ALLIANCE_CHAT ( strMessage );
	m_pBasicChatRightBody->SetIgnoreState ( false );
}

void CBasicChat::REFRESH_FRIEND_LIST ()
{
	if( m_pBasicChatRightBody )
		m_pBasicChatRightBody->ADD_FRIEND_LIST ();
}

void CBasicChat::FRIEND_LIST( CONST CString & strName, bool bOnline )
{
	if( !m_pBasicChatRightBody ) return ;

	if( bOnline )	m_pBasicChatRightBody->ADD_FRIEND( strName );
	else			m_pBasicChatRightBody->DEL_FRIEND( strName );
}

bool CBasicChat::IsCHAT_BEGIN ()
{
	if( !m_pBasicChatRightBody ) return false;
	return m_pBasicChatRightBody->IsEDIT_BEGIN ();
}
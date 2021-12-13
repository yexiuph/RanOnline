#include "pch.h"
#include "BasicChatLeftBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CBasicChatLeftBar::CBasicChatLeftBar ()
{
}

CBasicChatLeftBar::~CBasicChatLeftBar ()
{
}

void CBasicChatLeftBar::CreateSubControl ()
{
	CUIControl* pTop = new CUIControl;
	pTop->CreateSub ( this, "CHAT_LEFT_BAR_TOP", UI_FLAG_DEFAULT, CHAT_LEFT_BAR_TOP );
	pTop->SetProtectSizePoint ();
	RegisterControl ( pTop );

	CUIControl* pBody = new CUIControl;
	pBody->CreateSub ( this, "CHAT_LEFT_BAR_BODY", UI_FLAG_YSIZE );
	pBody->SetProtectSizePoint ();
	RegisterControl ( pBody );

	CUIControl* pBottom = new CUIControl;
	pBottom->CreateSub ( this, "CHAT_LEFT_BAR_BOTTOM", UI_FLAG_BOTTOM );
	pBottom->SetProtectSizePoint ();
	RegisterControl ( pBottom );

	CUIControl* pNormalChat = new CUIControl;
	pNormalChat->CreateSub ( this, "CHAT_NORMAL_STATE", UI_FLAG_BOTTOM, CHAT_NORMAL_STATE );
	RegisterControl ( pNormalChat );
	m_pCHATSTATE[0] = pNormalChat;

	CUIControl* pPrivateChat = new CUIControl;
	pPrivateChat->CreateSub ( this, "CHAT_PRIVATE_STATE", UI_FLAG_BOTTOM, CHAT_PRIVATE_STATE );	
	RegisterControl ( pPrivateChat );
	m_pCHATSTATE[1] = pPrivateChat;

	CUIControl* pPartyChat = new CUIControl;
	pPartyChat->CreateSub ( this, "CHAT_PARTY_STATE", UI_FLAG_BOTTOM, CHAT_PARTY_STATE );	
	RegisterControl ( pPartyChat );
	m_pCHATSTATE[2] = pPartyChat;

	CUIControl* pToallChat = new CUIControl;
	pToallChat->CreateSub ( this, "CHAT_TOALL_STATE", UI_FLAG_BOTTOM, CHAT_TOALL_STATE );	
	RegisterControl ( pToallChat );
	m_pCHATSTATE[3] = pToallChat;

	CUIControl* pGuildChat = new CUIControl;
	pGuildChat->CreateSub ( this, "CHAT_GUILD_STATE", UI_FLAG_BOTTOM, CHAT_GUILD_STATE );	
	RegisterControl ( pGuildChat );
	m_pCHATSTATE[4] = pGuildChat;

	CUIControl* pAllianceChat = new CUIControl;
	pAllianceChat->CreateSub ( this, "CHAT_ALLIANCE_STATE", UI_FLAG_BOTTOM, CHAT_ALLIANCE_STATE );	
	RegisterControl ( pAllianceChat );
	m_pCHATSTATE[5] = pAllianceChat;

	DO_CHAT_STATE( 0 );
}

void CBasicChatLeftBar::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case CHAT_LEFT_BAR_TOP:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( CHECK_LB_DOWN_LIKE ( dwMsg ) )
				{
					AddMessageEx ( UIMSG_MOUSEIN_LEFTBAR_DRAG );
				}
			}
		}
		break;

	case CHAT_NORMAL_STATE:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( UIMSG_LB_DOWN & dwMsg )
				{
					AddMessageEx ( UIMSG_MOUSEIN_PRIVATE_CHAT );
				}
			}			
		}
		break;

	case CHAT_PRIVATE_STATE:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( UIMSG_LB_DOWN & dwMsg )
				{
					AddMessageEx ( UIMSG_MOUSEIN_PARTY_CHAT );
				}
			}			
		}
		break;

	case CHAT_PARTY_STATE:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( UIMSG_LB_DOWN & dwMsg )
				{
					AddMessageEx ( UIMSG_MOUSEIN_TOALL_CHAT );
				}
			}			
		}
		break;

	case CHAT_TOALL_STATE:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( UIMSG_LB_DOWN & dwMsg )
				{
					AddMessageEx ( UIMSG_MOUSEIN_GUILD_CHAT );
				}
			}
		}
		break;

	case CHAT_GUILD_STATE:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( UIMSG_LB_DOWN & dwMsg )
				{
					AddMessageEx ( UIMSG_MOUSEIN_ALLIANCE_CHAT );
				}
			}
		}
		break;

	case CHAT_ALLIANCE_STATE:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( UIMSG_LB_DOWN & dwMsg )
				{
					AddMessageEx ( UIMSG_MOUSEIN_NORMAL_CHAT );
				}
			}
		}
		break;
	}
}

void CBasicChatLeftBar::DO_CHAT_STATE ( int nSTATE )
{
	if ( nSTATE < 0 || TOTAL_CHAT_STATE <= nSTATE )
		return ;

	for ( int i = 0; i < TOTAL_CHAT_STATE; ++i )
	{
		m_pCHATSTATE[i]->SetVisibleSingle ( FALSE );
	}

	m_pCHATSTATE[nSTATE]->SetVisibleSingle ( TRUE );
}
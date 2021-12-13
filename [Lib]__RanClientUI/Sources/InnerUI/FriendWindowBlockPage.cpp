#include "pch.h"
#include "FriendWindowBlockPage.h"
#include "FriendWindow.h"
#include "BasicTextBoxEx.h"
#include "BasicScrollBarEx.h"
#include "../[Lib]__EngineUI/Sources/BasicScrollThumbFrame.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "GameTextControl.h"
#include "BasicTextButton.h"
#include "../[Lib]__EngineUI/Sources/UIEditBox.h"
#include "BasicLineBox.h"
#include "InnerInterface.h"
#include "ModalWindow.h"
#include "UITextControl.h"
#include "ModalCallerID.h"
#include "BasicChat.h"
#include "../[Lib]__EngineUI/Sources/UIKeyCheck.h"
#include "GLGaeaClient.h"
#include "DXInputString.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const int CFriendWindowBlockPage::nLIMIT_CHAR = 16;

CFriendWindowBlockPage::CFriendWindowBlockPage()
	: m_nSelectIndex( -1 )
	, m_pListText(NULL)
	, m_pListScrollBar(NULL)
	, m_pADD_BLOCK(NULL)
	, m_pDEL_BLOCK(NULL)
	, m_pEditBox(NULL)
{
}

CFriendWindowBlockPage::~CFriendWindowBlockPage ()
{
}

void CFriendWindowBlockPage::CreateSubControl ()
{
	CD3DFontPar* pFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	//	텍스트 박스
	{
		CBasicTextBoxEx* pTextBox = new CBasicTextBoxEx;
		pTextBox->CreateSub ( this, "FRIEND_LIST_TEXTBOX_BLOCK", UI_FLAG_DEFAULT, FRIEND_LIST_TEXTBOX );
		pTextBox->SetFont ( pFont9 );		
		pTextBox->SetLineInterval ( 3.0f );
		pTextBox->SetSensitive ( true );
		pTextBox->SetLimitLine ( 10000 );
		RegisterControl ( pTextBox );
		m_pListText = pTextBox;

		int nTotalLine = pTextBox->GetVisibleLine ();

		//	스크롤바
		CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx;
		pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE, FRIEND_LIST_SCROLLBAR );
		pScrollBar->CreateBaseScrollBar ( "FRIEND_LIST_SCROLLBAR_BLOCK" );
		pScrollBar->GetThumbFrame()->SetState ( 1, nTotalLine );
		RegisterControl ( pScrollBar );
		m_pListScrollBar = pScrollBar;
	}

	m_pADD_BLOCK = CreateTextButton14 ( "FRIEND_LIST_ADD_BLOCK_BUTTON", FRIEND_LIST_ADD_BLOCK_BUTTON, (char*)ID2GAMEWORD("FRIEND_BUTTON",4) );
	m_pADD_BLOCK->SetShortcutKey ( DIK_RETURN );
	m_pDEL_BLOCK = CreateTextButton14 ( "FRIEND_LIST_DEL_BLOCK_BUTTON", FRIEND_LIST_DEL_BLOCK_BUTTON, (char*)ID2GAMEWORD("FRIEND_BUTTON",3) );

	CBasicLineBox* pBasicLineBox = new CBasicLineBox;
	pBasicLineBox->CreateSub ( this, "BASIC_LINE_BOX_EDIT", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pBasicLineBox->CreateBaseBoxEditBox ( "FRIEND_EDIT_BOX_BACK_BLOCK" );
	RegisterControl ( pBasicLineBox );

	CUIEditBox* pEditBox = new CUIEditBox;
	pEditBox->CreateSub ( this, "FRIEND_EDIT_BOX_BLOCK", UI_FLAG_XSIZE | UI_FLAG_BOTTOM, FRIEND_EDITBOX_BLOCK );
	pEditBox->CreateCarrat ( "FRIEND_CARRAT", TRUE, UINT_MAX );
	pEditBox->SetLimitInput ( nLIMIT_CHAR );
	pEditBox->SetFont ( pFont9 );		
	RegisterControl ( pEditBox );
	m_pEditBox = pEditBox;

	LoadBlockList ();
}

CBasicTextButton* CFriendWindowBlockPage::CreateTextButton14 ( char* szButton, UIGUID ControlID , char* szText )
{
	const int nBUTTONSIZE = CBasicTextButton::SIZE14;
	CBasicTextButton* pButton = new CBasicTextButton;
	pButton->CreateSub ( this, "BASIC_TEXT_BUTTON14", UI_FLAG_XSIZE, ControlID );
	pButton->CreateBaseButton ( szButton, nBUTTONSIZE, CBasicButton::CLICK_FLIP, szText );
	RegisterControl ( pButton );
	return pButton;
}

void CFriendWindowBlockPage::LoadBlockList ()
{
	//	친구 목록을 갱신한다.
	m_pListText->ClearText ();

	GLFriendClient::FRIENDMAP& FriendMap = GLFriendClient::GetInstance().GetFriend();

	{
		GLFriendClient::FRIENDMAP_ITER iter = FriendMap.begin ();
		GLFriendClient::FRIENDMAP_ITER iter_end = FriendMap.end ();	
		for ( ; iter != iter_end; ++iter )
		{
			SFRIEND& sFriend = (*iter).second;			
			if ( sFriend.IsBLOCK() && sFriend.bONLINE )
				LoadBlockFriend ( sFriend );
		}
	}

	{
		GLFriendClient::FRIENDMAP_ITER iter = FriendMap.begin ();
		GLFriendClient::FRIENDMAP_ITER iter_end = FriendMap.end ();	
		for ( ; iter != iter_end; ++iter )
		{
			SFRIEND& sFriend = (*iter).second;			
			if ( sFriend.IsBLOCK() && !sFriend.bONLINE ) 
				LoadBlockFriend ( sFriend );
		}
	}

	m_pListText->SetCurLine ( 0 );

	const int nTotal = m_pListText->GetCount ();
	const int nViewPerPage = m_pListText->GetVisibleLine ();	
	m_pListScrollBar->GetThumbFrame()->SetState ( nTotal, nViewPerPage );
	m_pListScrollBar->GetThumbFrame()->SetPercent ( 0.0f );
}

void CFriendWindowBlockPage::LoadBlockFriend ( const SFRIEND& sFriend )
{
	const CString& strName = sFriend.szCharName;
	const bool bOnline = sFriend.bONLINE;

	enum { ONLINE = 0, OFFLINE };

	static	CString strKeyword[] =
	{
		"FRIEND_ONLINE",
		"FRIEND_OFFLINE"
	};

	D3DCOLOR dwTextColor;
	int nIcon;

	//if( bOnline )
	//{
	//	dwTextColor = NS_UITEXTCOLOR::WHITE;
	//	nIcon = ONLINE;
	//}
	//else
	{
		dwTextColor = NS_UITEXTCOLOR::DARKGRAY;
		nIcon = OFFLINE;
	}

	int nIndex = m_pListText->AddText ( strName, dwTextColor );	
	m_pListText->SetTextImage ( nIndex, strKeyword[nIcon] );
	m_pListText->SetTextData ( nIndex, sFriend.nCharID );
}

void CFriendWindowBlockPage::TranslateUIMessage ( UIGUID cID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( cID, dwMsg );

	switch ( cID )
	{
	case FRIEND_LIST_TEXTBOX:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				const int nIndex = m_pListText->GetSelectPos ();
				if ( nIndex < 0 || m_pListText->GetCount () <= nIndex ) return ;

				m_pListText->SetUseOverColor ( TRUE );
				m_pListText->SetOverColor ( nIndex, NS_UITEXTCOLOR::DARKORANGE );

				if( UIMSG_LB_UP & dwMsg )
				{
					m_nSelectIndex = nIndex;
				
					int nCharID = m_pListText->GetTextData ( m_nSelectIndex );
					m_strSelectName = GetFriend ( nCharID );
				}

				//if ( UIMSG_LB_DUP & dwMsg )
				//{
				//	m_pEditBox->SetEditString ( m_strSelectName );

				//	//SFRIEND* pFriend = GLFriendClient::GetInstance().GetFriend ( m_strSelectName.GetString() );
				//	//if ( pFriend && pFriend->bONLINE ) CInnerInterface::GetInstance().PostUIMessage ( GetWndID (), UIMSG_ADDNAME_TO_CHATBOX );
				//}
			}
		}
		break;

	case FRIEND_EDITBOX_BLOCK:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( UIMSG_LB_UP & dwMsg )
				{
					CString strName = m_pEditBox->GetEditString ();
					EDIT_BEGIN ( strName );				
				}
			}
			else
			{
				if ( UIMSG_LB_DOWN & dwMsg || UIMSG_LB_UP & dwMsg )
					EDIT_END ();
			}
		}
		break;

	case FRIEND_LIST_ADD_BLOCK_BUTTON:
		{
			bool bKEY_FOCUSED = false;
			if ( CHECK_KEYFOCUSED ( dwMsg ) )
			{
				if ( !m_pEditBox->IsBegin () )
				{
					//	RETURN 무시
					UIKeyCheck::GetInstance()->RemoveCheck ( DIK_RETURN, DXKEY_DOWN );

					dwMsg &= ~UIMSG_KEY_FOCUSED;
					SetMessageEx ( dwMsg );
					return ;
				}

				bKEY_FOCUSED = true;
			}

			if ( bKEY_FOCUSED || CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				if ( CHECK_KEYFOCUSED(dwMsg) &&
					!DXInputString::GetInstance().CheckEnterKeyDown() )	return;

				if ( !m_pEditBox->GetEditLength () ) return;

				CString strName = m_pEditBox->GetEditString();
				ADD_BLOCK ( strName );

				SetMessageEx( dwMsg &= ~UIMSG_KEY_FOCUSED ); // Focus 문제로 메세지 삭제

				//	FriendWindowNormalPage에 선언된,
				//	UIMSG_EDIT_END과 같은 메시지를 보내준다.
				//	이 메시지는 에디트가 종료(캐럿을 죽임)되는 것을 목적으로 한다.
				//CInnerInterface::GetInstance().PostUIMessage ( FRIEND_WINDOW, UIMSG_EDIT_END );
				GetParent()->AddMessageEx( UIMSG_EDIT_END );
			}
		}
		break;

	case FRIEND_LIST_DEL_BLOCK_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				if ( m_nSelectIndex < 0 )
				{
					CInnerInterface::GetInstance().PrintMsgText (
						NS_UITEXTCOLOR::RED,
						ID2GAMEINTEXT("FRIEND_BLOCK_DEL_INCORRECT") );
					return ;
				}

				int nCharID = m_pListText->GetTextData ( m_nSelectIndex );
				CString strName = GetFriend ( nCharID );

				DEL_BLOCK ( strName );
			}
		}
		break;
	}
}


void CFriendWindowBlockPage::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	m_pListText->SetUseOverColor ( FALSE );
	if ( 0 <= m_nSelectIndex && m_nSelectIndex < m_pListText->GetCount () )
		m_pListText->SetUseTextColor ( m_nSelectIndex, FALSE );

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );


	if ( 0 <= m_nSelectIndex && m_nSelectIndex < m_pListText->GetCount () )
	{
		m_pListText->SetUseTextColor ( m_nSelectIndex, TRUE );
		m_pListText->SetTextColor ( m_nSelectIndex, NS_UITEXTCOLOR::ORANGE );
	}

	{
		CBasicScrollThumbFrame* const pThumbFrame = m_pListScrollBar->GetThumbFrame ();

		const int nTotalLine = m_pListText->GetTotalLine ();
		const int nLinePerOneView = m_pListText->GetVisibleLine ();
		CDebugSet::ToView ( 1, 10, "보이는 라인 %d", nLinePerOneView );
		pThumbFrame->SetState ( nTotalLine, nLinePerOneView );
		if ( nLinePerOneView < nTotalLine )
		{
			const int nMovableLine = nTotalLine - nLinePerOneView;
			float fPercent = pThumbFrame->GetPercent ();
			int nPos = (int)floor(fPercent * nMovableLine);
			m_pListText->SetCurLine ( nPos );			
		}
	}
}

void CFriendWindowBlockPage::SetVisibleSingle ( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle ( bVisible );

	if ( bVisible )
	{
		LoadBlockList ();
		m_nSelectIndex = -1;
	}
	else
	{
		EDIT_END ();
	}
}

CString	CFriendWindowBlockPage::GetFriend ( const int nCharID )
{
	GLFriendClient::FRIENDMAP& FriendMap = GLFriendClient::GetInstance().GetFriend();

	GLFriendClient::FRIENDMAP_ITER iter = FriendMap.begin ();
	GLFriendClient::FRIENDMAP_ITER iter_end = FriendMap.end ();	
	for ( ; iter != iter_end; ++iter )
	{
		SFRIEND& sFriend = (*iter).second;
		if ( sFriend.nCharID == nCharID )
		{
			return sFriend.szCharName;
		}
	}

	static CString strNULL;
	return strNULL;
}


void CFriendWindowBlockPage::EDIT_BEGIN ( const CString& strName )
{
	m_pEditBox->EndEdit ();
	m_pEditBox->ClearEdit ();

	m_pEditBox->SetEditString ( strName );
	m_pEditBox->BeginEdit ();	
}

void CFriendWindowBlockPage::EDIT_END ()
{
	m_pEditBox->EndEdit ();
}

void CFriendWindowBlockPage::ADD_NAME_TO_EDITBOX ( const CString& strName )
{
	m_pEditBox->SetEditString ( strName );
}

void CFriendWindowBlockPage::ADD_BLOCK ( const CString& strName )
{
	if ( !strName.GetLength () ) return ;

	m_strSelectName = strName;

	SFRIEND* pFriend = GLFriendClient::GetInstance().GetFriend (m_strSelectName.GetString());
	if ( pFriend && pFriend->IsBLOCK () )
	{
		CInnerInterface::GetInstance().PrintMsgText ( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("FRIEND_BLOCK_ADD_ALREADY"), strName );
		return ;
	}

	INT nTotalLine = GLFriendClient::GetInstance().GetFriendSize( EMFRIEND_BLOCK );
	if( nTotalLine >= GLCONST_CHAR::nMAX_FRIEND_NUMBER )
	{
		CInnerInterface::GetInstance().PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT("FRIEND_ADD_MAX"), GLCONST_CHAR::nMAX_FRIEND_NUMBER );
		return ;
	}

	CString strCombine = CInnerInterface::GetInstance().MakeString ( ID2GAMEINTEXT("FRIEND_BLOCK_ADD"), strName );
	DoModal ( strCombine, MODAL_QUESTION, YESNO, MODAL_FRIEND_BLOCK_ADD_B );
}

void CFriendWindowBlockPage::DEL_BLOCK ( const CString& strName )
{
	if ( !strName.GetLength () ) return ;

	m_strSelectName = strName;

	CString strCombine = CInnerInterface::GetInstance().MakeString ( ID2GAMEINTEXT("FRIEND_BLOCK_DEL"), strName );
	DoModal ( strCombine, MODAL_QUESTION, YESNO, MODAL_FRIEND_BLOCK_DEL );
}

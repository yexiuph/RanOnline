#include "pch.h"
#include "BasicChatRightBody.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "DxInputDevice.h"
#include "BasicTextBoxEx.h"
#include "BasicScrollBarEx.h"
#include "../[Lib]__EngineUI/Sources/BasicScrollThumbFrame.h"
#include "DxGlobalStage.h"
#include "GLGaeaClient.h"
#include "UITextControl.h"
#include "dxincommand.h"
#include "InnerInterface.h"
#include "HeadChatDisplayMan.h"
#include "DxViewPort.h"
#include "SystemMessageDisplay.h"
#include "GLPartyClient.h"
#include "GameTextControl.h"
#include "DxInputString.h"
#include "../[Lib]__EngineUI/Sources/BasicButton.h"
#include "RanFilter.h"
#include "GLFriendClient.h"
#include "../[Lib]__EngineUI/Sources/UIKeyCheck.h"
#include "BasicTextButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const int CBasicChatRightBody::nLIMITCHAR = CHAT_MSG_SIZE - 1;
const int CBasicChatRightBody::nSTARTLINE = 0;
const int CBasicChatRightBody::nLIMITLINE = 200;
const D3DCOLOR	CBasicChatRightBody::dwCLICKCOLOR = D3DCOLOR_ARGB(140,255,255,255);

const FLOAT	CBasicChatRightBody::fLIMIT_TIME = 30.0f;
const FLOAT CBasicChatRightBody::fINTERVAL_INPUT = 2.0f;
const INT	CBasicChatRightBody::nCOUNT_INPUT = 7;

const CString CBasicChatRightBody::BLANK_SYMBOL( _T(" ") );
const CString CBasicChatRightBody::PRIVATE_SYMBOL( _T("@") );
const CString CBasicChatRightBody::PARTY_SYMBOL( _T("#") );
const CString CBasicChatRightBody::TOALL_SYMBOL( _T("$") );
const CString CBasicChatRightBody::GUILD_SYMBOL( _T("%") );
const CString CBasicChatRightBody::ALLIANCE_SYMBOL( _T("!") );
const CString CBasicChatRightBody::SYSTEM_SYMBOL( _T("&") );

CBasicChatRightBody::CBasicChatRightBody () :
	m_pTextBox_ALL ( NULL ),
	m_pScrollBar_ALL ( NULL ),
	m_pEditBox ( NULL ),
	m_wDisplayState ( CHAT_ALL ),
	m_NamePos ( 0 ),
	m_nCHATTYPE ( CHAT_NO ),
	m_nFRIENDSIZE_BACK ( 0 ),
	m_ChatPos ( 0 ),
	m_ChatCountBack ( 0 ),
	m_NameCountBack ( 0 ),
	m_bIgnoreState ( false ),
	m_wDISPLAYTYPE ( CHAT_CHANNEL_TOTAL ),
	m_bFREEZE ( false ),
	m_fLifeTime( 0.0f ),
	m_nInputCount( 0 ),
	m_nTimeCount( 0 ),
	m_bPapering( FALSE ),
	m_bChatLog ( false ),
	m_nChatLogType ( 0 ),
	m_hCheckStrDLL ( NULL ),
	m_pCheckString ( NULL )
{
}

CBasicChatRightBody::~CBasicChatRightBody ()
{
#if defined ( TH_PARAM )
	if ( m_hCheckStrDLL ) FreeLibrary( m_hCheckStrDLL );
#endif 
}

void CBasicChatRightBody::CreateSubControl ( CD3DFontPar*	pFont )
{
	//CUIControl* pLeftBar = new CUIControl;
	//pLeftBar->CreateSub ( this, "CHAT_RIGHT_BODY_LEFT_BAR", UI_FLAG_YSIZE );
	//pLeftBar->SetProtectSizePoint ();
	//pLeftBar->SetNoUpdate ( true );
	//RegisterControl ( pLeftBar );

	CUIControl* pTopBar = new CUIControl;
	pTopBar->CreateSub ( this, "CHAT_RIGHT_BODY_TOP_BAR" );
	pTopBar->SetProtectSizePoint ();
	pTopBar->SetNoUpdate ( true );
	RegisterControl ( pTopBar );

	CUIControl* pMain = new CUIControl;
	pMain->CreateSub ( this, "CHAT_RIGHT_BODY_MAIN", UI_FLAG_YSIZE );
	pMain->SetProtectSizePoint ();
	pMain->SetNoUpdate ( true );
	RegisterControl ( pMain );

	CUIControl* pUnder = new CUIControl;
	pUnder->CreateSub ( this, "CHAT_RIGHT_BODY_UNDER", UI_FLAG_BOTTOM );
	pUnder->SetProtectSizePoint ();
	pUnder->SetNoUpdate ( true );
	RegisterControl ( pUnder );

	CUIControl* pRightBar = new CUIControl;
	pRightBar->CreateSub ( this, "CHAT_RIGHT_BODY_RIGHT_BAR", UI_FLAG_YSIZE | UI_FLAG_RIGHT );
	pRightBar->SetProtectSizePoint ();
	pRightBar->SetNoUpdate ( true );
	RegisterControl ( pRightBar );

	CUIControl*	pChannelBack = new CUIControl;
	pChannelBack->CreateSub ( this, "CHAT_CHANNEL_BACK", UI_FLAG_BOTTOM );
	pChannelBack->SetProtectSizePoint ();
	pChannelBack->SetNoUpdate ( true );
	RegisterControl ( pChannelBack );

	
	m_pChatTotalButton = CreateTextButton ( "CHAT_CHANNEL_TOTAL", CHAT_CHANNEL_TOTAL, const_cast<char*>(ID2GAMEWORD("CHAT_CHANNEL_BUTTON",0)) );
	m_pChatPrivateButton = CreateTextButton ( "CHAT_CHANNEL_PRIVATE", CHAT_CHANNEL_PRIVATE, const_cast<char*>(ID2GAMEWORD("CHAT_CHANNEL_BUTTON",1)) );	
	m_pChatPartyButton = CreateTextButton ( "CHAT_CHANNEL_PARTY", CHAT_CHANNEL_PARTY, const_cast<char*>(ID2GAMEWORD("CHAT_CHANNEL_BUTTON",2)) );
	m_pChatClubButton = CreateTextButton ( "CHAT_CHANNEL_CLUB", CHAT_CHANNEL_CLUB, const_cast<char*>(ID2GAMEWORD("CHAT_CHANNEL_BUTTON",3)) );
	m_pChatSystemButton = CreateTextButton ( "CHAT_CHANNEL_SYSTEM", CHAT_CHANNEL_SYSTEM, const_cast<char*>(ID2GAMEWORD("CHAT_CHANNEL_BUTTON",4)) );
	m_pChatAllianceButton = CreateTextButton ( "CHAT_CHANNEL_ALLIANCE", CHAT_CHANNEL_ALLIANCE, const_cast<char*>(ID2GAMEWORD("CHAT_CHANNEL_BUTTON",5)) );

	int nTotalLine(0);
	int nVisibleLine(0);

	{	//	전체
		m_pTextBox_ALL = CreateTextBoxEx ( pFont, CHAT_TEXTBOX_ALL );
		nTotalLine = m_pTextBox_ALL->GetTotalLine ();
		nVisibleLine = m_pTextBox_ALL->GetVisibleLine ();
		
		m_pScrollBar_ALL = CreateScrollBarEx ( nTotalLine, nVisibleLine );	
	}

	{	//	귓속말
		m_pTextBox_PRIVATE = CreateTextBoxEx ( pFont, CHAT_TEXTBOX_PRIVATE );
		nTotalLine = m_pTextBox_PRIVATE->GetTotalLine ();
		nVisibleLine = m_pTextBox_PRIVATE->GetVisibleLine ();

		m_pScrollBar_PRIVATE = CreateScrollBarEx ( nTotalLine, nVisibleLine );
	}

	{	//	클럽
		m_pTextBox_CLUB = CreateTextBoxEx ( pFont, CHAT_TEXTBOX_CLUB );
		nTotalLine = m_pTextBox_CLUB->GetTotalLine ();
		nVisibleLine = m_pTextBox_CLUB->GetVisibleLine ();

		m_pScrollBar_CLUB = CreateScrollBarEx ( nTotalLine, nVisibleLine );
	}

	{	//	파티
		m_pTextBox_PARTY = CreateTextBoxEx ( pFont, CHAT_TEXTBOX_PARTY );
		nTotalLine = m_pTextBox_PARTY->GetTotalLine ();
		nVisibleLine = m_pTextBox_PARTY->GetVisibleLine ();

		m_pScrollBar_PARTY = CreateScrollBarEx ( nTotalLine, nVisibleLine );	
	}

	{	//	동맹
		m_pTextBox_ALLIANCE = CreateTextBoxEx ( pFont, CHAT_TEXTBOX_ALLIANCE );
		nTotalLine = m_pTextBox_ALLIANCE->GetTotalLine ();
		nVisibleLine = m_pTextBox_ALLIANCE->GetVisibleLine ();

		m_pScrollBar_ALLIANCE = CreateScrollBarEx ( nTotalLine, nVisibleLine );	
	}

	{	//	시스템
		m_pTextBox_SYSTEM = CreateTextBoxEx ( pFont, CHAT_TEXTBOX_SYSTEM );
		nTotalLine = m_pTextBox_SYSTEM->GetTotalLine ();
		nVisibleLine = m_pTextBox_SYSTEM->GetVisibleLine ();

		m_pScrollBar_SYSTEM = CreateScrollBarEx ( nTotalLine, nVisibleLine );	
	}

	{
		CUIControl* pScrollBarDummy = new CUIControl;
		pScrollBarDummy->CreateSub ( this, "BASIC_CHAT_SCROLLBAR_DUMMY" );
		pScrollBarDummy->SetVisibleSingle ( FALSE );
		RegisterControl ( pScrollBarDummy );
		m_pScrollBarDummy = pScrollBarDummy;
	}	

	m_pEditBoxBack = new CUIControl;
	m_pEditBoxBack->CreateSub ( this, "CHAT_EDIT_BOX_BACK", UI_FLAG_BOTTOM );
	m_pEditBoxBack->SetUseRender ( TRUE );
	m_pEditBoxBack->SetDiffuse ( D3DCOLOR_ARGB(70,255,255,255) );
	RegisterControl ( m_pEditBoxBack );

	m_pEditBox = new CUIEditBox;
	m_pEditBox->CreateSub ( this, "CHAT_EDIT_BOX", UI_FLAG_BOTTOM, CHAT_EDITBOX );
	m_pEditBox->CreateCarrat ( "CHAT_CARRAT", TRUE, UINT_MAX );
	m_pEditBox->SetLimitInput ( nLIMITCHAR );
	m_pEditBox->SetFont ( pFont );	
	m_pEditBox->SetProtectSizePoint ();
	RegisterControl ( m_pEditBox );

	{	//	한글 버튼
		m_pNativeButton = new CBasicButton;
		m_pNativeButton->CreateSub ( this, "CHAT_NATIVE_BUTTON", UI_FLAG_BOTTOM | UI_FLAG_RIGHT, CHAT_NATIVE_BUTTON );
		m_pNativeButton->CreateFlip ( "CHAT_NATIVE_BUTTON_F", CBasicButton::MOUSEIN_FLIP );
		m_pNativeButton->SetUseGlobalAction ( TRUE );
		RegisterControl ( m_pNativeButton );

		m_pEnglishButton = new CBasicButton;
		m_pEnglishButton->CreateSub ( this, "CHAT_ENGLISH_BUTTON", UI_FLAG_BOTTOM | UI_FLAG_RIGHT, CHAT_ENGLISH_BUTTON );
		m_pEnglishButton->CreateFlip ( "CHAT_ENGLISH_BUTTON_F", CBasicButton::MOUSEIN_FLIP );
		m_pEnglishButton->SetUseGlobalAction ( TRUE );
		RegisterControl ( m_pEnglishButton );
	}

#if defined( TH_PARAM )
	{
		m_hCheckStrDLL = LoadLibrary("ThaiCheck.dll");

		if ( m_hCheckStrDLL )
		{
			m_pCheckString = ( BOOL (_stdcall*)(CString)) GetProcAddress(m_hCheckStrDLL, "IsCompleteThaiChar");
		}
	}
#endif


}

CBasicTextBoxEx*	CBasicChatRightBody::CreateTextBoxEx( CD3DFontPar* pFont, UIGUID uID )
{
	CBasicTextBoxEx* pTextBox = new CBasicTextBoxEx;
	pTextBox->CreateSub ( this, "CHAT_BOX_TEXT_BOX", UI_FLAG_YSIZE, uID );
	pTextBox->SetFont ( pFont );
	pTextBox->SetTextAlign ( TEXT_ALIGN_BOTH_X | TEXT_ALIGN_BOTTOM );
	pTextBox->SetLimitLine ( nLIMITLINE );
	pTextBox->SetLineInterval ( 2.0f );
	pTextBox->SetPartInterval ( 2.0f );
	pTextBox->SetProtectSizePoint ();
	RegisterControl ( pTextBox );

	return pTextBox;
}

CBasicScrollBarEx*	CBasicChatRightBody::CreateScrollBarEx ( const int& nTotalLine, const int& nVisibleLine )
{
	CBasicScrollBarEx* pScrollBar = new CBasicScrollBarEx;
	pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE );
	pScrollBar->CreateBaseScrollBar ( "CHAT_SCROLLBAR" );
	pScrollBar->SetVisibleSingle( FALSE );
	pScrollBar->GetThumbFrame()->SetState ( nTotalLine, nVisibleLine );	
	pScrollBar->SetProtectSizePoint ();
	RegisterControl ( pScrollBar );

	return pScrollBar;
}

void CBasicChatRightBody::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{	
	const int nCHATTYPE_BACK = m_nCHATTYPE;

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	DecreaseTime( fElapsedTime );

	UPDATE_CHAT_PAGE ( bFirstControl );


// Focus 및 Update문제로 주석처리
//	if ( bFirstControl ) 
	{
		if ( !IsEDIT_BEGIN() )
		{
			if ( UIKeyCheck::GetInstance()->Check(DIK_RETURN,DXKEY_DOWN) ||
				UIKeyCheck::GetInstance()->Check(DIK_NUMPADENTER,DXKEY_DOWN) )
			{
				CUIControl::AddMessageEx ( UIMSG_KEY_FOCUSED );

				EDIT_BEGIN ();
			}
		}
		else
		{
			if ( UIKeyCheck::GetInstance()->Check(DIK_RETURN,DXKEY_DOWN) ||
				UIKeyCheck::GetInstance()->Check(DIK_NUMPADENTER,DXKEY_DOWN) )
			{
				if ( DXInputString::GetInstance().CheckEnterKeyDown() )
				{
					CUIControl::AddMessageEx ( UIMSG_KEY_FOCUSED );

					//	친구 리스트에 등록된 목록을
					//	귓속말 목록에 추가하는 부분
					ADD_FRIEND_LIST ();			

					SEND_CHAT_MESSAGE ();
				}
			}
		}

		if ( m_pEditBox->IsBegin () )
		{
			static bool bVALID_INPUT_BACK = true;

			//if ( bVALID_INPUT_BACK && !m_pEditBox->IsVALID_INPUT () )
			//{
			//	AddStringToSystemMessage ( ID2GAMEINTEXT("INVALID_KEYBOARD_INPUT"), NS_UITEXTCOLOR::RED );				
			//}
			bVALID_INPUT_BACK = m_pEditBox->IsVALID_INPUT ();

            CLASSIFY_CHATTYPE ();

			UPDATE_CHATTYPE_STATE ( nCHATTYPE_BACK );
			UPDATE_SHORTKEY ();
		}
	}

	//	한/영 모드 보여주기
	BOOL bNative = (m_pEditBox->IsMODE_NATIVE ())?TRUE:FALSE;
	m_pNativeButton->SetVisibleSingle ( bNative );
	m_pEnglishButton->SetVisibleSingle ( !bNative );

	if ( IsEDIT_BEGIN () )
	{
		m_pEditBoxBack->SetVisibleSingle ( TRUE );
	}
	else
	{
		m_pEditBoxBack->SetVisibleSingle ( FALSE );
	}
}

void	CBasicChatRightBody::UPDATE_CHAT_PAGE ( const BOOL& bFirstControl )
{
	m_bMINIMUM_SIZE_TEXT = false;
	m_bMINIMUM_SIZE_SCROLL = false;

	if ( m_pTextBox_ALL && m_pScrollBar_ALL )
	{
		//	텍스트박스 보이지 않게 조절
		{			
			const UIRECT& rcGlobalPos = m_pTextBox_ALL->GetGlobalPos ();
			const float fOneLine = m_pTextBox_ALL->CalcMaxHEIGHT(0,0);
			if ( rcGlobalPos.sizeY <= ((fOneLine*2)+3) )
			{
				m_bMINIMUM_SIZE_TEXT = true;
			}
		}

		//	스크롤바 보이지 않게 조절
		{
			const UIRECT& rcGlobalPos = m_pScrollBar_ALL->GetGlobalPos ();
			const float fProtectY = m_pScrollBarDummy->GetGlobalPos ().sizeY;
			if ( rcGlobalPos.sizeY <= fProtectY )
			{
				m_bMINIMUM_SIZE_SCROLL = true;
			}
		}
	}

//	if ( bFirstControl )
	{
		if ( !m_bMINIMUM_SIZE_TEXT )
		{	
			if ( m_pTextBox_ALL->IsVisible () )				UPDATE_CHAT_PAGE_TEXTBOX ( m_pTextBox_ALL, m_pScrollBar_ALL );
			else if ( m_pTextBox_PARTY->IsVisible () )		UPDATE_CHAT_PAGE_TEXTBOX ( m_pTextBox_PARTY, m_pScrollBar_PARTY );
			else if ( m_pTextBox_CLUB->IsVisible () )		UPDATE_CHAT_PAGE_TEXTBOX ( m_pTextBox_CLUB, m_pScrollBar_CLUB );
			else if ( m_pTextBox_PRIVATE->IsVisible () )	UPDATE_CHAT_PAGE_TEXTBOX ( m_pTextBox_PRIVATE, m_pScrollBar_PRIVATE );
			else if ( m_pTextBox_SYSTEM->IsVisible () )		UPDATE_CHAT_PAGE_TEXTBOX ( m_pTextBox_SYSTEM, m_pScrollBar_SYSTEM );
			else if ( m_pTextBox_ALLIANCE->IsVisible () )	UPDATE_CHAT_PAGE_TEXTBOX ( m_pTextBox_ALLIANCE, m_pScrollBar_ALLIANCE );
		}
	}

	SELECT_CHAT_PAGE ();
}

void	CBasicChatRightBody::UPDATE_CHAT_PAGE_TEXTBOX ( CBasicTextBoxEx* pTextBox, CBasicScrollBarEx* pScrollBar )
{
	//	스크롤바 조절
	CBasicScrollThumbFrame* const pThumbFrame = pScrollBar->GetThumbFrame ();	

	const int nTotalLine = pTextBox->GetTotalLine ();
	const int nLinePerOneView = pTextBox->GetVisibleLine ();

	pThumbFrame->SetState ( nTotalLine, nLinePerOneView );	
	if ( nLinePerOneView < nTotalLine )
	{
		const int nMovableLine = nTotalLine - nLinePerOneView;
		float fPercent = pThumbFrame->GetPercent ();
		int nPos = (int)(fPercent * nMovableLine + 0.5f);
		pTextBox->SetCurLine ( nPos );
	}
}

void	CBasicChatRightBody::ADD_CHAT_PAGE_TEXTBOX ( const CString& strTemp, const D3DCOLOR& dwColor, CBasicTextBoxEx* pTextBox, CBasicScrollBarEx* pScrollBar )
{
	GASSERT( pTextBox && pScrollBar );

	pTextBox->AddText ( strTemp, dwColor );

	CBasicScrollThumbFrame* const pThumbFrame = pScrollBar->GetThumbFrame ();
	if ( !pThumbFrame )
	{
		GASSERT ( 0 && "썸이 만들어지지 않았습니다.");
		return ;
	}

	int nTotal, nView;
	pThumbFrame->GetState ( nTotal, nView );	

	const int nTotalLine = pTextBox->GetTotalLine ();
	pThumbFrame->SetState ( nTotalLine, nView );
	pThumbFrame->SetPercent ( CBasicScrollThumbFrame::fHUNDRED_PERCENT );
}

void	CBasicChatRightBody::ChangePrivateName ()
{
	if ( !IsEDIT_BEGIN () ) return ;

	const CString& strOrigin = m_pEditBox->GetEditString();
	const int nEditLength = m_pEditBox->GetEditLength ();
	const int nSize = (int)m_NameList.size ();

	if ( nSize != m_NameCountBack )
	{
		m_NamePos = 0;
	}

	m_NameCountBack = nSize;

	if ( nSize )
	{											
		if ( m_pEditBox->GetEditLength() == 1 && !m_strChatBack.GetLength() ) // MEMO
		{
			WORD wKEYSTATE = (WORD)DxInputDevice::GetInstance().GetKeyState ( DIK_BACK );
			if ( !(wKEYSTATE & DXKEY_DOWN) && !(wKEYSTATE & DXKEY_PRESSED) )
			{
				NAMELIST_ITER iter = m_NameList.begin ();
				CString strName = (*iter);
				//if ( IS_FRIEND ( strName ) ) strName = GET_ONLINE_FRIEND ( strName );

				m_pEditBox->SetEditString ( PRIVATE_SYMBOL + strName + BLANK_SYMBOL );
			}
		}
		//	문자 조합중이 아닐 경우에만 귓속말 상대 선택이 가능.
		else if ( !DXInputString::GetInstance().CheckComposition() )
		{
			BOOL bKEYDOWN = FALSE;
			if ( UIKeyCheck::GetInstance()->Check ( DIK_UP, DXKEY_DOWN) )
			{							
				m_NamePos++;
				if ( nSize <= m_NamePos ) m_NamePos = nSize - 1;
				bKEYDOWN = TRUE;
			}
			else if ( UIKeyCheck::GetInstance()->Check ( DIK_DOWN, DXKEY_DOWN ) )
			{
				m_NamePos--;
				if ( m_NamePos < 0 ) m_NamePos = 0;
				bKEYDOWN = TRUE;
			}

			if ( bKEYDOWN )
			{
				NAMELIST_ITER iter = m_NameList.begin ();
				NAMELIST_ITER iter_end = m_NameList.end ();
				for ( int i = 0; i < m_NamePos && iter != iter_end; ++i, ++iter ) ;
		        
				CString strName = (*iter);
				//if ( IS_FRIEND ( strName ) ) strName = GET_ONLINE_FRIEND ( strName );

				m_pEditBox->SetEditString ( PRIVATE_SYMBOL + strName + BLANK_SYMBOL );
			}
		}
	}
	m_strChatBack = strOrigin;
}

bool CBasicChatRightBody::IS_FRIEND ( CString strName )
{
	SFRIEND* psFriend = GLFriendClient::GetInstance().GetFriend ( strName.GetString () );
	if ( psFriend )	return true;
	else			return false;
}


CString CBasicChatRightBody::GET_ONLINE_FRIEND ( CString strName )
{
	GLFriendClient::FRIENDMAP& mapFriend = GLFriendClient::GetInstance().GetFriend ();
	GLFriendClient::FRIENDMAP_ITER iter = mapFriend.find ( strName.GetString () );	
	if ( iter==mapFriend.end() ) return strName;

	SFRIEND& sFriend = (*iter).second;	
	if ( sFriend.bONLINE ) return strName;

	const int nSize = (int)m_NameList.size ();

	++iter;
	++m_NamePos;
	if ( nSize <= m_NamePos ) m_NamePos = nSize - 1;
	while ( iter!=mapFriend.end() )
	{		
		SFRIEND& sFriend = (*iter).second;
		if ( sFriend.bONLINE ) return sFriend.szCharName;
		++iter;
		++m_NamePos;
		if ( nSize <= m_NamePos ) m_NamePos = nSize - 1;
	}

	return strName;
}

void CBasicChatRightBody::ChangeLastChat ()
{
	if ( !IsEDIT_BEGIN () )									return;

	BOOL bKEYDOWN = FALSE;
	//	채팅이 갱신되었으면, 채팅 포지션을 -1로 맞춘다.
	int nSize = (int)m_ChatLog.size ();
	if ( nSize != m_ChatCountBack )
	{
		m_ChatPos = OUT_OF_RANGE;
	}
	m_ChatCountBack = nSize;

	if ( !nSize )											return;

	//	문자 조합중이 아닐 경우에만 귓속말 상대 선택이 가능.
	if ( DXInputString::GetInstance().CheckComposition() )	return;

	if ( UIKeyCheck::GetInstance()->Check ( DIK_UP, DXKEY_DOWN) )
	{							
		if ( m_ChatPos < (nSize-1) )
		{
			m_ChatPos++;
			bKEYDOWN = TRUE;
		}
	}
	else if ( UIKeyCheck::GetInstance()->Check ( DIK_DOWN, DXKEY_DOWN ) )
	{
		if ( OUT_OF_RANGE < m_ChatPos )
		{
			m_ChatPos--;			
			bKEYDOWN = TRUE;
		}
	}

	if ( bKEYDOWN )
	{
		if ( m_ChatPos == OUT_OF_RANGE )
		{
			m_pEditBox->ClearEdit ();
			return ;
		}
		else
		{
			NORMAL_CHAT_LOG_ITER iter = m_ChatLog.begin ();
			NORMAL_CHAT_LOG_ITER iter_end = m_ChatLog.end ();
			for ( int i = 0; i < m_ChatPos && iter != iter_end; ++i, ++iter ) ;
			
			CString strChat = (*iter);
			m_pEditBox->SetEditString ( strChat );
		}
	}
}

void	CBasicChatRightBody::UPDATE_CHATTYPE_STATE ( const int nCHATTYPE_BACK )
{
	//	채팅 모드 전환 표시
	//	앞 조건 : 채팅꺼진 상태에서 켜려고 하는 경우는 전환이 일어나는 것이
	//			아니므로 무시한다.
	//	뒤 조건 : 채팅 타입이 바뀐것이다.
	if ( nCHATTYPE_BACK != CHAT_NO && nCHATTYPE_BACK != m_nCHATTYPE )
	{
		switch ( m_nCHATTYPE )
		{
		case CHAT_NORMAL:
			{					
				m_pEditBox->SetTextColor ( NS_UITEXTCOLOR::DEFAULT );
				AddStringToNORMAL ( ID2GAMEINTEXT("CHAT_NORMAL_MODE"), NS_UITEXTCOLOR::DEFAULT );
			}
			break;

		case CHAT_PRIVATE:
			{
				const int nEditLength = m_pEditBox->GetEditLength();
				if( nEditLength == 1 ) // 두번 출력되는 것을 막기 위해 임시로 넣은 코드이다. ㅜㅜ
				{
					m_pEditBox->SetTextColor ( NS_UITEXTCOLOR::PRIVATE );
					AddStringToNORMAL ( ID2GAMEINTEXT("CHAT_PRIVATE_MODE"), NS_UITEXTCOLOR::PRIVATE );
				}
			}
			break;

		case CHAT_PARTY:
			{
				m_pEditBox->SetTextColor ( NS_UITEXTCOLOR::AQUAMARINE );
				AddStringToNORMAL ( ID2GAMEINTEXT("CHAT_PARTY_MODE"), NS_UITEXTCOLOR::AQUAMARINE );
			}
			break;

		case CHAT_TOALL:
			{
				m_pEditBox->SetTextColor ( NS_UITEXTCOLOR::ORANGE );
				AddStringToNORMAL ( ID2GAMEINTEXT("CHAT_TOALL_MODE"), NS_UITEXTCOLOR::ORANGE );
			}
			break;

		case CHAT_GUILD:
			{
				m_pEditBox->SetTextColor ( NS_UITEXTCOLOR::VIOLET );
				AddStringToNORMAL ( ID2GAMEINTEXT("CHAT_GUILD_MODE"), NS_UITEXTCOLOR::VIOLET );
			}
			break;

		case CHAT_ALLIANCE:
			{
				m_pEditBox->SetTextColor ( NS_UITEXTCOLOR::HOTPINK );
				AddStringToNORMAL ( ID2GAMEINTEXT("CHAT_ALLIANCE_MODE"), NS_UITEXTCOLOR::HOTPINK );
			}
			break;

		default:
			GASSERT(FALSE);
			break;
		}
	}
}

void	CBasicChatRightBody::ChangeChatType ()
{
	if ( !IsEDIT_BEGIN () ) return ;
	if ( UIKeyCheck::GetInstance()->Check ( DIK_ESCAPE, DXKEY_DOWN ) )
	{
		EDIT_END ();
	}

	if ( UIKeyCheck::GetInstance()->Check ( DIK_TAB, DXKEY_DOWN ) )
	{
		const int nCHATTYPE_BACK = GetCHATTYPE();
		EDIT_END ();

		switch ( nCHATTYPE_BACK )
		{
		case CHAT_NORMAL:	BEGIN_PRIVATE_CHAT();	break;
		case CHAT_PRIVATE:	BEGIN_PARTY_CHAT();		break;
		case CHAT_PARTY:	BEGIN_TOALL_CHAT();		break;
		case CHAT_TOALL:	BEGIN_GUILD_CHAT();		break;
		case CHAT_GUILD:	BEGIN_ALLIANCE_CHAT();	break;
		case CHAT_ALLIANCE:	BEGIN_NORMAL_CHAT();	break;
		}			
	}	
}

void	CBasicChatRightBody::CLASSIFY_CHATTYPE ()
{	
	m_nCHATTYPE = CHAT_NORMAL;

	const int nEditLength = m_pEditBox->GetEditLength ();
	if( nEditLength )
	{
		CString strOrigin = m_pEditBox->GetEditString();
		const CString FIRST_CHAR( strOrigin[0] );

		if( FIRST_CHAR == PRIVATE_SYMBOL )			m_nCHATTYPE = CHAT_PRIVATE;
		else if( FIRST_CHAR == PARTY_SYMBOL )		m_nCHATTYPE = CHAT_PARTY;					
		else if( FIRST_CHAR == TOALL_SYMBOL )		m_nCHATTYPE = CHAT_TOALL;
		else if( FIRST_CHAR == GUILD_SYMBOL )		m_nCHATTYPE = CHAT_GUILD;
		else if( FIRST_CHAR == ALLIANCE_SYMBOL )	m_nCHATTYPE = CHAT_ALLIANCE;
	}
}

void	CBasicChatRightBody::UPDATE_SHORTKEY ()
{
	ChangeChatType ();

	switch ( m_nCHATTYPE )
	{
	case CHAT_NORMAL:
		{
			ChangeLastChat ();
		}
		break;

	case CHAT_PRIVATE:
		{
			ChangePrivateName ();
		}
		break;

	case CHAT_PARTY:
		{
		}
		break;

	case CHAT_GUILD:
		{
		}
		break;

	case CHAT_ALLIANCE:
		{
		}
		break;

	default:
		{
		}
		break;
	}
}

void CBasicChatRightBody::SELECT_CHAT_PAGE ()
{
//	m_pSystemMessageDisplay->SetVisibleSingle ( TRUE );

	m_pTextBox_ALL->SetVisibleSingle ( FALSE );
	m_pScrollBar_ALL->SetVisibleSingle ( FALSE );

	m_pTextBox_PARTY->SetVisibleSingle ( FALSE );
	m_pScrollBar_PARTY->SetVisibleSingle ( FALSE );

	m_pTextBox_CLUB->SetVisibleSingle ( FALSE );
	m_pScrollBar_CLUB->SetVisibleSingle ( FALSE );

	m_pTextBox_ALLIANCE->SetVisibleSingle ( FALSE );
	m_pScrollBar_ALLIANCE->SetVisibleSingle ( FALSE );

	m_pTextBox_PRIVATE->SetVisibleSingle ( FALSE );
	m_pScrollBar_PRIVATE->SetVisibleSingle ( FALSE );

	m_pTextBox_SYSTEM->SetVisibleSingle ( FALSE );
	m_pScrollBar_SYSTEM->SetVisibleSingle ( FALSE );

	m_pChatTotalButton->SetFlip ( FALSE );
	m_pChatPrivateButton->SetFlip ( FALSE );
	m_pChatPartyButton->SetFlip ( FALSE );
	m_pChatClubButton->SetFlip ( FALSE );
	m_pChatAllianceButton->SetFlip ( FALSE );
	m_pChatSystemButton->SetFlip ( FALSE );

	switch ( m_wDISPLAYTYPE )
	{
	case CHAT_CHANNEL_TOTAL:
		{
			m_pChatTotalButton->SetFlip ( TRUE );			

			if ( !m_bMINIMUM_SIZE_TEXT ) m_pTextBox_ALL->SetVisibleSingle ( TRUE );
			if ( !m_bMINIMUM_SIZE_SCROLL )
			{
				int nTotal = m_pTextBox_ALL->GetTotalLine();
				int nVisible = m_pTextBox_ALL->GetVisibleLine();

				if( nTotal > nVisible ) m_pScrollBar_ALL->SetVisibleSingle ( TRUE );
			}
		}
		break;

	case CHAT_CHANNEL_PRIVATE:
		{
			m_pChatPrivateButton->SetFlip ( TRUE );

			if ( !m_bMINIMUM_SIZE_TEXT ) m_pTextBox_PRIVATE->SetVisibleSingle ( TRUE );
			if ( !m_bMINIMUM_SIZE_SCROLL )
			{
				int nTotal = m_pTextBox_PRIVATE->GetTotalLine();
				int nVisible = m_pTextBox_PRIVATE->GetVisibleLine();

				if( nTotal > nVisible ) m_pScrollBar_PRIVATE->SetVisibleSingle ( TRUE );
			}
		}
		break;

	case CHAT_CHANNEL_PARTY:
		{
			m_pChatPartyButton->SetFlip ( TRUE );

			if ( !m_bMINIMUM_SIZE_TEXT ) m_pTextBox_PARTY->SetVisibleSingle ( TRUE );
			if ( !m_bMINIMUM_SIZE_SCROLL )
			{
				int nTotal = m_pTextBox_PARTY->GetTotalLine();
				int nVisible = m_pTextBox_PARTY->GetVisibleLine();

				if( nTotal > nVisible ) m_pScrollBar_PARTY->SetVisibleSingle ( TRUE );
			}
		}
		break;

	case CHAT_CHANNEL_CLUB:
		{
			m_pChatClubButton->SetFlip ( TRUE );

			if ( !m_bMINIMUM_SIZE_TEXT ) m_pTextBox_CLUB->SetVisibleSingle ( TRUE );
			if ( !m_bMINIMUM_SIZE_SCROLL )
			{
				int nTotal = m_pTextBox_CLUB->GetTotalLine();
				int nVisible = m_pTextBox_CLUB->GetVisibleLine();

				if( nTotal > nVisible ) m_pScrollBar_CLUB->SetVisibleSingle ( TRUE );
			}
		}
		break;

	case CHAT_CHANNEL_ALLIANCE:
		{
			m_pChatAllianceButton->SetFlip ( TRUE );

			if ( !m_bMINIMUM_SIZE_TEXT ) m_pTextBox_ALLIANCE->SetVisibleSingle ( TRUE );
			if ( !m_bMINIMUM_SIZE_SCROLL )
			{
				int nTotal = m_pTextBox_ALLIANCE->GetTotalLine();
				int nVisible = m_pTextBox_ALLIANCE->GetVisibleLine();

				if( nTotal > nVisible ) m_pScrollBar_ALLIANCE->SetVisibleSingle ( TRUE );
			}
		}
		break;

	case CHAT_CHANNEL_SYSTEM:
		{
			m_pChatSystemButton->SetFlip ( TRUE );

			if ( !m_bMINIMUM_SIZE_TEXT ) m_pTextBox_SYSTEM->SetVisibleSingle ( TRUE );
			if ( !m_bMINIMUM_SIZE_SCROLL )
			{
				int nTotal = m_pTextBox_SYSTEM->GetTotalLine();
				int nVisible = m_pTextBox_SYSTEM->GetVisibleLine();

				if( nTotal > nVisible ) m_pScrollBar_SYSTEM->SetVisibleSingle ( TRUE );
			}

//			m_pSystemMessageDisplay->SetVisibleSingle ( FALSE );
		}
		break;
	}
}

CBasicTextButton*	 CBasicChatRightBody::CreateTextButton ( char* szButton, UIGUID ControlID, char* szText )
{
	CBasicTextButton* pTextButton = new CBasicTextButton;
	pTextButton->CreateSub ( this, "BASIC_TEXT_BUTTON191", UI_FLAG_XSIZE, ControlID );
	pTextButton->CreateBaseButton ( szButton, 
									CBasicTextButton::SIZE19_RECT, 
									CBasicButton::RADIO_FLIP, 
									szText, 
									_DEFAULT_FONT_SHADOW_EX_FLAG );
	pTextButton->SetAlignFlag ( UI_FLAG_BOTTOM );
	pTextButton->SetFlip ( TRUE );
	RegisterControl ( pTextButton );

	return pTextButton;
}

void CBasicChatRightBody::AddChat ( const CString& strName, const CString& strMsg, const WORD& wType )
{
	CString strCombine;
	strCombine.Format ( "[%s]:%s", strName, strMsg );

	AddStringToChatEx ( strCombine,wType );

	switch ( wType )
	{
	case CHAT_PRIVATE:
		{
			ADD_FRIEND ( strName );
			break;
		}
		break;
	}
}

void CBasicChatRightBody::AddChatMacro(CString strMsg)
{
	strMsg.TrimLeft ( BLANK_SYMBOL );	//	우측 공백 무시
	if( !strMsg.GetLength() )	return;

	DWORD wType;
	CString ChatType( strMsg[0] );

	if( ChatType == PRIVATE_SYMBOL )		wType = CHAT_PRIVATE;
	else if( ChatType == PARTY_SYMBOL )		wType = CHAT_PARTY;					
	else if( ChatType == TOALL_SYMBOL )		wType = CHAT_TOALL;
	else if( ChatType == GUILD_SYMBOL )		wType = CHAT_GUILD;
	else if( ChatType == ALLIANCE_SYMBOL )	wType = CHAT_ALLIANCE;
	else	wType = CHAT_NORMAL;
	
	m_nCHATTYPE = wType;

	SEND_CHAT_MESSAGE(strMsg);
}

void CBasicChatRightBody::SetVisibleChatButton( BOOL bVisible )
{
	m_pChatTotalButton->SetVisibleSingle( bVisible );
	m_pChatPrivateButton->SetVisibleSingle( bVisible );	
	m_pChatPartyButton->SetVisibleSingle( bVisible );
	m_pChatClubButton->SetVisibleSingle( bVisible );
	m_pChatSystemButton->SetVisibleSingle( bVisible );
	m_pChatAllianceButton->SetVisibleSingle( bVisible );
}
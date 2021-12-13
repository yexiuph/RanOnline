#include "pch.h"
#include "BasicChatRightBody.h"
#include "../[Lib]__Engine/Sources/DxTools/DxFontMan.h"
#include "../[Lib]__EngineUI/Sources/UIEditBox.h"
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
#include "Emoticon.h"
#include "../[Lib]__Engine/Sources/Common/SubPath.h"
#include "shlobj.h"
#include "../[Lib]__Engine/Sources/Common/StringUtils.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void CBasicChatRightBody::ADD_CHATLOG ( const CString& strChat )
{
	if( strChat.IsEmpty() ) return;

	NORMAL_CHAT_LOG_ITER iter = m_ChatLog.begin ();
	NORMAL_CHAT_LOG_ITER iter_end = m_ChatLog.end ();
	for ( ; iter != iter_end; ++iter )	//	혹시 중간에 있다면...
	{
		if( (*iter) == strChat )
		{
			m_ChatLog.erase( iter );
			m_ChatPos = OUT_OF_RANGE;
			break;
		}
	}

	m_ChatLog.push_front ( strChat );

	if ( m_ChatLog.size () > 50 )
	{
		m_ChatLog.pop_back ();
	}
}

VOID CBasicChatRightBody::DecreaseTime( float fElapsedTime )			
{ 
	if( m_bPapering && ( m_fLifeTime > 0.0f ) ) 
	{
		m_fLifeTime -= fElapsedTime;
	}
}

BOOL CBasicChatRightBody::IsPapering( const CString& strOrigin )
{
	if( GetLifeTime() > 0.0f ) 
		return TRUE;
	else 
		m_bPapering = FALSE;

	//	Memo :	같은 메세지 중복 처리
	if( m_strLastMsg == strOrigin )
	{
		++m_nInputCount;

		if( m_nInputCount > nCOUNT_INPUT)
		{
			m_bPapering = TRUE;
		}
	}
	else 
	{
		m_nInputCount = 0;			// 연속되지 않을 경우 초기화
		m_strLastMsg = strOrigin;	// 이전 메세지 저장
	}

	//	Memo :	연속된 메세지 중복 처리
	static FLOAT s_fStartTime = 0.f;
	static FLOAT s_fEndTime = 0.f;
	static BOOL bStart(TRUE);

	if( bStart )
	{
		s_fStartTime = DXUtil_Timer( TIMER_GETAPPTIME );
		bStart = FALSE;
	}
	else
	{
		s_fEndTime = DXUtil_Timer( TIMER_GETAPPTIME );

		if( (s_fEndTime-s_fStartTime) <= fINTERVAL_INPUT)
		{
			// 2초안에 메세지가 들어왔다.
			++m_nTimeCount;

			if( m_nTimeCount >= nCOUNT_INPUT)
			{
				m_bPapering = TRUE;
			}
			else
			{
				s_fStartTime = s_fEndTime;
			}
		}
		else
		{
			m_nTimeCount = 0;	// 연속되지 않을 경우 초기화
			bStart = TRUE;
		}
	}

	if( m_bPapering )
	{
		AddStringToNORMAL( ID2GAMEINTEXT("CHAT_PAPERING_MODE"), NS_UITEXTCOLOR::ORANGE );
		SetLifeTime( fLIMIT_TIME );

		//	Memo :	모든 체크 변수들을 초기화 한다.
		m_nInputCount = 0;
		m_strLastMsg.Empty();
		m_nTimeCount = 0;
		bStart = TRUE;

		return TRUE;
	}

	return FALSE;
}

bool CBasicChatRightBody::SEND_COMMON_MESSAGE ( const CString& strOrigin )
{
	CNetClient*	pNetClient = DxGlobalStage::GetInstance().GetNetClient();
	GLCharacter* pCharacter = GLGaeaClient::GetInstance().GetCharacter();

	//	필터링
	//std::string strTemp = strOrigin.GetString();
	CString strTemp( strOrigin );
	if ( CRanFilter::GetInstance().ChatFilter( strTemp ) )
		strTemp = CRanFilter::GetInstance().GetProverb();

	//	메시지 전송
	if ( pNetClient ) pNetClient->SndChatNormal ( pCharacter->m_szName, strOrigin.GetString() );

	ADD_CHATLOG ( strTemp.GetString() );
	
	CString strCombine;
	strCombine.Format ( "[%s]:%s", pCharacter->m_szName, strTemp.GetString() );
	AddStringToChatEx ( strCombine, CHAT_NORMAL );

	ShowSelfMessageOnHead ( strTemp );

	return true;
}

bool CBasicChatRightBody::SEND_PRIVATE_MESSAGE( const CString& strOrigin )
{
	CNetClient*	pNetClient = DxGlobalStage::GetInstance().GetNetClient();
	GLCharacter* pCharacter = GLGaeaClient::GetInstance().GetCharacter();

	CString strTemp;
	strTemp = strOrigin.Right ( strOrigin.GetLength () - 1 );

	int nFirstBlankPos = strTemp.Find ( BLANK_SYMBOL );

	CString strName;
	CString strMsg;

	//	메시지가 없는 경우
	if ( nFirstBlankPos < 0 )
	{
		strName = strTemp;
	}
	else
	{
		//	@ 캐릭명.. @와 캐릭명 사이의 공백을 무시하게 한다.
		if ( 0 == nFirstBlankPos )
		{
			while ( nFirstBlankPos < strTemp.GetLength () )
			{
				if ( strTemp[nFirstBlankPos] != BLANK_SYMBOL )
					break;
				nFirstBlankPos++;
			}

			strTemp = strTemp.Right ( strTemp.GetLength () - nFirstBlankPos );
			nFirstBlankPos = strTemp.Find ( BLANK_SYMBOL );					
		}

		strName = strTemp.Left ( nFirstBlankPos );			
		strMsg = strTemp.Right ( strTemp.GetLength () - nFirstBlankPos - 1 );
	}

	if ( strName.GetLength () )
	{
		ADD_FRIEND ( strName );
		CInnerInterface::GetInstance().ADD_FRIEND_NAME_TO_EDITBOX ( strName );
	}

	CString strTrim = strMsg;
	strTrim.Trim ( BLANK_SYMBOL );
	if ( !strName.GetLength () || !strTrim.GetLength () ) return false;

	CString srtTemp = strMsg;

    //	필터링
	if ( CRanFilter::GetInstance().ChatFilter ( srtTemp ) )
		srtTemp = CRanFilter::GetInstance().GetProverb();

	if ( pNetClient ) pNetClient->SndChatPrivate ( strName.GetString(), strMsg.GetString () );

	CString strCombine;
	strCombine.Format ( "[%s->%s]:%s", pCharacter->m_szName, strName, srtTemp.GetString () );
	AddStringToChatEx ( strCombine, CHAT_PRIVATE );

	//strCombine.Format ( "[%s]%s:%s", strName, ID2GAMEINTEXT("CHAT_PRIVATE_TO_MSG"), strMsg.GetString () );
	//AddStringToChatEx ( strCombine, CHAT_PRIVATE );

	return true;
}

bool CBasicChatRightBody::SEND_PARTY_MESSAGE ( const CString& strOrigin )
{
	CNetClient*	pNetClient = DxGlobalStage::GetInstance().GetNetClient();
	GLCharacter* pCharacter = GLGaeaClient::GetInstance().GetCharacter();

	CString strMsg;
	strMsg = strOrigin.Right ( strOrigin.GetLength () - 1 );

	CString strTrim = strMsg;
	strTrim.Trim ( BLANK_SYMBOL );
	if ( !strTrim.GetLength () ) return false;

	CString strTemp = strMsg;

	//	필터링
	if ( CRanFilter::GetInstance().ChatFilter ( strTemp ) )
		strTemp = CRanFilter::GetInstance().GetProverb();

	if ( pNetClient ) pNetClient->SndChatParty ( strMsg.GetString () );			

	CString strCombine;
	strCombine.Format ( "[%s]:%s", pCharacter->m_szName, strTemp.GetString () );
	AddStringToChatEx ( strCombine, CHAT_PARTY );

	return true;
}

bool CBasicChatRightBody::SEND_TOALL_MESSAGE ( const CString& strOrigin )
{
	CNetClient*	pNetClient = DxGlobalStage::GetInstance().GetNetClient();
	GLCharacter* pCharacter = GLGaeaClient::GetInstance().GetCharacter();

	CString strMsg;
	strMsg = strOrigin.Right ( strOrigin.GetLength () - 1 );

	CString strTrim = strMsg;
	strTrim.Trim ( BLANK_SYMBOL );
	if ( !strTrim.GetLength () ) return false;

//	받을때 필터링
//	CString strTemp = strMsg;
//	if ( CRanFilter::GetInstance().ChatFilter ( strTemp ) )
//		strTemp = CRanFilter::GetInstance().GetProverb();

	pCharacter->ReqLoudSpeaker ( strMsg.GetString () );

	return true;
}

bool CBasicChatRightBody::SEND_GUILD_MESSAGE ( const CString& strOrigin )
{
	CNetClient*	pNetClient = DxGlobalStage::GetInstance().GetNetClient();
	GLCharacter* pCharacter = GLGaeaClient::GetInstance().GetCharacter();

	CString strMsg;
	strMsg = strOrigin.Right ( strOrigin.GetLength () - 1 );

	CString strTrim = strMsg;
	strTrim.Trim ( BLANK_SYMBOL );
	if ( !strTrim.GetLength () ) return false;

//	받을때 필터링
//	CString strTemp = strMsg;
//	if ( CRanFilter::GetInstance().ChatFilter ( strTemp ) )
//		strTemp = CRanFilter::GetInstance().GetProverb();

	if ( pNetClient ) pNetClient->SndChatGuild ( strMsg.GetString () );			

	return true;
}

bool CBasicChatRightBody::SEND_ALLIANCE_MESSAGE ( const CString& strOrigin )
{
	CNetClient*	pNetClient = DxGlobalStage::GetInstance().GetNetClient();
	GLCharacter* pCharacter = GLGaeaClient::GetInstance().GetCharacter();

	CString strMsg;
	strMsg = strOrigin.Right ( strOrigin.GetLength () - 1 );

	CString strTrim = strMsg;
	strTrim.Trim ( BLANK_SYMBOL );
	if ( !strTrim.GetLength () ) return false;

//	받을때 필터링
//	CString strTemp = strMsg;
//	if ( CRanFilter::GetInstance().ChatFilter ( strTemp ) )
//		strTemp = CRanFilter::GetInstance().GetProverb();

	if ( pNetClient ) pNetClient->SndChatAlliance( strMsg.GetString() );			

	return true;
}

bool CBasicChatRightBody::SEND_CHAT_MESSAGE ( const CString& strOrigin )
{

	if( ( m_nCHATTYPE == CHAT_NORMAL || m_nCHATTYPE == CHAT_TOALL ) && IsPapering( strOrigin ) ) return FALSE;

#if defined( TH_PARAM )
	if ( !IsThaiCheck( strOrigin ) ) return FALSE;
#endif 

#if defined( VN_PARAM )
	if ( !IsVnCheck( strOrigin ) ) return FALSE;
#endif 


	switch ( m_nCHATTYPE )
	{
	case CHAT_PRIVATE:	return SEND_PRIVATE_MESSAGE( strOrigin );
	case CHAT_PARTY:	return SEND_PARTY_MESSAGE( strOrigin );
	case CHAT_TOALL:	return SEND_TOALL_MESSAGE( strOrigin );
	case CHAT_GUILD:	return SEND_GUILD_MESSAGE( strOrigin );
	case CHAT_ALLIANCE:	return SEND_ALLIANCE_MESSAGE( strOrigin );
	default:
		{
			//	Note : 제스쳐.
			int nMOTION = CEmoticon::GetInstance().GetEmoticonOfTerm ( strOrigin.GetString() );
			if ( nMOTION!=UINT_MAX )
			{
				GLGaeaClient::GetInstance().GetCharacter()->ReqGESTURE ( nMOTION );
			}

			return SEND_COMMON_MESSAGE ( strOrigin );
		}
		break;
	}
}

HRESULT CBasicChatRightBody::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	hr = CUIGroup::RestoreDeviceObjects ( pd3dDevice );
	if ( FAILED ( hr ) ) return hr;

	m_pScrollBar_ALL->GetThumbFrame()->SetPercent ( CBasicScrollThumbFrame::fHUNDRED_PERCENT );

	return hr;
}

void CBasicChatRightBody::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	if ( m_bFREEZE ) return ;

	switch ( ControlID )
	{
	case CHAT_EDITBOX:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				//	캐릭터 움직임 막기
				AddMessageEx ( UIMSG_MOUSEIN_RIGHTBODY );

				if ( UIMSG_LB_DOWN & dwMsg )
				{
					ADD_FRIEND_LIST();
					EDIT_BEGIN ();
				}
			}
			else
			{
				if ( m_pEditBox->IsMODE_NATIVE () )
				{
					DWORD dwNativeMsg = m_pNativeButton->GetMessageEx ();					
					if ( CHECK_MOUSE_IN ( dwNativeMsg ) ) return ;
				}
				else
				{
					DWORD dwEnglishMsg = m_pEnglishButton->GetMessageEx ();
					if ( CHECK_MOUSE_IN ( dwEnglishMsg ) ) return ;
				}

				if ( m_pEditBox->IsBegin () )
				{
					if ( (UIMSG_LB_DOWN & dwMsg) ||
						 (UIMSG_MB_DOWN & dwMsg) ||
						 (UIMSG_RB_DOWN & dwMsg)
						)
					{
						EDIT_END ();
					}
				}
			}
		}
		break;
		
	case CHAT_NATIVE_BUTTON:
	case CHAT_ENGLISH_BUTTON:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				//	캐릭터 움직임 막기
				AddMessageEx ( UIMSG_MOUSEIN_RIGHTBODY );
			}

			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_pEditBox->DoMODE_TOGGLE ();
			}
		}
		break;

	case CHAT_CHANNEL_TOTAL:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				//	캐릭터 움직임 막기
				AddMessageEx ( UIMSG_MOUSEIN_RIGHTBODY );
			}

			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_wDISPLAYTYPE = CHAT_CHANNEL_TOTAL;
			}
		}
		break;

	case CHAT_CHANNEL_PRIVATE:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				//	캐릭터 움직임 막기
				AddMessageEx ( UIMSG_MOUSEIN_RIGHTBODY );
			}

			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_wDISPLAYTYPE = CHAT_CHANNEL_PRIVATE;
			}
		}
		break;

	case CHAT_CHANNEL_PARTY:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				//	캐릭터 움직임 막기
				AddMessageEx ( UIMSG_MOUSEIN_RIGHTBODY );
			}

			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_wDISPLAYTYPE = CHAT_CHANNEL_PARTY;
			}
		}
		break;

	case CHAT_CHANNEL_CLUB:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				//	캐릭터 움직임 막기
				AddMessageEx ( UIMSG_MOUSEIN_RIGHTBODY );
			}

			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_wDISPLAYTYPE = CHAT_CHANNEL_CLUB;
			}
		}
		break;

	case CHAT_CHANNEL_ALLIANCE:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				//	캐릭터 움직임 막기
				AddMessageEx ( UIMSG_MOUSEIN_RIGHTBODY );
			}

			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_wDISPLAYTYPE = CHAT_CHANNEL_ALLIANCE;
			}
		}
		break;

	case CHAT_CHANNEL_SYSTEM:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				//	캐릭터 움직임 막기
				AddMessageEx ( UIMSG_MOUSEIN_RIGHTBODY );
			}

			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_wDISPLAYTYPE = CHAT_CHANNEL_SYSTEM;
			}
		}
		break;

	case CHAT_TEXTBOX_ALL:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( UIMSG_LB_DUP & dwMsg )
				{
					const int nIndex = m_pTextBox_ALL->GetSelectPos ();
					if ( nIndex < 0 || m_pTextBox_ALL->GetCount () <= nIndex ) return ;

					//m_pTextBox_ALL->SetUseOverColor ( TRUE );
					//m_pTextBox_ALL->SetOverColor ( nIndex, NS_UITEXTCOLOR::DARKORANGE );

					CString strCharName = m_pTextBox_ALL->GetText( nIndex );
					if( strCharName[0] == _T('[') )
					{
						INT nCnt = strCharName.Find( _T(']') );

						BEGIN_PRIVATE_CHAT( strCharName.Mid( 1, nCnt - 1 ) );
					}
				}
			}
		}
		break;
	}
}

void CBasicChatRightBody::AddStringToNORMAL ( CString strTemp, D3DCOLOR dwColor )
{
	if ( m_pTextBox_ALL && m_pScrollBar_ALL )
	{
		m_pTextBox_ALL->AddText ( strTemp, dwColor );

		{
			CBasicScrollThumbFrame* const pThumbFrame = m_pScrollBar_ALL->GetThumbFrame ();
			if ( !pThumbFrame )
			{
				GASSERT ( 0 && "썸이 만들어지지 않았습니다.");
				return ;
			}

			int nTotal, nView;
			pThumbFrame->GetState ( nTotal, nView );	

			const int nTotalLine = m_pTextBox_ALL->GetTotalLine ();
			pThumbFrame->SetState ( nTotalLine, nView );
			pThumbFrame->SetPercent ( CBasicScrollThumbFrame::fHUNDRED_PERCENT );
		}
	}
}

void CBasicChatRightBody::AddStringToChatEx ( CString strTemp, WORD wType )
{
	D3DCOLOR dwTempColor = NS_UITEXTCOLOR::DEFAULT;
	switch ( wType )
	{
	case CHAT_NORMAL:			//일반 채팅 모드
		dwTempColor = NS_UITEXTCOLOR::DEFAULT;
		break;

	case CHAT_PRIVATE:
		{
			dwTempColor = NS_UITEXTCOLOR::PRIVATE;
			ADD_CHAT_PAGE_TEXTBOX ( strTemp, dwTempColor, m_pTextBox_PRIVATE, m_pScrollBar_PRIVATE );
		}
		break;	

	case CHAT_GUILD:
		{
			dwTempColor = NS_UITEXTCOLOR::VIOLET;
			ADD_CHAT_PAGE_TEXTBOX ( strTemp, dwTempColor, m_pTextBox_CLUB, m_pScrollBar_CLUB );
		}
		break;

	case CHAT_TOALL:
		{
			dwTempColor = NS_UITEXTCOLOR::ORANGE;			
		}
		break;

	case CHAT_PARTY:
		{
			dwTempColor = NS_UITEXTCOLOR::AQUAMARINE;
			ADD_CHAT_PAGE_TEXTBOX ( strTemp, dwTempColor, m_pTextBox_PARTY, m_pScrollBar_PARTY );
		}
		break;

	case CHAT_ALLIANCE:
		{
			dwTempColor = NS_UITEXTCOLOR::HOTPINK;
			ADD_CHAT_PAGE_TEXTBOX ( strTemp, dwTempColor, m_pTextBox_ALLIANCE, m_pScrollBar_ALLIANCE );
		}
		break;

	case CHAT_SYSTEM:			//에러 또는 시스템 메시지
		{
			dwTempColor = NS_UITEXTCOLOR::LIGHTSKYBLUE;
			ADD_CHAT_PAGE_TEXTBOX ( strTemp, dwTempColor, m_pTextBox_SYSTEM, m_pScrollBar_SYSTEM );
		}
		break;
	}

	//	강제로 추가하는 경우에는
	//	스테이트를 무시한다.
	if ( !m_bIgnoreState && !(m_wDisplayState & wType) ) return ;

	AddStringToNORMAL ( strTemp, dwTempColor );

	if ( m_bChatLog ) SAVE_CHATLOG( strTemp, wType );
	RECORD_CHAT( strTemp, wType );
}

void CBasicChatRightBody::AddStringToSystemMessage ( CString strTemp, D3DCOLOR dwColor )
{
	ADD_CHAT_PAGE_TEXTBOX ( strTemp, dwColor, m_pTextBox_SYSTEM, m_pScrollBar_SYSTEM );

	if ( m_wDisplayState & CHAT_SYSTEM )
	{
		AddStringToNORMAL ( strTemp, dwColor );
		
		if ( m_bChatLog ) SAVE_CHATLOG( strTemp, CHAT_SYSTEM );
		RECORD_CHAT( strTemp, CHAT_SYSTEM );
	}
}

void	CBasicChatRightBody::BEGIN_NORMAL_CHAT ( const CString& strMessage )
{	
	m_pEditBox->SetTextColor ( NS_UITEXTCOLOR::DEFAULT );
	AddStringToNORMAL ( ID2GAMEINTEXT("CHAT_NORMAL_MODE"), NS_UITEXTCOLOR::DEFAULT );	

	EDIT_CLEAR ();
	EDIT_BEGIN ( strMessage );
}

void	CBasicChatRightBody::BEGIN_PRIVATE_CHAT ( const CString& strName, const CString strMessage )
{	
	m_pEditBox->SetTextColor ( NS_UITEXTCOLOR::PRIVATE );
	AddStringToNORMAL ( ID2GAMEINTEXT("CHAT_PRIVATE_MODE"), NS_UITEXTCOLOR::PRIVATE );

	CString strCombine( PRIVATE_SYMBOL + strName + BLANK_SYMBOL + strMessage );
	if ( !strName.GetLength () ) strCombine = PRIVATE_SYMBOL;
	EDIT_CLEAR ();
	EDIT_BEGIN ( strCombine );
}

void	CBasicChatRightBody::BEGIN_PARTY_CHAT ( const CString& strMessage )
{
	m_pEditBox->SetTextColor ( NS_UITEXTCOLOR::AQUAMARINE );
	AddStringToNORMAL ( ID2GAMEINTEXT("CHAT_PARTY_MODE"), NS_UITEXTCOLOR::AQUAMARINE );

	CString strCombine = PARTY_SYMBOL + strMessage;
	EDIT_CLEAR ();
	EDIT_BEGIN ( strCombine );
}

void	CBasicChatRightBody::BEGIN_TOALL_CHAT ( const CString& strMessage )
{
	m_pEditBox->SetTextColor ( NS_UITEXTCOLOR::ORANGE );
	AddStringToNORMAL ( ID2GAMEINTEXT("CHAT_TOALL_MODE"), NS_UITEXTCOLOR::ORANGE );

	CString strCombine = TOALL_SYMBOL + strMessage;
	EDIT_CLEAR ();
	EDIT_BEGIN ( strCombine );
}

void	CBasicChatRightBody::BEGIN_GUILD_CHAT ( const CString& strMessage )
{
	m_pEditBox->SetTextColor ( NS_UITEXTCOLOR::VIOLET );
	AddStringToNORMAL ( ID2GAMEINTEXT("CHAT_GUILD_MODE"), NS_UITEXTCOLOR::VIOLET );

	CString strCombine = GUILD_SYMBOL + strMessage;
	EDIT_CLEAR ();
	EDIT_BEGIN ( strCombine );
}

void	CBasicChatRightBody::BEGIN_ALLIANCE_CHAT( const CString& strMessage )
{
	m_pEditBox->SetTextColor ( NS_UITEXTCOLOR::HOTPINK );
	AddStringToNORMAL ( ID2GAMEINTEXT("CHAT_ALLIANCE_MODE"), NS_UITEXTCOLOR::HOTPINK );

	CString strCombine = ALLIANCE_SYMBOL + strMessage;
	EDIT_CLEAR ();
	EDIT_BEGIN ( strCombine );
}

void	CBasicChatRightBody::ADD_FRIEND_LIST ()
{
	GLFriendClient::FRIENDMAP& FriendMap = GLFriendClient::GetInstance().GetFriend ();
	if ( FriendMap.size () == m_nFRIENDSIZE_BACK ) return ;

	GLFriendClient::FRIENDMAP_ITER iter = FriendMap.begin ();
	GLFriendClient::FRIENDMAP_ITER iter_end = FriendMap.end ();

	for ( ; iter != iter_end; ++iter )
	{
		SFRIEND& sFriend = (*iter).second;

		if( sFriend.bONLINE ) ADD_FRIEND ( sFriend.szCharName ); // 온라인이면 리스트에 추가한다.
	}

	m_nFRIENDSIZE_BACK = (int)FriendMap.size ();
}

void	CBasicChatRightBody::ADD_FRIEND ( const CString& strName )
{
	if ( !strName.GetLength () ) return ;

	//	삽입
	NAMELIST_ITER iter = m_NameList.begin ();
	NAMELIST_ITER iter_end = m_NameList.end ();
	for ( ; iter != iter_end; ++iter )	//	혹시 중간에 있다면...
	{
		if( (*iter) == strName )
		{
			m_NameList.erase( iter );
			m_NamePos = 0; // 삭제된 이름이 있다면 다시 포지션을 0으로 설정
			break;
		}
	}

	m_NameList.push_front( strName );

	if ( m_NameList.size () > 30 )
	{
		m_NameList.pop_back();
	}
}

void CBasicChatRightBody::DEL_FRIEND( const CString& strName )
{
	if ( !strName.GetLength () ) return ;

	if( !m_NameList.empty() ) m_NameList.remove( strName );
}

void CBasicChatRightBody::EDIT_BEGIN ( const CString& strMessage )
{
	if ( strMessage.GetLength () ) m_pEditBox->SetEditString ( strMessage );

	m_pEditBox->BeginEdit ();
}

bool CBasicChatRightBody::SEND_CHAT_MESSAGE ()
{
	bool no_message = false;   
	
	CString strInput = m_pEditBox->GetEditString ();
	strInput.TrimLeft ( BLANK_SYMBOL );	//	좌측 공백 무시
	if ( strInput.GetLength () )
	{
		bool bincmd(false);
		bincmd = dxincommand::command ( strInput.GetString () );
		if ( !bincmd )
		{						
			if ( !SEND_CHAT_MESSAGE ( strInput ) )
			{
				no_message = true;
			}
		}
	}
	else
	{
		no_message = true;
	}

	EDIT_CLEAR ();
	KEEP_LAST_STATE ();

	if ( no_message ) EDIT_END ();

	return true;
}

void CBasicChatRightBody::KEEP_LAST_STATE ()
{
	switch ( m_nCHATTYPE )
	{
	case CHAT_PRIVATE:
		{
			if ( m_NameList.size () )
			{
				NAMELIST_ITER iter = m_NameList.begin ();
				CString strName = (*iter);
				m_pEditBox->SetEditString ( PRIVATE_SYMBOL + strName + BLANK_SYMBOL );
			}
			else
			{
				m_pEditBox->SetEditString ( CString(PRIVATE_SYMBOL) );
			}	
		}
		break;

	case CHAT_PARTY:
		{
			m_pEditBox->SetEditString ( CString(PARTY_SYMBOL) );
		}
		break;

	case CHAT_TOALL:
		{
			m_pEditBox->SetEditString ( CString(TOALL_SYMBOL) );
		}
		break;

	case CHAT_GUILD:
		{
			m_pEditBox->SetEditString ( CString(GUILD_SYMBOL) );
		}
		break;

	case CHAT_ALLIANCE:
		{
			m_pEditBox->SetEditString ( CString(ALLIANCE_SYMBOL) );
		}
		break;
	}
}

void CBasicChatRightBody::EDIT_END ()
{
	m_pEditBox->EndEdit ();
	m_nCHATTYPE = CHAT_NO;
}

void	CBasicChatRightBody::ShowSelfMessageOnHead ( const CString& strOrigin )
{
	GLCharacter* pCharacter = GLGaeaClient::GetInstance().GetCharacter();
	CHeadChatDisplayMan* pHeadChatDisplayMan = CInnerInterface::GetInstance().GetHeadChatDisplayMan ();

	if( pHeadChatDisplayMan && pCharacter )
	{
		D3DXVECTOR3 *pPos = GLGaeaClient::GetInstance().FindCharHeadPos( pCharacter->m_szName );
		if ( pPos )
		{
			D3DXVECTOR3 vPos = *pPos;
			D3DXVECTOR3 vScreen = DxViewPort::GetInstance().ComputeVec3Project ( &vPos, NULL );

			int nPosX = (int)( vScreen.x);
			int nPosY = (int)( vScreen.y);

			D3DXVECTOR2 vCharPos = D3DXVECTOR2 ( float(nPosX), float(nPosY ) );

			DWORD dwIDColor = NS_UITEXTCOLOR::IDCOLOR;
			GLPARTY_CLIENT* pMaster = GLPartyClient::GetInstance().GetMaster();
			if ( pMaster ) dwIDColor = NS_UITEXTCOLOR::GREENYELLOW;
			pHeadChatDisplayMan->AddChat ( pCharacter->m_szName, dwIDColor, strOrigin, NS_UITEXTCOLOR::DEFAULT, vCharPos );
		}
	}
}

void CBasicChatRightBody::ChatLog( bool bChatLog, int nChatLogType )
{
	if ( bChatLog ) 
	{
		m_bChatLog = true;
		m_nChatLogType |= nChatLogType;
		
		// CreteDirectory...
		TCHAR szChatLogDir[MAX_PATH]={0};
		SHGetSpecialFolderPath( NULL, szChatLogDir, CSIDL_PERSONAL, FALSE );

		std::string strChatLogDir(szChatLogDir);
		strChatLogDir += SUBPATH::CHATLOG;

		CreateDirectory( strChatLogDir.c_str() , NULL );

	}
	else 
	{
		m_nChatLogType &= ~(nChatLogType);
		if ( !m_nChatLogType ) m_bChatLog = false;
	}
}

void CBasicChatRightBody::SAVE_CHATLOG( CString strTemp, WORD wType )
{

	int nType = wType;
	
	if ( !(m_nChatLogType & nType) ) return;

	// 현재시각

	const int nServerYear = (int) GLGaeaClient::GetInstance().GetCurrentTime().GetYear ();
	const int nServerMonth = (int) GLGaeaClient::GetInstance().GetCurrentTime().GetMonth ();
	const int nServerDay = (int) GLGaeaClient::GetInstance().GetCurrentTime().GetDay ();
	const int nServerHour = (int) GLGaeaClient::GetInstance().GetCurrentTime().GetHour ();
	const int nServerMinute = (int) GLGaeaClient::GetInstance().GetCurrentTime().GetMinute();
//	const int nServerYear = (int) CInnerInterface::GetInstance().GetCurrentTime().GetYear ();
//	const int nServerMonth = (int) CInnerInterface::GetInstance().GetCurrentTime().GetMonth ();
//	const int nServerDay = (int) CInnerInterface::GetInstance().GetCurrentTime().GetDay ();
//	const int nServerHour = (int) CInnerInterface::GetInstance().GetCurrentTime().GetHour ();
//	const int nServerMinute = (int) CInnerInterface::GetInstance().GetCurrentTime().GetMinute();

	CString strChatLog;
	strChatLog.Format("%02d::%02d\t ", nServerHour, nServerMinute );

	switch ( wType )
	{
	case CHAT_NORMAL:
		break;
	case CHAT_PRIVATE:
		strChatLog += PRIVATE_SYMBOL;
		break;
	case CHAT_GUILD:
		strChatLog += GUILD_SYMBOL + GUILD_SYMBOL;
		break;
	case CHAT_TOALL:
		strChatLog += TOALL_SYMBOL;
		break;
	case CHAT_PARTY:
		strChatLog += PARTY_SYMBOL;
		break;
	case CHAT_ALLIANCE:
		strChatLog += ALLIANCE_SYMBOL;
		break;
	case CHAT_SYSTEM:
		strChatLog += SYSTEM_SYMBOL;
		break;
	}

	strChatLog += "\t";

	strChatLog += strTemp;
	strChatLog += '\n';


	// 로그파일 로드 및 저장

	TCHAR szChatLogDir[MAX_PATH]={0};
	SHGetSpecialFolderPath( NULL, szChatLogDir, CSIDL_PERSONAL, FALSE );

	CString strChatLogDir(szChatLogDir);
	strChatLogDir += SUBPATH::CHATLOG;
	
	CString strChatLogFile;
	
	strChatLogFile.Format( "%s%d%02d%02d.txt", strChatLogDir, nServerYear, nServerMonth, nServerDay );

	FILE* file=NULL;
	fopen_s(&file, strChatLogFile, "a");
	if ( !file )	return;

	fprintf( file, strChatLog );

	fclose( file );

}

void CBasicChatRightBody::RECORD_CHAT( const CString strChatMsg, WORD wType  )
{
	SRecordChatMsg recordChatMsg;
	CTime curTime = GLGaeaClient::GetInstance().GetCurrentTime();

	// 2분의 시간이 지난 메시지 삭제를 위해 지난 메시지를 체크한다.
	{		
		RECORD_CHAT_LOG_ITER iter = m_vecRecordChatMsg.begin();
		int i = 0;
		// 최대 10개까지만 검사
		for( ; i < 10 &&  iter != m_vecRecordChatMsg.end(); ++iter, i++ )
		{
			recordChatMsg = *iter;
			CTimeSpan timeSpan = curTime - recordChatMsg.recordTime;
			if( timeSpan.GetMinutes() >= 2 ) m_vecRecordChatMsg.erase(iter--);
		}
	}

	// 메시지 추가
	{
		recordChatMsg.recordTime = curTime;
		recordChatMsg.strChatMsg = "";
		switch ( wType )
		{
		case CHAT_PRIVATE:
			recordChatMsg.strChatMsg = "@";
			break;
		case CHAT_GUILD:
			recordChatMsg.strChatMsg = GUILD_SYMBOL + GUILD_SYMBOL;
			break;
		case CHAT_TOALL:
			recordChatMsg.strChatMsg = TOALL_SYMBOL;
			break;
		case CHAT_PARTY:
			recordChatMsg.strChatMsg = PARTY_SYMBOL;
			break;
		case CHAT_ALLIANCE:
			recordChatMsg.strChatMsg =ALLIANCE_SYMBOL;
			break;
		case CHAT_SYSTEM:
			recordChatMsg.strChatMsg =SYSTEM_SYMBOL;
			break;
		}

		recordChatMsg.strChatMsg += strChatMsg;
		m_vecRecordChatMsg.push_back( recordChatMsg );
	}
}

CString CBasicChatRightBody::GET_RECORD_CHAT()
{
	SRecordChatMsg recordChatMsg;
	RECORD_CHAT_LOG_ITER iter = m_vecRecordChatMsg.begin();
	CString strLine, strReturn;
	for( ; iter != m_vecRecordChatMsg.end(); ++iter )
	{
		recordChatMsg = *iter;
		strLine = "";
		strLine.Format( "[%d:%d:%d:%d:%d] %s \n", recordChatMsg.recordTime.GetYear(), 
												  recordChatMsg.recordTime.GetMonth(),
												  recordChatMsg.recordTime.GetHour(),
												  recordChatMsg.recordTime.GetMinute(),
												  recordChatMsg.recordTime.GetSecond(),
												  recordChatMsg.strChatMsg.GetString() );

		strReturn += strLine;
											   
	}

	return strReturn;
}

BOOL CBasicChatRightBody::IsThaiCheck( const CString& strOrigin )
{
#if defined( TH_PARAM )
	// 태국 문자 조합 체크 
	if ( !m_pCheckString ) return FALSE;

	if ( !m_pCheckString( strOrigin ) )
	{
		AddStringToSystemMessage( ID2GAMEINTEXT("CHAT_THAI_CHECK_ERROR") , NS_UITEXTCOLOR::NEGATIVE );
		return FALSE;
	}
#endif 
	return TRUE;
}

BOOL CBasicChatRightBody::IsVnCheck( const CString& strOrigin )
{
#if defined( VN_PARAM )
	// 베트남 문자 조합 체크 
	if ( STRUTIL::CheckVietnamString( strOrigin ) )
	{
		AddStringToSystemMessage( ID2GAMEINTEXT("CHAT_VN_CHECK_ERROR") , NS_UITEXTCOLOR::NEGATIVE );
		return FALSE;
	}
#endif 
	return TRUE;
}
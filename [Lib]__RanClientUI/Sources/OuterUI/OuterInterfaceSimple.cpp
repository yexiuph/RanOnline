#include "pch.h"
#include "OuterInterface.h"
#include "ModalWindow.h"

#include "SelectServerPage.h"
#include "LoginPage.h"
#include "SelectCharacterPage.h"
#include "CreateCharacterPage.h"
#include "DxGlobalStage.h"
#include "WaitDialogue.h"
#include "SelectCharacterLeftPage.h"
#include "MessageWindow.h"
#include "RANPARAM.h"
#include "GameTextControl.h"
#include "NewCreateCharPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void COuterInterface::WAITSERVER_DIALOGUE_OPEN ( const CString& strMessage, const int nAction, const float fTimer )
{
	if ( IsVisibleGroup ( WAIT_DISPLAY ) ) return ;

	m_pWaitDisplay->SetPrevPageID ( GetRenderStage () );

	ShowGroupFocus ( WAIT_DISPLAY );	
	m_pWaitDisplay->SetDisplayMessage ( strMessage, nAction, fTimer );
}

void COuterInterface::WAITSERVER_DIALOGUE_CLOSE ()
{
	HideGroup ( WAIT_DISPLAY );
}

UIGUID COuterInterface::GetRenderStage ()
{
	CUIControl* pFocusControl = GetFocusControl ();
	if ( pFocusControl ) 
	{
		UIGUID cID = pFocusControl->GetWndID ();
		switch ( cID )
		{
		case MODAL_WINDOW_OUTER:	return m_pModalWindow->GetPrevPageID ();
		case WAIT_DISPLAY:			return m_pWaitDisplay->GetPrevPageID ();
		default:					return cID;
		}		
	}
	return NO_ID;
}

void COuterInterface::CloseAllWindow ()
{	
	CUIFocusContainer::UICONTROL_FOCUSLIST list = GetFocusList ()->GetFocusList ();
	CUIFocusContainer::UICONTROL_FOCUSLIST_CITER citer = list.begin ();
	CUIFocusContainer::UICONTROL_FOCUSLIST_CITER citer_end = list.end ();
	for ( ; citer != citer_end; ++citer )
	{
		HideGroup ( citer->cID );
	}
}

void COuterInterface::ToChinaAreaPage ( UIGUID cID )
{
	bool bMakeMsg = true;

	if( cID != SELECT_CHINA_AREA_PAGE )
	{
		HideGroup ( cID, bMakeMsg );
		ShowGroupFocus ( SELECT_CHINA_AREA_PAGE, bMakeMsg );
	}
}

void COuterInterface::ToSelectServerPage ( UIGUID cID )
{
	bool bMakeMsg = true;

	if( LOGIN_PAGE == cID || SECPASS_SETPAGE == cID || SECPASS_CHECKPAGE == cID )
	{
		HideGroup( PASS_KEYBOARD, bMakeMsg );
	}

	HideGroup ( cID, bMakeMsg );
	ShowGroupBottom ( COPYRIGHT, bMakeMsg );
#ifdef CH_PARAM
	ShowGroupBottom ( CHINA_TITLE, bMakeMsg );
#endif

	if ( m_pSelectServerPage )
	{	
		m_pSelectServerPage->ResetAll ();		
		ShowGroupFocus ( SELECT_SERVER_PAGE, bMakeMsg );		
	}
}

void	COuterInterface::ToLoginPage ( UIGUID cID )
{
	bool bMakeMsg = true;

	HideGroup ( cID, bMakeMsg );
	ShowGroupBottom ( COPYRIGHT, bMakeMsg );
#ifdef CH_PARAM
	ShowGroupBottom ( CHINA_TITLE, bMakeMsg );
#endif

	if ( m_pLoginPage )
	{
		if( GLCONST_CHAR::nUI_KEYBOARD != 1 )
		{
			ShowGroupFocus( PASS_KEYBOARD, bMakeMsg );
		}

		ShowGroupFocus( LOGIN_PAGE, bMakeMsg );
	}
}

void COuterInterface::ToSecPassPage( UIGUID cID, int nCheckFlag )
{
    bool bMakeMsg = true;

	HideGroup ( cID, bMakeMsg );
	ShowGroupBottom ( COPYRIGHT, bMakeMsg );

	if( nCheckFlag == 0 ) // 비번 설정
	{
		if ( m_pSecPassSetPage )  
		{
			if( GLCONST_CHAR::nUI_KEYBOARD != 1 )
			{
				ShowGroupFocus( PASS_KEYBOARD, bMakeMsg );
			}

			ShowGroupFocus( SECPASS_SETPAGE, bMakeMsg );			
		}
	}
	else if( nCheckFlag == 1 )
	{
		if ( m_pSecPassCheckPage )  
		{
			if( GLCONST_CHAR::nUI_KEYBOARD != 1 )
			{
				ShowGroupFocus( PASS_KEYBOARD, bMakeMsg );
			}

			ShowGroupFocus( SECPASS_CHECKPAGE, bMakeMsg );
		}

	}
}

void	COuterInterface::ToSelectCharacterPage ( UIGUID cID )
{
	bool bMakeMsg = true;

	if( LOGIN_PAGE == cID || SECPASS_SETPAGE == cID || SECPASS_CHECKPAGE == cID )
	{
		HideGroup( SELECT_SERVER_PAGE, bMakeMsg ); // MEMO : 대만일 경우 서버 페이지를 닫아준다.
		HideGroup( PASS_KEYBOARD, bMakeMsg );
	}
	else if( CREATE_CHARACTER_PAGE == cID )
	{
		HideGroup( CREATE_CHARACTER_WEB, bMakeMsg );
	}

	HideGroup ( cID, bMakeMsg );
	HideGroup ( COPYRIGHT, bMakeMsg );
#ifdef CH_PARAM
	HideGroup ( CHINA_TITLE, bMakeMsg );
#endif

	if ( m_pSelectServerPage )
	{
		m_pSelectCharacterPage->ResetAll ();
		ShowGroupFocus ( SELECT_CHARACTER_LEFTPAGE, bMakeMsg );
		ShowGroupFocus ( SELECT_CHARACTER_PAGE, bMakeMsg );
	}
}

void	COuterInterface::ToCreateCharacterPage ( UIGUID cID )
{
	bool bMakeMsg = true;

	if( LOGIN_PAGE == cID || SECPASS_SETPAGE == cID || SECPASS_CHECKPAGE == cID )
	{
		HideGroup( PASS_KEYBOARD, bMakeMsg );
	}

	HideGroup ( cID, bMakeMsg );
	HideGroup ( SELECT_CHARACTER_PAGE, bMakeMsg );
	HideGroup ( COPYRIGHT, bMakeMsg );
#ifdef CH_PARAM
	HideGroup ( CHINA_TITLE, bMakeMsg );
#endif

	if ( m_pSelectServerPage )
	{
		ShowGroupFocus ( CREATE_CHARACTER_PAGE, bMakeMsg );
	}
}


const char* COuterInterface::MakeString ( const char* szFormat, ... )
{
	const int MAX_STRLENGTH = 256;

	va_list argList;
	static	char szBuffer[MAX_STRLENGTH];

	va_start(argList, szFormat);
	StringCbVPrintf(szBuffer, MAX_STRLENGTH, szFormat, argList);
	va_end(argList);

	return szBuffer;
}

BOOL	COuterInterface::LoadClientVersion ( int& nPatchVer, int& nGameVer )
{
	//	NOTE
	//		런처에도 버전을 로드하는 부분이 있습니다.
	//		변경될 경우 반드시 체크 해야합니다.
	//		AutoPatchManDlgEx.cpp!!!

	const CString strVersionFileName = "cVer.bin";

	CString strTemp;
	strTemp.Format ( "%s\\%s", DxGlobalStage::GetInstance().GetAppPath(), strVersionFileName );
	FILE* cfp = NULL;
	fopen_s(&cfp, strTemp.GetString(), "rb");
	if ( !cfp )
	{
		return FALSE;
	}
	if ( 1 != fread ( &nPatchVer, sizeof ( int ), 1, cfp ) )
	{
		fclose ( cfp );
		return FALSE;
	}
	if ( 1 != fread ( &nGameVer, sizeof ( int ), 1, cfp ) )
	{
		fclose ( cfp );		
		return FALSE;
	}
	fclose ( cfp );

	return TRUE;
}

void	COuterInterface::START_WAIT_TIME ( float fTime )
{
	m_bUseWaitTime = TRUE;
	m_fWaitTime = fTime;
}

void	COuterInterface::UPDATE_WAIT_TIME ( float fElapsedTime )
{
	if ( m_bUseWaitTime )
	{
		m_fWaitTime -= fElapsedTime;

		if ( m_fWaitTime < 0.0f ) m_bUseWaitTime = FALSE;
	}
}

HRESULT COuterInterface::FinalCleanup ()
{
	HRESULT hr;

	hr = CUIMan::FinalCleanup ();
	if ( FAILED ( hr ) ) return hr;

	return S_OK;
}

void	COuterInterface::DeleteCharacter ()
{
	//	캐릭터 삭제 요청.
	//	Step 1.	2차 비밀번호 서버 전송
	//
	// 컨트롤로 부터 아이디와 암호를 가져와 서버에 전송
	CNetClient* pNetClient = DxGlobalStage::GetInstance().GetNetClient ();
	SCHARINFO_LOBBY* pCharInfo = m_pSelectCharacterLeftPage->GetDelCharInfo ();

	if ( !pCharInfo ) return;

	switch ( RANPARAM::emSERVICE_TYPE )
	{
	case EMSERVICE_KOREA:
		pNetClient->DaumSndDelCharacter ( pCharInfo->m_dwCharID );
		break;

	case EMSERVICE_JAPAN:
		pNetClient->ExciteSndDelCharacter ( pCharInfo->m_dwCharID );
		break;

	//case EMSERVICE_GSP:
	//	pNetClient->GspSndDelCharacter ( pCharInfo->m_dwCharID );
	//	break;

	case EMSERVICE_MALAYSIA_CN:
	case EMSERVICE_MALAYSIA_EN:
	case EMSERVICE_PHILIPPINES:
	case EMSERVICE_VIETNAM:
		pNetClient->TerraSndDelCharacter ( pCharInfo->m_dwCharID );
		break;

	case EMSERVICE_CHINA:
		{
			CString strTemp = COuterInterface::GetInstance().GetModalWindow()->GetEditString ();
			pNetClient->ChinaSndDelCharacter ( pCharInfo->m_dwCharID, strTemp.GetString () );
		}
		break;

	case EMSERVICE_GLOBAL:
		{
			pNetClient->GsSndDelCharacter ( pCharInfo->m_dwCharID );
		}
		break;

	default: // 대만 태국
		{
			CString strTemp = COuterInterface::GetInstance().GetModalWindow()->GetEditString ();
			pNetClient->SndDelCharacter ( pCharInfo->m_dwCharID, strTemp.GetString () );
		}
		break;
	};
}

void	COuterInterface::SetConnectServerInfo ( const int nServerGroup, const int nServerChannel )
{
	m_nServerGroup = nServerGroup;
	m_nServerChannel = nServerChannel;
}

void	COuterInterface::GetConnectServerInfo ( int& nServerGroup, int& nServerChannel )
{
	nServerGroup = m_nServerGroup;
	nServerChannel = m_nServerChannel;
}

void COuterInterface::OpenMessageWindow ( UIGUID cID, BOOL bOpen )
{
	if ( !m_pMessageWindow ) return;

	if ( bOpen )
	{
		if ( cID == SECPASS_SETPAGE )
		{
			m_pMessageWindow->SetMessage( ID2GAMEEXTEXT ( "SECPASS_SETPAGE_TEXT") );
		}
		else if ( cID == SECPASS_CHECKPAGE )
		{
			m_pMessageWindow->SetMessage( ID2GAMEEXTEXT ( "SECPASS_CHECKPAGE_TEXT") );
		}

		ShowGroupTop( MESSAGE_WINDOW );
	}
	else
	{
		HideGroup( MESSAGE_WINDOW );
	}
}

const UIRECT COuterInterface::GetBackImageRect()	
{ 
	return m_pNewCreateCharPage->m_pBackImage->GetGlobalPos();
}